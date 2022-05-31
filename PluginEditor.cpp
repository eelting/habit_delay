/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
HabitDelayAudioProcessorEditor::HabitDelayAudioProcessorEditor (HabitDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setLookAndFeel(&customLookAndFeel);
    setSize(406, 500);
    //setSize (4 * margin + 3 * sliderBoxSide, 4 * margin + 2 * sliderBoxSide + 3 * labelHeight);
    
    addAndMakeVisible(levelSlider);
    levelSlider.setRange(0, 1);
    levelSlider.onDragEnd = [&] () {
        audioProcessor.setLevel(levelSlider.getValue());
    };
    levelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    levelSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(levelLabel);
    levelLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    levelLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    levelLabel.setText("Level", juce::dontSendNotification);
    levelLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(feedbackSlider);
    feedbackSlider.setRange(0, 1);
    feedbackSlider.onDragEnd = [&] () {
        audioProcessor.setFeedback(feedbackSlider.getValue());
    };
    feedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(feedbackLabel);
    feedbackLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    feedbackLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(delayRateSlider);
    delayRateSlider.setRange(1, 6, 1);
    delayRateSlider.onDragEnd = [&] () {
        audioProcessor.setDelayRate(delayRateSlider.getValue());
    };
    delayRateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(delayRateLabel);
    delayRateLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    delayRateLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    delayRateLabel.setText("Delay Rate", juce::dontSendNotification);
    delayRateLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setRange(1, 20000);
    cutoffSlider.onValueChange = [&] () {
        audioProcessor.updateFilter(cutoffSlider.getValue());
    };
    cutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    cutoffSlider.setSkewFactorFromMidPoint(1000);
    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    cutoffLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(loopSpreadSlider);
    loopSpreadSlider.setRange(0, audioProcessor.getSampleRate() * audioProcessor.getLoopBufferSizeInSeconds());
    loopSpreadSlider.onDragEnd = [&] () {
        audioProcessor.setLoopSpread(loopSpreadSlider.getValue());
    };
    loopSpreadSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    loopSpreadSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(loopSpreadLabel);
    loopSpreadLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    loopSpreadLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    loopSpreadLabel.setText("Spread", juce::dontSendNotification);
    loopSpreadLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(loopScanSlider);
    loopScanSlider.setRange(0, audioProcessor.getSampleRate() * audioProcessor.getLoopBufferSizeInSeconds());
    loopScanSlider.onDragEnd = [&] () {
        audioProcessor.setLoopScan(loopScanSlider.getValue());
    };
    loopScanSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    loopScanSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(loopScanLabel);
    loopScanLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    loopScanLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    loopScanLabel.setText("Scan", juce::dontSendNotification);
    loopScanLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(collectModeButton);
    collectModeButton.onClick = [&] {
        audioProcessor.toggleCollectMode(collectModeButton.getToggleState());
    };
    collectModeButton.setColour(juce::ToggleButton::textColourId, juce::Colours::black);
    collectModeButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
    collectModeButton.setButtonText("Collect Mode");
}

HabitDelayAudioProcessorEditor::~HabitDelayAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void HabitDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Image background = juce::ImageCache::getFromMemory (BinaryData::habitDelay_png, BinaryData::habitDelay_pngSize);
    g.drawImageAt (background, 0, 0);
}

void HabitDelayAudioProcessorEditor::resized()
{
    levelSlider.setBounds(offset + margin, top + margin, sliderBoxSide, sliderBoxSide);
    levelLabel.setBounds(offset + margin, top + margin + sliderBoxSide, sliderBoxSide, labelHeight);
    feedbackSlider.setBounds(offset + 2 * margin + sliderBoxSide, top + margin, sliderBoxSide, sliderBoxSide);
    feedbackLabel.setBounds(offset + 2 * margin + sliderBoxSide, top + margin + sliderBoxSide, sliderBoxSide, labelHeight);
    delayRateSlider.setBounds(offset + 3 * margin + 2 * sliderBoxSide, top + margin, sliderBoxSide, sliderBoxSide);
    delayRateLabel.setBounds(offset + 3 * margin + 2 * sliderBoxSide, top + margin + sliderBoxSide, sliderBoxSide, labelHeight);
    
    cutoffSlider.setBounds(offset + margin, top + 2 * margin + sliderBoxSide + labelHeight, sliderBoxSide, sliderBoxSide);
    cutoffLabel.setBounds(offset + margin, top + 2 * margin + 2 * sliderBoxSide + labelHeight, sliderBoxSide, labelHeight);
    loopSpreadSlider.setBounds(offset + 2 * margin + sliderBoxSide, top + 2 * margin + sliderBoxSide + labelHeight, sliderBoxSide, sliderBoxSide);
    loopSpreadLabel.setBounds(offset + 2 * margin + sliderBoxSide, top + 2 * margin + 2 * sliderBoxSide + labelHeight, sliderBoxSide, labelHeight);
    loopScanSlider.setBounds(offset + 3 * margin + 2 * sliderBoxSide, top + 2 * margin + sliderBoxSide + labelHeight, sliderBoxSide, sliderBoxSide);
    loopScanLabel.setBounds(offset + 3 * margin + 2 * sliderBoxSide, top + 2 * margin + 2 * sliderBoxSide + labelHeight, sliderBoxSide, labelHeight);
    
    collectModeButton.setBounds(offset + margin, top + 3 * margin + 2 * sliderBoxSide + 2 * labelHeight, sliderBoxSide, labelHeight);
}
