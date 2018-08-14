#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayFiltersAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DelayFiltersAudioProcessorEditor (DelayFiltersAudioProcessor&);
    ~DelayFiltersAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayFiltersAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayFiltersAudioProcessorEditor)
};
