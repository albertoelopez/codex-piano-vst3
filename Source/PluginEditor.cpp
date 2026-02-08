#include "PluginEditor.h"

CodexPianoVST3AudioProcessorEditor::CodexPianoVST3AudioProcessorEditor (CodexPianoVST3AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setupSlider (gainSlider, gainLabel, "Gain");
    setupSlider (brightnessSlider, brightnessLabel, "Brightness");
    setupSlider (releaseSlider, releaseLabel, "Release");
    setupSlider (reverbSlider, reverbLabel, "Reverb");

    gainAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "gain", gainSlider);
    brightnessAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "brightness", brightnessSlider);
    releaseAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "release", releaseSlider);
    reverbAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "reverb", reverbSlider);

    setSize (520, 260);
}

void CodexPianoVST3AudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 74, 20);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (227, 181, 84));
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB (45, 52, 65));

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colours::whitesmoke);

    addAndMakeVisible (slider);
    addAndMakeVisible (label);
}

void CodexPianoVST3AudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient gradient (
        juce::Colour::fromRGB (18, 24, 34), 0.0f, 0.0f,
        juce::Colour::fromRGB (40, 28, 20), 0.0f, static_cast<float> (getHeight()), false);

    g.setGradientFill (gradient);
    g.fillAll();

    g.setColour (juce::Colour::fromRGB (227, 181, 84));
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawFittedText ("Codex Piano VST3", 20, 14, getWidth() - 40, 34, juce::Justification::centred, 1);

    g.setColour (juce::Colours::whitesmoke.withAlpha (0.75f));
    g.setFont (juce::FontOptions (13.0f));
    g.drawFittedText ("Synthesized piano-style instrument built with JUCE", 20, 44, getWidth() - 40, 20,
                      juce::Justification::centred, 1);
}

void CodexPianoVST3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (74);

    const auto cell = area.getWidth() / 4;
    const auto h = area.getHeight();

    auto place = [h] (juce::Rectangle<int> bounds, juce::Slider& slider, juce::Label& label)
    {
        label.setBounds (bounds.removeFromTop (20));
        slider.setBounds (bounds.withSizeKeepingCentre (bounds.getWidth() - 10, h - 20));
    };

    place (area.removeFromLeft (cell), gainSlider, gainLabel);
    place (area.removeFromLeft (cell), brightnessSlider, brightnessLabel);
    place (area.removeFromLeft (cell), releaseSlider, releaseLabel);
    place (area, reverbSlider, reverbLabel);
}
