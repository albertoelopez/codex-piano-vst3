#pragma once

#include <JuceHeader.h>

class PianoSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

class PianoVoice final : public juce::SynthesiserVoice
{
public:
    PianoVoice();
    using juce::SynthesiserVoice::renderNextBlock;

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;

    void updateVoiceParameters (float brightnessAmount, float releaseAmount);

private:
    void updateDeltas();

    static constexpr int numPartials = 4;

    double sampleRate = 44100.0;
    double currentFrequency = 440.0;

    std::array<double, numPartials> phase {};
    std::array<double, numPartials> phaseDelta {};
    std::array<float, numPartials> partialGains { 1.0f, 0.52f, 0.30f, 0.15f };

    float level = 0.0f;
    float envelope = 0.0f;
    float decayCoeff = 0.9995f;
    float releaseCoeff = 0.9990f;
    float noteOnSamples = 0.0f;
    float attackNoise = 0.0f;

    bool keyIsDown = false;

    juce::Random random;
};

class CodexPianoVST3AudioProcessor final : public juce::AudioProcessor
{
public:
    CodexPianoVST3AudioProcessor();
    ~CodexPianoVST3AudioProcessor() override = default;
    using juce::AudioProcessor::processBlock;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #if ! JucePlugin_IsMidiEffect
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::Synthesiser synth;
    juce::Reverb reverb;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodexPianoVST3AudioProcessor)
};
