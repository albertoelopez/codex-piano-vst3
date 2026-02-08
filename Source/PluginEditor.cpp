#include "PluginEditor.h"

class CodexPianoVST3AudioProcessorEditor::StudioKnobLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g,
                           int x,
                           int y,
                           int width,
                           int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override
    {
        const auto bounds = juce::Rectangle<float> (static_cast<float> (x),
                                                    static_cast<float> (y),
                                                    static_cast<float> (width),
                                                    static_cast<float> (height)).reduced (5.0f);

        const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto centre = bounds.getCentre();
        const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        g.setColour (juce::Colours::black.withAlpha (0.38f));
        g.fillEllipse (centre.x - radius, centre.y - radius + 3.0f, radius * 2.0f, radius * 2.0f);

        juce::ColourGradient outerMetal (juce::Colour::fromRGB (228, 232, 238),
                                         centre.x - radius * 0.7f, centre.y - radius,
                                         juce::Colour::fromRGB (92, 99, 109),
                                         centre.x + radius * 0.9f, centre.y + radius * 0.9f, false);
        outerMetal.addColour (0.48, juce::Colour::fromRGB (175, 183, 194));
        g.setGradientFill (outerMetal);
        g.fillEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

        g.setColour (juce::Colour::fromRGB (20, 25, 33).withAlpha (0.72f));
        g.drawEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.4f);

        const auto knurlRadius = radius * 0.90f;
        for (int i = 0; i < 28; ++i)
        {
            const auto a = juce::MathConstants<float>::twoPi * static_cast<float> (i) / 28.0f;
            const auto p1 = centre.getPointOnCircumference (knurlRadius, a);
            const auto p2 = centre.getPointOnCircumference (knurlRadius * 0.82f, a);
            g.setColour (i % 2 == 0 ? juce::Colour::fromRGB (245, 247, 250).withAlpha (0.20f)
                                    : juce::Colour::fromRGB (28, 31, 38).withAlpha (0.35f));
            g.drawLine ({ p1, p2 }, 1.0f);
        }

        const auto core = radius * 0.70f;
        juce::ColourGradient coreShade (juce::Colour::fromRGB (157, 165, 176),
                                        centre.x - core * 0.5f, centre.y - core,
                                        juce::Colour::fromRGB (78, 84, 94),
                                        centre.x + core * 0.7f, centre.y + core, false);
        g.setGradientFill (coreShade);
        g.fillEllipse (centre.x - core, centre.y - core, core * 2.0f, core * 2.0f);

        // Plastic pointer tip.
        juce::Path pointer;
        const auto tipLength = radius * 0.62f;
        const auto tipWidth = radius * 0.11f;
        pointer.addRoundedRectangle (-tipWidth, -tipLength, tipWidth * 2.0f, tipLength * 0.45f, tipWidth * 0.45f);
        pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
        g.setColour (juce::Colour::fromRGB (247, 242, 226));
        g.fillPath (pointer);

        g.setColour (juce::Colour::fromRGB (33, 29, 24).withAlpha (0.70f));
        g.strokePath (pointer, juce::PathStrokeType (1.0f));

        g.setColour (juce::Colour::fromRGB (255, 255, 255).withAlpha (0.25f));
        juce::Path highlightArc;
        highlightArc.addCentredArc (centre.x, centre.y, radius * 0.93f, radius * 0.93f, 0.0f, rotaryStartAngle, angle, true);
        g.strokePath (highlightArc, juce::PathStrokeType (2.2f));

        g.setColour (juce::Colour::fromRGB (232, 235, 242).withAlpha (0.45f));
        g.fillEllipse (centre.x - radius * 0.12f, centre.y - radius * 0.48f, radius * 0.24f, radius * 0.20f);
    }
};

CodexPianoVST3AudioProcessorEditor::CodexPianoVST3AudioProcessorEditor (CodexPianoVST3AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    knobLookAndFeel = std::make_unique<StudioKnobLookAndFeel>();

    setupSlider (gainSlider, gainLabel, "Gain");
    setupSlider (brightnessSlider, brightnessLabel, "Brightness");
    setupSlider (releaseSlider, releaseLabel, "Release");
    setupSlider (reverbSlider, reverbLabel, "Reverb");

    gainAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "gain", gainSlider);
    brightnessAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "brightness", brightnessSlider);
    releaseAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "release", releaseSlider);
    reverbAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "reverb", reverbSlider);

    setSize (760, 420);
}

