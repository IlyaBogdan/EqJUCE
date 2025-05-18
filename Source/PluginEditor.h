#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

struct ResponseCurveComponent : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer {

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
};

class SimpleEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
    public:
        SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor&);
        ~SimpleEQAudioProcessorEditor() override;

        void resized() override;

    private:
        CustomRotarySlider peakFreqSlider, peakGainSlider, peakQualitySlider;
        CustomRotarySlider lowCutFreqSlider, highCutFreqSlider;
        CustomRotarySlider lowCutSlopeSlider, highCutSlopeSlider;

        ResponseCurveComponent responseCurveComponent;

        using APVTS = juce::AudioProcessorValueTreeState;
        using Attachment = APVTS::SliderAttachment;

        Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQualitySliderAttachment;
        Attachment lowCutFreqSliderAttachment, highCutFreqSliderAttachment;
        Attachment lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;

        std::vector<juce::Component*> getComps();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
