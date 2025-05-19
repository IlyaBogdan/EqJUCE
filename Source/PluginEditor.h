#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#define SLIDER_FILL_COLOR juce::Colour(97u, 18u, 167u)
#define SLIDER_BORDER_COLOR juce::Colour(255u, 154u, 1u)
#define SLIDER_FONT_COLOR juce::Colours::white

enum FFTOrder
{
    oreder2048 = 11,
    order4096 = 12,
    order8192 = 13
};

template<typename BlockType>
struct FFTDataGenerator
{
    public:
        void produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, const float negativeInfinity)
        {
            const auto fftSize = getFFTSize();

            fftData.assign(fftData.size(), 0);
            auto* readIndex = audioData.getReadPointer(0);
            std::copy(readIndex, readIndex + fftSize, fftData.begin());

            window->multiplyWithWindowingTable(fftData.data(), fftSize);
            forwardFFT->performFrequencyOnlyForwardTransform(fftData.data());

            int numBins = (int)fftSize / 2;

            for (int i = 0; i < numBins; ++i) {
                fftData[i] /= (float)numBins;
            }

            for (int i = 0; i < numBins; ++i) {
                fftData[i] = juce::Decibels::gainToDecibels(fftData[i], negativeInfinity);
            }

            fftDataFifo.push(fftData);
        }

        void changeOrder(FFTOrder newOrder)
        {
            order = newOrder;
            auto fftSize = getFFTSize();

            forwardFFT = std::make_unique<juce::dsp::FFT>(order);
            window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);

            fftData.clear();
            fftData.resize(fftSize * 2, 0);

            fftDataFifo.prepare(fftData.size());
        }

        int getFFTSize() const { return 1 << order; }
        int getNumAvailableFFTDataBlocks() const { return fftDataFifo.getNumAvailableForReading(); }
        bool getFFTData(BlockType& fftData) { return fftDataFifo.pull(fftData); }
    private:
        FFTOrder order;
        BlockType fftData;
        std::unique_ptr<juce::dsp::FFT> forwardFFT;
        std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

        Fifo<BlockType> fftDataFifo;
};

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

struct ResponseCurveComponent : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    public:
        ResponseCurveComponent(SimpleEQAudioProcessor&);
        ~ResponseCurveComponent();

        void parameterValueChanged(int parameterIndex, float newValue) override;
        void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
        void timerCallback() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        MonoChain monoChain;
        juce::Image background;
        juce::Atomic<bool> parametersChanged{ false };
        SimpleEQAudioProcessor& audioProcessor;
        SingleChannelSampleInfo<SimpleEQAudioProcessor::BlockType>* leftChannelFifo;
        juce::AudioBuffer<float> monoBuffer;
        FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;


        void updateChain();
        juce::Rectangle<int> getRenderArea();
        juce::Rectangle<int> getAnalisysArea();

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