void CodexPianoVST3AudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setLookAndFeel (knobLookAndFeel.get());
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 74, 20);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour::fromRGB (233, 236, 239));
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (9, 13, 18).withAlpha (0.7f));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour::fromRGB (225, 223, 218));
    label.setFont (juce::FontOptions (15.0f, juce::Font::bold));

    addAndMakeVisible (slider);
    addAndMakeVisible (label);
}

void CodexPianoVST3AudioProcessorEditor::drawPianoBackdrop (juce::Graphics& g)
{
    auto upper = getLocalBounds().removeFromTop (236).toFloat();

    auto synthTop = upper.reduced (18.0f, 14.0f);
    const auto sideW = 20.0f;

    auto leftWood = juce::Rectangle<float> (synthTop.getX(), synthTop.getY(), sideW, synthTop.getHeight());
    auto rightWood = juce::Rectangle<float> (synthTop.getRight() - sideW, synthTop.getY(), sideW, synthTop.getHeight());
    auto centerDeck = synthTop.reduced (sideW, 0.0f);

    juce::ColourGradient wood (juce::Colour::fromRGB (145, 86, 42), 0.0f, leftWood.getY(),
                               juce::Colour::fromRGB (74, 40, 18), 0.0f, leftWood.getBottom(), false);
    g.setGradientFill (wood);
    g.fillRect (leftWood);
    g.fillRect (rightWood);

    juce::ColourGradient brushed (juce::Colour::fromRGB (36, 70, 104), centerDeck.getX(), centerDeck.getY(),
                                  juce::Colour::fromRGB (26, 31, 46), centerDeck.getRight(), centerDeck.getBottom(), false);
    brushed.addColour (0.22, juce::Colour::fromRGB (24, 112, 128));
    brushed.addColour (0.52, juce::Colour::fromRGB (43, 55, 93));
    g.setGradientFill (brushed);
    g.fillRoundedRectangle (centerDeck, 10.0f);

    for (int y = static_cast<int> (centerDeck.getY()); y < static_cast<int> (centerDeck.getBottom()); y += 3)
    {
        const auto alpha = (y % 2 == 0) ? 0.065f : 0.03f;
        g.setColour (juce::Colour::fromRGB (245, 245, 245).withAlpha (alpha));
        g.drawHorizontalLine (y, centerDeck.getX() + 2.0f, centerDeck.getRight() - 2.0f);
    }

    auto logoStrip = centerDeck.removeFromTop (66.0f).reduced (18.0f, 9.0f);
    juce::ColourGradient logoGrad (juce::Colour::fromRGB (20, 22, 35), logoStrip.getX(), logoStrip.getY(),
                                   juce::Colour::fromRGB (14, 44, 66), logoStrip.getRight(), logoStrip.getBottom(), false);
    g.setGradientFill (logoGrad);
    g.fillRoundedRectangle (logoStrip, 9.0f);
    g.setColour (juce::Colour::fromRGB (255, 137, 56).withAlpha (0.35f));
    g.fillRoundedRectangle (logoStrip.removeFromTop (2.0f), 2.0f);

    g.setColour (juce::Colour::fromRGB (237, 248, 255).withAlpha (0.98f));
    g.setFont (juce::FontOptions (38.0f, juce::Font::bold));
    g.drawFittedText ("SYNTH PIANO", logoStrip.toNearestInt(), juce::Justification::centred, 1);

    auto keyBed = centerDeck.reduced (16.0f, 12.0f);
    keyBed.removeFromTop (56.0f);

    g.setColour (juce::Colour::fromRGB (16, 21, 31).withAlpha (0.90f));
    g.fillRoundedRectangle (keyBed.expanded (8.0f, 9.0f), 10.0f);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillRoundedRectangle (keyBed.translated (0.0f, 5.0f), 8.0f);

    constexpr int whiteKeys = 15;
    const auto whiteW = keyBed.getWidth() / static_cast<float> (whiteKeys);
    for (int i = 0; i < whiteKeys; ++i)
    {
        auto white = juce::Rectangle<float> (keyBed.getX() + whiteW * static_cast<float> (i), keyBed.getY(), whiteW - 1.8f, keyBed.getHeight());
        juce::ColourGradient whiteShade (juce::Colour::fromRGB (255, 253, 248), white.getX(), white.getY(),
                                         juce::Colour::fromRGB (188, 194, 205), white.getRight(), white.getBottom(), false);
        g.setGradientFill (whiteShade);
        g.fillRoundedRectangle (white, 2.8f);
        g.setColour (juce::Colour::fromRGB (42, 46, 54).withAlpha (0.22f));
        g.drawRoundedRectangle (white, 2.8f, 0.8f);
    }

    const std::array<int, 10> blackPattern { 0, 1, 3, 4, 5, 7, 8, 10, 11, 13 };
    const auto blackW = whiteW * 0.56f;
    const auto blackH = keyBed.getHeight() * 0.61f;
    for (const auto idx : blackPattern)
    {
        auto black = juce::Rectangle<float> (keyBed.getX() + whiteW * static_cast<float> (idx + 1) - blackW * 0.5f,
                                             keyBed.getY() - 2.0f,
                                             blackW,
                                             blackH);
        juce::ColourGradient blackShade (juce::Colour::fromRGB (70, 76, 84), black.getX(), black.getY(),
                                         juce::Colour::fromRGB (8, 10, 13), black.getRight(), black.getBottom(), false);
        g.setGradientFill (blackShade);
        g.fillRoundedRectangle (black, 3.0f);
        g.setColour (juce::Colours::white.withAlpha (0.08f));
        g.drawRoundedRectangle (black.reduced (0.8f), 2.5f, 0.7f);
    }
}

