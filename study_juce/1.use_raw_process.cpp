// 나이브한 방식으로 Gain을 구현해 본 클래스이다.
// JUCE는, AudioProcessor클래스를 상속받아 구현하는 방식으로
// 오디오 플러그인을 만들 수 있다.
class SimpleGainProcessor : public AudioProcessor {
public:
  SimpleGainProcessor() {}
  ~SimpleGainProcessor() override {}

  // 오디오 데이터에 50% 게인을 적용하는 메서드이다.
  void processBlock(AudioBuffer<float> &buffer,
                    MidiBuffer &midiMessages) override {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
      // 각 채널의 데이터를 가져와서 처리(2차원 배열)
      float *channelData = buffer.getWritePointer(channel);
      for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        // 50%의 게인 적용
        channelData[sample] *= 0.5f; // 각 채널의 각 배열에 *0.5f씩 곱해준다.
      }
    }
  }

  // 플러그인 이름 반환
  const String getName() const override { return "Simple Gain Processor"; }

  // prepareToPlay 및 releaseResources는 특별한 작업이 필요하지 않으므로 비운다.
  void prepareToPlay(double sampleRate, int samplesPerBlock) override {}
  void releaseResources() override {}

  // 이 플러그인에는 편집기(UI)가 없으므로 기본값 반환
  bool hasEditor() const override { return false; }
  AudioProcessorEditor *createEditor() override { return nullptr; }

  // 상태 저장 및 복원 메서드 (현재는 상태 정보가 없으므로 빈 메서드)
  void getStateInformation(MemoryBlock &destData) override {}
  void setStateInformation(const void *data, int sizeInBytes) override {}

  // 입력 및 출력 버스 레이아웃이 지원되는지 확인하는 메서드 (기본 레이아웃
  // 지원)
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
    return true;
  }
};
