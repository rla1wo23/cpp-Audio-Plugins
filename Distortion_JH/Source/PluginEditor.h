#pragma once

#include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

class DistortionJhAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         public juce::ToggleButton::Listener {
public:
  DistortionJhAudioProcessorEditor(DistortionJhAudioProcessor &);
  ~DistortionJhAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void buttonClicked(juce::Button *button) override;

private:
  // Reference to the processor object that created this editor
  DistortionJhAudioProcessor &audioProcessor;

  // UI Components
  juce::ImageComponent mImageComponent;
  juce::Slider driveSlider;
  juce::Slider phatSlider;
  juce::Slider crushSlider;
  juce::Slider volumeSlider;
  juce::ToggleButton autoGainButton;
  juce::HyperlinkButton linkLabel;
  juce::Label version;
  CustomLookAndFeel customLookAndFeel;

  // Attachments to connect sliders to processor parameters
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      driveValue;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      phatValue;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      crushValue;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      volumeValue;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionJhAudioProcessorEditor)
};
