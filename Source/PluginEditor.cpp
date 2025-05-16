#include "PluginProcessor.h"
#include "PluginEditor.h"


SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    peakFreqSliderAttachment(audioProcessor.apvts, PEAK_FREQ_PARAM_NAME, peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, PEAK_GAIN_PARAM_NAME, peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, PEAK_QUALITY_PARAM_NAME, peakQualitySlider),
    lowCutFreqSliderAttachment(audioProcessor.apvts, LOW_CUT_FREQ_PARAM_NAME, lowCutFreqSlider),
    highCutFreqSliderAttachment(audioProcessor.apvts, HIGH_CUT_FREQ_PARAM_NAME, highCutFreqSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, LOW_CUT_SLOPE_PARAM_NAME, lowCutSlopeSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, HIGH_CUT_SLOPE_PARAM_NAME, highCutSlopeSlider)
{
    for (auto *comp : getComps()) {
        addAndMakeVisible(comp);
    }

    setSize (600, 400);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleEQAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component*> SimpleEQAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider
    };
}
