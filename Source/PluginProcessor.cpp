#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayFiltersAudioProcessor::DelayFiltersAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
, delayFilter(2)   // two channels
{
    delayFilter.set_delay(44100);
}

DelayFiltersAudioProcessor::~DelayFiltersAudioProcessor()
{
}

//==============================================================================
const String DelayFiltersAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayFiltersAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayFiltersAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayFiltersAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayFiltersAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayFiltersAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayFiltersAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayFiltersAudioProcessor::setCurrentProgram (int index)
{
}

const String DelayFiltersAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayFiltersAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DelayFiltersAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    delayFilter.init(sampleRate, samplesPerBlock);
}

void DelayFiltersAudioProcessor::releaseResources()
{
    // TODO destruct delayFilter?
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayFiltersAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void DelayFiltersAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto b = dspgm::AudioBuffer(
            buffer.getArrayOfWritePointers(),
            static_cast<size_t>(getTotalNumOutputChannels()),
            static_cast<size_t>(buffer.getNumSamples())
    );

    delayFilter.process(b);
}

//==============================================================================
bool DelayFiltersAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayFiltersAudioProcessor::createEditor()
{
    return new DelayFiltersAudioProcessorEditor (*this);
}

//==============================================================================
void DelayFiltersAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayFiltersAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayFiltersAudioProcessor();
}
