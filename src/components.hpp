using namespace rack;

extern Plugin *pluginInstance;

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
