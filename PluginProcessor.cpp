#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <sjf/widgets/sjf_GenericEditor.h>


//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
 : AudioProcessor ([]() {
     auto ret = BusesProperties{};
     for ( auto& bus : sjf::plugin_processor_config::getBusProperties()) {
        ret.addBus(bus.isInput, bus.busName, bus.defaultLayout, bus.isActivatedByDefault);
     }
     return ret;
 }())
, params(*this, &undoManager, "Params", sjf::plugin_processor_config::Config::createParameterLayout(processor, groupMetaData))
{
    sjf::helpers::PresetManager::initAPVTS(params.state, params.processor.getParameterTree(), groupMetaData.get());

    sjf::optional_calls::attachToState(processor, params.state);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return "Default";
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    const auto numChannels = juce::jmax(getTotalNumInputChannels(), getTotalNumOutputChannels());
    processSpec = juce::dsp::ProcessSpec{sampleRate, static_cast<uint32>(samplesPerBlock), static_cast<juce::uint32>(numChannels)};
    callPrepare();
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return sjf::plugin_processor_config::isBusesLayoutSupported (layouts);
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
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


    if ( auto playHead = getPlayHead())
    {
        auto positionInfo = playHead->getPosition();
        if (positionInfo.hasValue())
        {
            processor.setPositionInfo(*positionInfo);
        }
    }

    setLatencySamples(processor.getLatencySamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    processor.process(context);

}

void AudioPluginAudioProcessor::processBlockBypassed(AudioBuffer<float> &/*buffer*/,
                                                     MidiBuffer &/*midiMessages*/)
{
    jassertfalse;
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new sjf::generic_editor::GenericEditor (params, *this, *groupMetaData, &undoManager);
    // return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    destData = sjf::helpers::PresetManager::toMemoryBlock(params.copyState());
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (const juce::ValueTree loadedTree = sjf::helpers::PresetManager::toValueTree(data, sizeInBytes); loadedTree.isValid())
    {
        sjf::helpers::PresetManager::initAPVTS(loadedTree, params.processor.getParameterTree(), groupMetaData.get());
        params.replaceState(loadedTree);
        sjf::optional_calls::attachToState(processor, params.state);
    }
}

void AudioPluginAudioProcessor::numBusesChanged()
{
    callPrepare();
}

void AudioPluginAudioProcessor::numChannelsChanged()
{
    callPrepare();
}

void AudioPluginAudioProcessor::processorLayoutsChanged()
{
    callPrepare();
}


void AudioPluginAudioProcessor::callPrepare()
{
    const auto numChannels = juce::jmax(getTotalNumInputChannels(), getTotalNumOutputChannels());
    processSpec.numChannels = static_cast<juce::uint32>(numChannels);
    processor.prepare(processSpec);
}

AudioProcessorParameter * AudioPluginAudioProcessor::getBypassParameter() const
{
    auto topLevelParams = [&]() {
        auto& tree = getParameterTree();
        auto subGroups = tree.getSubgroups(false);
        jassert(subGroups.size() == 1);
        auto grp = subGroups[0];
        while (grp && grp->getParameters(false).size() == 0) {
            auto sub = grp->getSubgroups(false);
            jassert(sub.size() == 1);
            grp = sub[0];
        }

        return grp;
    }();

    jassert(topLevelParams);

    const auto params_ =  topLevelParams->getParameters(false);
    jassert(params_.size() >= 1);
    for (auto param : params_)
    {
        if (auto ranged = dynamic_cast<RangedAudioParameter*>(param); ranged->getParameterID().contains("Bypass"))
            return param;
    }
    jassertfalse;
    return nullptr;

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
