#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Constructor and Destructor
DistortionJhAudioProcessor::DistortionJhAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      treeState(*this, nullptr, "PARAMETERS",
                {// Parameters: MIN, MAX, DEFAULT
                 std::make_unique<juce::AudioParameterInt>(DRIVE_ID, DRIVE_NAME,
                                                           0, 100, 0),
                 std::make_unique<juce::AudioParameterInt>(PHAT_ID, PHAT_NAME,
                                                           0, 100, 0),
                 std::make_unique<juce::AudioParameterInt>(CRUSH_ID, CRUSH_NAME,
                                                           0, 100, 0),
                 std::make_unique<juce::AudioParameterFloat>(
                     VOLUME_ID, VOLUME_NAME,
                     NormalisableRange<float>(-36.0f, 10.0f, 0.1f), 0.0f)})
#endif
{
  treeState.state = juce::ValueTree("savedParams");
}

DistortionJhAudioProcessor::~DistortionJhAudioProcessor() {}

//==============================================================================
// Basic Plugin Information
const juce::String DistortionJhAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool DistortionJhAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool DistortionJhAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool DistortionJhAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double DistortionJhAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int DistortionJhAudioProcessor::getNumPrograms() { return 1; }

int DistortionJhAudioProcessor::getCurrentProgram() { return 0; }

void DistortionJhAudioProcessor::setCurrentProgram(int index) {}

const juce::String DistortionJhAudioProcessor::getProgramName(int index) {
  return {};
}

void DistortionJhAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

//==============================================================================
// Audio Processor Lifecycle
void DistortionJhAudioProcessor::prepareToPlay(double sampleRate,
                                               int samplesPerBlock) {
  // Initialization before playback starts
}

void DistortionJhAudioProcessor::releaseResources() {
  // Cleanup after playback stops
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionJhAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) {
    return false;
  }

#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) {
    return false;
  }
#endif

  return true;
#endif
}
#endif

//==============================================================================
// Main Audio Processing Block
void DistortionJhAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                              juce::MidiBuffer &midiMessages) {
  ScopedNoDenormals noDenormals;

  // Clear unused output channels
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
    buffer.clear(i, 0, buffer.getNumSamples());
  }

  // Retrieve parameter values
  float VOLUME = *treeState.getRawParameterValue(VOLUME_ID);
  float DRIVE = *treeState.getRawParameterValue(DRIVE_ID) / 10.0f;
  float PHAT = *treeState.getRawParameterValue(PHAT_ID);
  float CRUSH = *treeState.getRawParameterValue(CRUSH_ID) / 100.0f;

  // Apply effects to each channel
  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    auto *channelData = buffer.getWritePointer(channel);

    if (DRIVE > 0.0f) {
      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        channelData[i] = std::tanh(channelData[i] * (2.0f + DRIVE));
      }
    }

    if (PHAT > 0.0f) {
      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        channelData[i] = (1.0f + PHAT) * channelData[i] /
                         (1.0f + PHAT * std::abs(channelData[i]));
      }
    }

    if (CRUSH > 0.0f) {
      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        float dry = channelData[i];
        float wet = std::round(channelData[i] * std::pow(2.0f, CRUSH)) /
                    std::pow(2.0f, CRUSH);
        channelData[i] = ((wet + dry) * std::asin(CRUSH) + dry);
      }
    }
  }

  // Auto Gain Control
  if (autoGain) {
    float totalGainReduction =
        ((DRIVE - 2.0f) + (PHAT / 10.0f) + (CRUSH * 15.0f));
    buffer.applyGain(juce::Decibels::decibelsToGain(-totalGainReduction));
  }

  // Apply final output gain
  float outputGain = juce::Decibels::decibelsToGain(VOLUME, -36.0f);
  buffer.applyGain(outputGain);

  // Debug logging
#if defined(_DEBUG)
  juce::Logger::writeToLog(juce::String::formatted("VOLUME: %f", VOLUME));
  juce::Logger::writeToLog(juce::String::formatted("DRIVE: %f", DRIVE));
  juce::Logger::writeToLog(juce::String::formatted("PHAT: %f", PHAT));
  juce::Logger::writeToLog(juce::String::formatted("CRUSH: %f", CRUSH));
  juce::Logger::writeToLog(
      juce::String::formatted("Applying Gain: %f", outputGain));
#endif
}

//==============================================================================
// Plugin Editor
bool DistortionJhAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor *DistortionJhAudioProcessor::createEditor() {
  return new DistortionJhAudioProcessorEditor(*this);
}

//==============================================================================
// State Management
void DistortionJhAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = treeState.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void DistortionJhAudioProcessor::setStateInformation(const void *data,
                                                     int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState && xmlState->hasTagName(treeState.state.getType())) {
    treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
  }
}

//==============================================================================
// Factory Function
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new DistortionJhAudioProcessor();
}
