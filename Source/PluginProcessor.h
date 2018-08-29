#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <dspgm/filter.h>
#include <dspgm/graph.h>

//==============================================================================

using namespace dspgm;

/**
*/
class VibratoFilterAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    VibratoFilterAudioProcessor();
    ~VibratoFilterAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoFilterAudioProcessor)

    graph::SequenceNode rootNode;

    AudioParameterFloat* modfreq;
    AudioParameterFloat* delay_ms;

    std::function<void(float)> delay_setter;
    std::function<void(float)> modfreq_setter;
};

//class ParameterChangeListener : juce::Listener {
//
//};
