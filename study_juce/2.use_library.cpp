class SimpleGainProcessor : public AudioProcessor {
private:
  // 게인 객체
  dsp::Gain<float> gain; // dsp 클래스에 이미 정의된 gain을 활용할 것이다.
public:
  SimpleGainProcessor() {}
  ~SimpleGainProcessor() override {}
  // 게인 객체 초기화하기.
  void prepareToPlay(double sampleRate, int samplesPerBlock) override {
    // 샘플 레이트에 맞게 처리 준비
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    gain.reset();
    gain.prepare(spec);
    gain.setGainLinear(1.0f); // 기본 게인 값 1.0 (변화 없음)
  }

  // 리소스 해제. gain 객체를 비워준다고 보면 된다.
  void releaseResources() override { gain.reset(); }

  // processBlock 메소드 작성
  void processBlock(AudioBuffer<float> &buffer,
                    MidiBuffer &midiMessages) override {
    dsp::AudioBlock<float> blocks(buffer); // float dsp클래스의 block객체 생성
    dsp::ProcessContextReplacing<float> context(blocks);
    // context 객체가 block 배열을 덮어쓴다. 즉, 버퍼에 바뀐 context를 저장
    // 게인 값을 통해 오디오 데이터를 처리
    gain.process(context);
    // gain 객체가, context block을 처리한다.
  }

  const String getName() const override { return "Simple Gain Processor"; }
  bool hasEditor() const override { return false; }
  AudioProcessorEditor *createEditor() override { return nullptr; }
  void getStateInformation(MemoryBlock &destData) override {}
  void setStateInformation(const void *data, int sizeInBytes) override {}

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
    return true;
  }
};