void CodexPianoVST3AudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient gradient (juce::Colour::fromRGB (7, 18, 30), 0.0f, 0.0f,
                                   juce::Colour::fromRGB (34, 24, 15), static_cast<float> (getWidth()), static_cast<float> (getHeight()), false);
    gradient.addColour (0.33, juce::Colour::fromRGB (17, 54, 72));
    gradient.addColour (0.68, juce::Colour::fromRGB (40, 26, 54));

    g.setGradientFill (gradient);
    g.fillAll();

    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillRoundedRectangle (getLocalBounds().toFloat().reduced (8.0f), 12.0f);

    drawPianoBackdrop (g);

    auto controlPanel = getLocalBounds().removeFromBottom (174).reduced (18, 14).toFloat();
    juce::ColourGradient panelGrad (juce::Colour::fromRGB (27, 31, 40), controlPanel.getX(), controlPanel.getY(),
                                    juce::Colour::fromRGB (10, 14, 20), controlPanel.getRight(), controlPanel.getBottom(), false);
    panelGrad.addColour (0.5, juce::Colour::fromRGB (30, 47, 63));
    g.setGradientFill (panelGrad);
    g.fillRoundedRectangle (controlPanel, 16.0f);
    g.setColour (juce::Colour::fromRGB (255, 148, 66).withAlpha (0.3f));
    g.fillRoundedRectangle (controlPanel.removeFromTop (2.0f), 1.0f);
    g.setColour (juce::Colour::fromRGB (220, 224, 233).withAlpha (0.23f));
    g.drawRoundedRectangle (controlPanel, 16.0f, 1.0f);
}

void CodexPianoVST3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds().removeFromBottom (174).reduced (30, 14);

    const auto cell = area.getWidth() / 4;
    const auto h = area.getHeight() - 10;

    auto place = [h] (juce::Rectangle<int> bounds, juce::Slider& slider, juce::Label& label)
    {
        label.setBounds (bounds.removeFromTop (22));
        slider.setBounds (bounds.withSizeKeepingCentre (bounds.getWidth() - 24, h - 12));
    };

    place (area.removeFromLeft (cell), gainSlider, gainLabel);
    place (area.removeFromLeft (cell), brightnessSlider, brightnessLabel);
    place (area.removeFromLeft (cell), releaseSlider, releaseLabel);
    place (area, reverbSlider, reverbLabel);
}
