#pragma once

#include <JuceHeader.h>

// Parameter IDs and Names
#define DRIVE_ID "drive"
#define DRIVE_NAME "Drive"
#define PHAT_ID "phat"
#define PHAT_NAME "Phat"
#define CRUSH_ID "crush"
#define CRUSH_NAME "Crush"
#define VOLUME_ID "volume"
#define VOLUME_NAME "Volume"

// Constants to replace magic numbers
constexpr float DEFAULT_DRIVE = 2.0f;
constexpr float DRIVE_SCALE = 10.0f;
constexpr float CRUSH_SCALE = 100.0f;
constexpr float AUTO_GAIN_SCALE = 15.0f;
constexpr float OUTPUT_GAIN_MIN = -36.0f;
constexpr float OUTPUT_GAIN_MAX = 10.0f;

class DistortionJhAudioProcessor : public juce::AudioProcessor {
public:
  //==============================================================================
  DistortionJhAudioProcessor();
  ~DistortionJhAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
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
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  AudioProcessorValueTreeState treeState;
  bool autoGain;

private:
  //==============================================================================
  dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                           dsp::IIR::Coefficients<float>>
      lowPassFilter;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionJhAudioProcessor)
};
