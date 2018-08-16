#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VibratoFilterAudioProcessor::VibratoFilterAudioProcessor()
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
, vibratoFilter(2)   // two channels
{
    addParameter (baseDelay = new AudioParameterFloat ("baseDelay", "Base Delay", 0.0f, 30.f, 10.f));
    addParameter (width     = new AudioParameterFloat ("width",     "Width",     0.0f, 30.f, 10.f));
}

VibratoFilterAudioProcessor::~VibratoFilterAudioProcessor()
{
}

//==============================================================================
const String VibratoFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VibratoFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VibratoFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VibratoFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VibratoFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VibratoFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VibratoFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VibratoFilterAudioProcessor::setCurrentProgram (int index)
{
}

const String VibratoFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void VibratoFilterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VibratoFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    vibratoFilter.init(sampleRate, static_cast<size_t>(samplesPerBlock));
}

void VibratoFilterAudioProcessor::releaseResources()
{
    // TODO destruct vibratoFilter?
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VibratoFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void VibratoFilterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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

    vibratoFilter.set_delay(baseDelay->get());
    vibratoFilter.process(b);
}

//==============================================================================
bool VibratoFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VibratoFilterAudioProcessor::createEditor()
{
//    return new DelayFiltersAudioProcessorEditor (*this);
    return new GenericAudioProcessorEditor (this);
}

//==============================================================================
void VibratoFilterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    MemoryOutputStream stream (destData, true);

    stream.writeFloat (*baseDelay);
    stream.writeFloat (*width);
}

void VibratoFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);

    baseDelay->setValueNotifyingHost (stream.readFloat());
    width->setValueNotifyingHost (stream.readFloat());
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VibratoFilterAudioProcessor();
}
