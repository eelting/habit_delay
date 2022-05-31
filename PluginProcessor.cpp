/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace std;
using namespace juce;

//==============================================================================
HabitDelayAudioProcessor::HabitDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        loopBuffer()
#endif
{
}

HabitDelayAudioProcessor::~HabitDelayAudioProcessor()
{
}

//==============================================================================
const juce::String HabitDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HabitDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HabitDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HabitDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HabitDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HabitDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HabitDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HabitDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HabitDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void HabitDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HabitDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    loopBuffer.setSize(getTotalNumInputChannels(),
                       sampleRate * loopBufferSizeInSeconds,
                       true,
                       true);
    
    lastSampleRate = sampleRate;
    
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();
    
    stateVariableFilter.reset();
    updateFilter(cutoff);
    stateVariableFilter.prepare(spec);
    
    setDelayRate(delayRate);
    
    float bufferDelayRate = pow(2.0, MAX_DELAY_RATE) / 16;
    float bps = bpm / 60;
    float secPerBeat = 1 / bps;
    float samplesOfDelay = (bufferDelayRate * secPerBeat * getSampleRate());
    delayBuffer.setSize(getTotalNumInputChannels(), (float)samplesOfDelay, true, true);
}

void HabitDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void HabitDelayAudioProcessor::updateFilter(float freq)
{
    cutoff = freq;
    stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::highPass;
    stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HabitDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HabitDelayAudioProcessor::loopPositionIn(int totalNumInputChannels, juce::AudioBuffer<float> & buffer)
{
    // loopPosition in
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        if (loopBuffer.getNumSamples() > loopPosition + buffer.getNumSamples()) {
            if (collectMode) {
                loopBuffer.addFrom(channel, loopPosition, buffer, channel, 0, buffer.getNumSamples());
            } else {
                loopBuffer.copyFrom(channel, loopPosition, buffer, channel, 0, buffer.getNumSamples());
            }
        } else {
            auto loopBufferRemaining = loopBuffer.getNumSamples() - loopPosition;
            if (collectMode) {
                loopBuffer.addFrom(channel, loopPosition, buffer, channel, 0, loopBufferRemaining);
                loopBuffer.addFrom(channel, 0, buffer, channel, loopBufferRemaining, buffer.getNumSamples() - loopBufferRemaining);
            } else {
                loopBuffer.copyFrom(channel, loopPosition, buffer, channel, 0, loopBufferRemaining);
                loopBuffer.copyFrom(channel, 0, buffer, channel, loopBufferRemaining, buffer.getNumSamples() - loopBufferRemaining);
            }
        }
    }
}

void HabitDelayAudioProcessor::circularBufferCopy(int totalNumInputChannels, juce::AudioBuffer<float>& inBuffer, juce::AudioBuffer<float>& outBuffer, int copyLen, int inPosition, int outPosition, float delayFade)
{
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        if (outBuffer.getNumSamples() > outPosition + copyLen) {
            if (inBuffer.getNumSamples() > inPosition + copyLen) {
                outBuffer.addFrom(channel, outPosition, inBuffer, channel, inPosition, copyLen, delayFade);
            } else {
                auto inBufferRemaining = inBuffer.getNumSamples() - inPosition;
                outBuffer.addFrom(channel, outPosition, inBuffer, channel, inPosition, inBufferRemaining, delayFade);
                outBuffer.addFrom(channel, outPosition + inBufferRemaining, inBuffer, channel, 0, copyLen - inBufferRemaining, delayFade);
            }
        } else {
            auto outBufferRemaining = outBuffer.getNumSamples() - outPosition;
            auto inBufferRemaining = inBuffer.getNumSamples() - inPosition;
            if (inBuffer.getNumSamples() > inPosition + copyLen) {
                outBuffer.addFrom(channel, outPosition, inBuffer, channel, inPosition, outBufferRemaining, delayFade);
                outBuffer.addFrom(channel, 0, inBuffer, channel, inPosition + outBufferRemaining, copyLen - outBufferRemaining, delayFade);
            } else if (outBufferRemaining > inBufferRemaining) {
                // ob [#####-----]
                // ib [########--]
                outBuffer.addFrom(channel, outPosition, inBuffer, channel, inPosition, inBufferRemaining, delayFade);
                // ob [#######---]
                // ib [##########]
                outBuffer.addFrom(channel, outPosition + inBufferRemaining, inBuffer, channel, 0, outBufferRemaining - inBufferRemaining, delayFade);
                // ob [##########]
                // ib [---#######]
                outBuffer.addFrom(channel, 0, inBuffer, channel, outBufferRemaining - inBufferRemaining, copyLen - outBufferRemaining, delayFade);
            } else {
                // ob [########--]
                // ib [#####-----]
                outBuffer.addFrom(channel, outPosition, inBuffer, channel, inPosition, outBufferRemaining, delayFade);
                // ob [##########]
                // ib [#######---]
                outBuffer.addFrom(channel, 0, inBuffer, channel, inPosition + outBufferRemaining, inBufferRemaining - outBufferRemaining, delayFade);
                // ob [---#######]
                // ib [##########]
                outBuffer.addFrom(channel, inBufferRemaining - outBufferRemaining, inBuffer, channel, 0, copyLen - inBufferRemaining, delayFade);
            }
        }
    }
}

void HabitDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    loopPositionIn(totalNumInputChannels, buffer);
    
    delayBuffer.clear(delayPosition, buffer.getNumSamples());
    
    // delay in
    circularBufferCopy(totalNumInputChannels, loopBuffer, delayBuffer, buffer.getNumSamples(), getLoopSpreadPosition(), delayPosition, level);
    if (getLoopScanPosition() != getLoopSpreadPosition()) {
        circularBufferCopy(totalNumInputChannels, loopBuffer, delayBuffer, buffer.getNumSamples(), getLoopScanPosition(), delayPosition, level);
    }
    
    AudioBuffer<float> noFilter;
    noFilter.setSize(buffer.getNumChannels(), buffer.getNumSamples());
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        noFilter.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
    }
    buffer.clear();
    
    // delay out
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        if (delayBuffer.getNumSamples() > getDelayOutPosition() + buffer.getNumSamples()) {
            buffer.addFrom(channel, 0, delayBuffer, channel, getDelayOutPosition(), buffer.getNumSamples());
        } else {
            auto delayBufferRemaining = delayBuffer.getNumSamples() - getDelayOutPosition();
            buffer.addFrom(channel, 0, delayBuffer, channel, getDelayOutPosition(), delayBufferRemaining);
            buffer.addFrom(channel, delayBufferRemaining, delayBuffer, channel, 0, buffer.getNumSamples() - delayBufferRemaining);
        }
    }
    
    // delay feedback
    circularBufferCopy(totalNumInputChannels, delayBuffer, delayBuffer, buffer.getNumSamples(), getDelayOutPosition(), delayPosition, delayFade);
    
    if (feedMode) {
        
    }
    
    loopPosition += buffer.getNumSamples();
    loopPosition %= loopBuffer.getNumSamples();
    
    delayPosition += buffer.getNumSamples();
    delayPosition %= delayBuffer.getNumSamples();
    
    dsp::AudioBlock<float> block(buffer);
    updateFilter(cutoff);
    stateVariableFilter.process(dsp::ProcessContextReplacing<float> (block));
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        buffer.addFrom(channel, 0, noFilter, channel, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool HabitDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HabitDelayAudioProcessor::createEditor()
{
    return new HabitDelayAudioProcessorEditor (*this);
}

//==============================================================================
void HabitDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void HabitDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HabitDelayAudioProcessor();
}
