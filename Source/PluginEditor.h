#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CodexPianoVST3AudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit CodexPianoVST3AudioProcessorEditor (CodexPianoVST3AudioProcessor&);
    ~CodexPianoVST3AudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& text);

    CodexPianoVST3AudioProcessor& audioProcessor;

    juce::Slider gainSlider;
    juce::Slider brightnessSlider;
    juce::Slider releaseSlider;
    juce::Slider reverbSlider;

    juce::Label gainLabel;
    juce::Label brightnessLabel;
    juce::Label releaseLabel;
    juce::Label reverbLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> brightnessAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<SliderAttachment> reverbAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodexPianoVST3AudioProcessorEditor)
};
