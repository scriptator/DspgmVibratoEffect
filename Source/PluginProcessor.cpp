#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <dspgm/oscillator.h>

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
, rootNode("root")
{
    addParameter (modfreq = new AudioParameterFloat ("modfreq", "Modulation Frequency", 0.5f, 10.f, 5.f));
    addParameter (delay_ms = new AudioParameterFloat ("baseDelay", "Base Delay", 0.0f, 10.f, 1.f));
}

VibratoFilterAudioProcessor::~VibratoFilterAudioProcessor() = default;

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
    int num_channels = std::max(getTotalNumInputChannels(), getTotalNumOutputChannels());
    assert(num_channels > 0);

    if (rootNode.is_initialized()) {
        std::cout << "Removing node VibratoFilter to be able to add it again" << std::endl;
        rootNode.remove_node("VibratoFilter");
    }
    rootNode.add_node(std::make_unique<graph::ProcessorNode<VibratoFilter>>("VibratoFilter"));
    rootNode.init(static_cast<size_t>(num_channels),
                           static_cast<float>(sampleRate),
                           static_cast<float>(sampleRate / 2), // control rate in Hz
                           static_cast<size_t>(samplesPerBlock));

    // add ugen which controls delay modification
    std::unique_ptr<SinOsc> osc = std::make_unique<SinOsc>();
    osc->set_amp(1);
    rootNode.attach_control_ugen("VibratoFilter.oscillation", std::move(osc));

    // retrieve parameter objects
    this->delay_param = rootNode.get_param("VibratoFilter.delay_ms");
    this->modfreq_param = rootNode.get_param("VibratoFilter.oscillation_controller.freq");
}

void VibratoFilterAudioProcessor::releaseResources() {}

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

    // set parameters
    delay_param = this->delay_ms->get();
    modfreq_param = this->modfreq->get();

    rootNode.process(b);
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

    stream.writeFloat(*modfreq);
    stream.writeFloat (*delay_ms);
}

void VibratoFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);

    // TODO how does this work?
//    modfreq->setValueNotifyingHost (stream.readFloat());
//    baseDelay->setValueNotifyingHost (stream.readFloat());
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VibratoFilterAudioProcessor();
}
