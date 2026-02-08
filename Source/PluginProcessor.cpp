#include "PluginProcessor.h"
#include "PluginEditor.h"

PianoVoice::PianoVoice() = default;

bool PianoVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<PianoSound*> (sound) != nullptr;
}

void PianoVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentFrequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    sampleRate = getSampleRate() > 0.0 ? getSampleRate() : 44100.0;

    updateDeltas();

    level = juce::jlimit (0.0f, 1.0f, velocity);
    envelope = 1.0f;
    noteOnSamples = 0.0f;
    attackNoise = 1.0f;
    keyIsDown = true;
}

void PianoVoice::stopNote (float, bool allowTailOff)
{
    keyIsDown = false;

    if (! allowTailOff)
    {
        clearCurrentNote();
        envelope = 0.0f;
    }
}

void PianoVoice::updateVoiceParameters (float brightnessAmount, float releaseAmount)
{
    const auto b = juce::jlimit (0.0f, 1.0f, brightnessAmount);

    partialGains[0] = 1.0f;
    partialGains[1] = 0.30f + 0.45f * b;
    partialGains[2] = 0.15f + 0.30f * b;
    partialGains[3] = 0.05f + 0.20f * b;

    const auto decaySeconds = 1.0f + 4.0f * juce::jlimit (0.0f, 1.0f, releaseAmount);
    const auto releaseSeconds = 0.08f + 2.4f * juce::jlimit (0.0f, 1.0f, releaseAmount);

    decayCoeff = std::exp (-1.0f / (static_cast<float> (sampleRate) * decaySeconds));
    releaseCoeff = std::exp (-1.0f / (static_cast<float> (sampleRate) * releaseSeconds));
}

void PianoVoice::updateDeltas()
{
    static constexpr std::array<double, numPartials> multipliers { 1.0, 2.0, 3.0, 4.0 };

    for (size_t i = 0; i < multipliers.size(); ++i)
        phaseDelta[i] = juce::MathConstants<double>::twoPi * currentFrequency * multipliers[i] / sampleRate;
}

void PianoVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    auto* left = outputBuffer.getWritePointer (0);
    auto* right = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1) : nullptr;

    while (--numSamples >= 0)
    {
        float sampleValue = 0.0f;

        for (size_t i = 0; i < phase.size(); ++i)
        {
            sampleValue += partialGains[i] * std::sin (phase[i]);
            phase[i] += phaseDelta[i];

            if (phase[i] >= juce::MathConstants<double>::twoPi)
                phase[i] -= juce::MathConstants<double>::twoPi;
        }

        const auto attackSamples = static_cast<float> (sampleRate * 0.008);
        const auto attackEnv = noteOnSamples < attackSamples ? noteOnSamples / attackSamples : 1.0f;

        if (noteOnSamples < static_cast<float> (sampleRate * 0.02))
        {
            const auto noiseAmt = 0.08f * attackNoise;
            sampleValue += noiseAmt * (random.nextFloat() * 2.0f - 1.0f);
            attackNoise *= 0.9985f;
        }

        sampleValue *= envelope * attackEnv * level;

        if (keyIsDown)
            envelope *= decayCoeff;
        else
            envelope *= releaseCoeff;

        if (envelope < 0.00008f)
        {
            clearCurrentNote();
            break;
        }

        left[startSample] += sampleValue;
        if (right != nullptr)
            right[startSample] += sampleValue;

        ++startSample;
        noteOnSamples += 1.0f;
    }
}

CodexPianoVST3AudioProcessor::CodexPianoVST3AudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    synth.clearVoices();
    for (int i = 0; i < 16; ++i)
        synth.addVoice (new PianoVoice());

    synth.clearSounds();
    synth.addSound (new PianoSound());
}

void CodexPianoVST3AudioProcessor::prepareToPlay (double newSampleRate, int)
{
    synth.setCurrentPlaybackSampleRate (newSampleRate);

    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.55f;
    reverbParams.damping = 0.35f;
    reverbParams.width = 0.9f;
    reverbParams.wetLevel = 0.2f;
    reverbParams.dryLevel = 0.8f;
    reverb.setParameters (reverbParams);
}

void CodexPianoVST3AudioProcessor::releaseResources() {}

#if ! JucePlugin_IsMidiEffect
bool CodexPianoVST3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}
#endif

void CodexPianoVST3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const auto brightness = apvts.getRawParameterValue ("brightness")->load();
    const auto release = apvts.getRawParameterValue ("release")->load();
    const auto gainDb = apvts.getRawParameterValue ("gain")->load();
    const auto reverbMix = apvts.getRawParameterValue ("reverb")->load();

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<PianoVoice*> (synth.getVoice(i)))
            voice->updateVoiceParameters (brightness, release);

    buffer.clear();
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.40f + 0.45f * reverbMix;
    reverbParams.damping = 0.30f;
    reverbParams.width = 0.9f;
    reverbParams.wetLevel = 0.15f + 0.45f * reverbMix;
    reverbParams.dryLevel = 1.0f - 0.5f * reverbMix;
    reverb.setParameters (reverbParams);

    if (buffer.getNumChannels() > 1)
        reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), buffer.getNumSamples());
    else
        reverb.processMono (buffer.getWritePointer (0), buffer.getNumSamples());

    buffer.applyGain (juce::Decibels::decibelsToGain (gainDb));
}

juce::AudioProcessorEditor* CodexPianoVST3AudioProcessor::createEditor()
{
    return new CodexPianoVST3AudioProcessorEditor (*this);
}

bool CodexPianoVST3AudioProcessor::hasEditor() const
{
    return true;
}

const juce::String CodexPianoVST3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CodexPianoVST3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CodexPianoVST3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CodexPianoVST3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CodexPianoVST3AudioProcessor::getTailLengthSeconds() const
{
    return 3.5;
}

int CodexPianoVST3AudioProcessor::getNumPrograms()
{
    return 1;
}

int CodexPianoVST3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void CodexPianoVST3AudioProcessor::setCurrentProgram (int) {}

const juce::String CodexPianoVST3AudioProcessor::getProgramName (int)
{
    return {};
}

void CodexPianoVST3AudioProcessor::changeProgramName (int, const juce::String&) {}

void CodexPianoVST3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (const auto state = apvts.copyState(); state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }
}

void CodexPianoVST3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout CodexPianoVST3AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "gain", "Gain", juce::NormalisableRange<float> (-24.0f, 6.0f, 0.1f), -6.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "brightness", "Brightness", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.55f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "release", "Release", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.45f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverb", "Reverb", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.2f));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CodexPianoVST3AudioProcessor();
}
