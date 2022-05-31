/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <math.h>

//==============================================================================
/**
*/
class HabitDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    HabitDelayAudioProcessor();
    ~HabitDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    inline int getLoopSpreadPosition()
    {
        return getLoopScanPosition() >= loopSpread ?
                getLoopScanPosition() - loopSpread :
                loopBuffer.getNumSamples() - loopSpread + getLoopScanPosition();
    };
    
    inline int getLoopScanPosition()
    {
        return loopPosition >= loopScan ?
                loopPosition - loopScan :
                loopBuffer.getNumSamples() - loopScan + loopPosition;
    };
    
    void setLevel(float newLevel) { level = newLevel; };
    
    void setFeedback(float newFeedback) { delayFade = newFeedback; };
    
    void setDelayRate(float newDelayRate) {
        delayRate = pow(2.0, newDelayRate) / 16;
    };
    
    void setLoopSpread(float newLoopSpread) { loopSpread = newLoopSpread; };
    int getLoopBufferSizeInSeconds() { return loopBufferSizeInSeconds; };
    
    void setLoopScan(float newLoopScan) { loopScan = newLoopScan; };

    int getDelayOutPosition()
    {
        float bps = bpm / 60;
        float secPerBeat = 1 / bps;
        float samplesOfDelay = (delayRate * secPerBeat * getSampleRate());
        return delayPosition >= samplesOfDelay ?
               delayPosition - samplesOfDelay :
               delayBuffer.getNumSamples() - samplesOfDelay + delayPosition;
    }
    
    int getDelayPosition() { return delayPosition; };
    
    void loopPositionIn(int totalNumInputChannels, juce::AudioBuffer<float>& buffer);
    
    void circularBufferCopy(int totalNumInputChannels, juce::AudioBuffer<float>& inBuffer, juce::AudioBuffer<float>& outBuffer, int copyLen, int inPosition, int outPosition, float delayFade = 1);

    void updateFilter(float freq);
    
    void toggleCollectMode(bool clicked) { collectMode = clicked; };
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HabitDelayAudioProcessor)
    
    int lastSampleRate{ };
    juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter<float>, juce::dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;
    float cutoff { 1 };
    
    juce::AudioBuffer<float> loopBuffer;
    float level { 0 };
    float loopFade { .5 };
    int loopBufferSizeInSeconds { 10 };
    int loopPosition { 0 };
    int loopSpread { 0 };
    int loopScan { 0 };
    
    juce::AudioBuffer<float> delayBuffer;
    const float MAX_DELAY_RATE { 7 };
    float delayRate { 1 };
    int bpm { 128 };
    int delayPosition { 0 };
    float delayFade { 0 };
    
    bool collectMode { false };
    bool feedMode { false };
};
