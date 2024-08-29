#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
DistortionJhAudioProcessorEditor::DistortionJhAudioProcessorEditor(
    DistortionJhAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  // Initialize Auto Gain Toggle Button
  autoGainButton.setToggleState(true, juce::dontSendNotification);
  autoGainButton.setClickingTogglesState(true);
  autoGainButton.addListener(this);
  addAndMakeVisible(autoGainButton);

  // Set the size of the editor window
  setSize(360, 340);
}

DistortionJhAudioProcessorEditor::~DistortionJhAudioProcessorEditor() {}

//==============================================================================
void DistortionJhAudioProcessorEditor::buttonClicked(juce::Button *button) {
  if (button == &autoGainButton) {
    audioProcessor.autoGain = autoGainButton.getToggleState();
  }
}

//==============================================================================
void DistortionJhAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

//==============================================================================
void DistortionJhAudioProcessorEditor::resized() {
  // Set bounds for the Auto Gain Button
  autoGainButton.setBounds(150, 150, 60, 30);
}
