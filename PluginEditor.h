/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/**
*/
class HabitDelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HabitDelayAudioProcessorEditor (HabitDelayAudioProcessor&);
    ~HabitDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    static juce::String valueToText(float value)
    {
        if (value == 1) {
            return "1/16";
        } else if (value == 2) {
            return "1/8";
        } else if (value == 4) {
            return "1/4";
        } else if (value == 8) {
            return "1/2";
        } else if (value == 16) {
            return "1";
        } else if (value == 32) {
            return "2";
        } else if (value == 64) {
            return "4";
        } else {
            return "error";
        }
    }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HabitDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HabitDelayAudioProcessorEditor)
    
    OtherLookAndFeel customLookAndFeel;
    
    const int sliderBoxSide { 100 };
    const int labelHeight { 20 };
    const int margin { 20 };
    const int offset { 13 };
    const int top { 13};
    
    juce::Slider levelSlider;
    juce::Label levelLabel;
    
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    
    juce::Slider delayRateSlider;
    juce::Label delayRateLabel;
    
    juce::Slider cutoffSlider;
    juce::Label cutoffLabel;
    
    juce::Slider loopSpreadSlider;
    juce::Label loopSpreadLabel;
    
    juce::Slider loopScanSlider;
    juce::Label loopScanLabel;
    
    juce::ToggleButton collectModeButton;
};
