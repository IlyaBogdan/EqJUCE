#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#define SLIDER_FILL_COLOR juce::Colour(97u, 18u, 167u)
#define SLIDER_BORDER_COLOR juce::Colour(255u, 154u, 1u)
#define SLIDER_FONT_COLOR juce::Colours::white


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;
};

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix)
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap), suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 12; }
    juce::String getDisplayString() const;

    private:
        LookAndFeel lnf;

        juce::RangedAudioParameter* param;
        juce::String suffix;
};

struct ResponseCurveComponent : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer {

    public:
        ResponseCurveComponent(SimpleEQAudioProcessor&);
        ~ResponseCurveComponent();

        void parameterValueChanged(int parameterIndex, float newValue) override;
        void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
        void timerCallback() override;

        void paint(juce::Graphics& g) override;

    private:
        SimpleEQAudioProcessor& audioProcessor;
        juce::Atomic<bool> parametersChanged{ false };
        MonoChain monoChain;

        void updateChain();
};

class SimpleEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
    public:
        SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor&);
        ~SimpleEQAudioProcessorEditor() override;

        void resized() override;

    private:
        RotarySliderWithLabels peakFreqSlider, peakGainSlider, peakQualitySlider;
        RotarySliderWithLabels lowCutFreqSlider, highCutFreqSlider;
        RotarySliderWithLabels lowCutSlopeSlider, highCutSlopeSlider;

        ResponseCurveComponent responseCurveComponent;

        using APVTS = juce::AudioProcessorValueTreeState;
        using Attachment = APVTS::SliderAttachment;

        Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQualitySliderAttachment;
        Attachment lowCutFreqSliderAttachment, highCutFreqSliderAttachment;
        Attachment lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;

        std::vector<juce::Component*> getComps();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
