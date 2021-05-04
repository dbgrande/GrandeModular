using namespace rack;

extern Plugin *pluginInstance;

struct CKSSNoRandom : CKSS {
	void randomize () override {
	}
};

struct CKSSThreeNoRandom : CKSSThree {
	void randomize () override {
	}
};

struct WhiteButton : SvgSwitch {
	WhiteButton() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WhiteButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WhiteButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

struct BlackButton : SvgSwitch {
	BlackButton() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlackButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlackButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

struct RectButton : SvgSwitch {
	RectButton() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

struct TL1105Red : SvgSwitch {
	TL1105Red() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TL1105_Gray.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TL1105_Red.svg")));
		//fb->removeChild(shadow);
		//delete shadow;
	}
	// currently only using this for QuantMT Ref button
	// if use elsewhere, may need to split out NoRandom version
	void randomize () override {
	}
};

struct RoundTinyRotarySwitch : Trimpot {
	RoundTinyRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}

	// handle the manually entered values
	void onChange(const event::Change &e) override {
		Trimpot::onChange(e);
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}

	// override the base randomizer as it sets switches to invalid values.
	void randomize() override {
		Trimpot::randomize();
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}
};

struct RoundTinyRotarySwitchNoRandom : RoundTinyRotarySwitch {
	void randomize() override {
	}
};

struct RoundSmallRotarySwitch : RoundSmallBlackKnob {
	RoundSmallRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}

	// handle the manually entered values
	void onChange(const event::Change &e) override {
		RoundSmallBlackKnob::onChange(e);
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}

	// override the base randomizer as it sets switches to invalid values.
	void randomize() override {
		RoundSmallBlackKnob::randomize();
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}
};

struct RoundBlackRotarySwitch : RoundBlackKnob {
	RoundBlackRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}

	// handle the manually entered values
	void onChange(const event::Change &e) override {
		RoundBlackKnob::onChange(e);
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}

	// override the base randomizer as it sets switches to invalid values.
	void randomize() override {
		RoundBlackKnob::randomize();
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}
};

struct RoundBlackKnobNoRandom : RoundBlackKnob {
	void randomize () override {
	}
};

struct RoundLargeRotarySwitch : RoundLargeBlackKnob {
	RoundLargeRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}

	// handle the manually entered values
	void onChange(const event::Change &e) override {
		RoundLargeBlackKnob::onChange(e);
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}

	// override the base randomizer as it sets switches to invalid values.
	void randomize() override {
		RoundLargeBlackKnob::randomize();
		paramQuantity->setValue(roundf(paramQuantity->getValue()));
	}
};

template <typename TBase>
struct TinyStealthLight : TBase {
	TinyStealthLight() {
		this->box.size = app::mm2px(math::Vec(1.088, 1.088));
		this->bgColor = nvgRGB(0xbb, 0xbb, 0xb0);
		this->borderColor = nvgRGB(0xbb, 0xbb, 0xb0);
	}
};

template <typename TBase>
struct PetiteStealthLight : TBase {
	PetiteStealthLight() {
		this->box.size = app::mm2px(math::Vec(1.632, 1.632));
		this->bgColor = nvgRGB(0xbb, 0xbb, 0xb0);
		this->borderColor = nvgRGB(0xbb, 0xbb, 0xb0);
	}
};

template <typename TBase>
struct PetiteLight : TBase {
	PetiteLight() {
		this->box.size = app::mm2px(math::Vec(1.632, 1.632));
	}
};

struct SmallLEDButton : SvgSwitch {
	SmallLEDButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SmallLEDButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SmallLEDButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

struct SmallPort : app::SvgPort {
	SmallPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SmallPort.svg")));
	}
};
