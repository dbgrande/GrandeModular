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

template <typename TBase>
struct SmallLightTop : TBase {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 0, 3.141592653589793, 1);  // top
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (this->color.a > 0.0) {
			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}

		// Border
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
	SmallLightTop() {
		this->box.size = app::mm2px(math::Vec(2.176, 2.176));
	}
};

template <typename TBase>
struct SmallLightBot : TBase {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 3.141592653589793, 0, 1);  // bot
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (this->color.a > 0.0) {
			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}

		// Border
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
	void drawHalo(const widget::Widget::DrawArgs& args) override {
		// Don't draw halo twice
	}
	SmallLightBot() {
		this->box.size = app::mm2px(math::Vec(2.176, 2.176));
	}
};

template <typename TBase>
struct PetiteLightTop : TBase {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 0, 3.141592653589793, 1);  // top
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (this->color.a > 0.0) {
			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}

		// Border
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
	PetiteLightTop() {
		this->box.size = app::mm2px(math::Vec(1.632, 1.632));
	}
};

template <typename TBase>
struct PetiteLightBot : TBase {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 3.141592653589793, 0, 1);  // bot
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (this->color.a > 0.0) {
			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}

		// Border
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
	void drawHalo(const widget::Widget::DrawArgs& args) override {
		// Don't draw halo twice
	}
	PetiteLightBot() {
		this->box.size = app::mm2px(math::Vec(1.632, 1.632));
	}
};
