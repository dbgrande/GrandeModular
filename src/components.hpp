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

struct RectButtonDim : SvgSwitch {
	RectButtonDim() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectButton1dim.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

struct SquareButtonDim : SvgSwitch {
	SquareButtonDim() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SquareButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SquareButton1dim.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SquareButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

struct RoundTinyRotarySwitch : Trimpot {
	RoundTinyRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}
};

struct RoundSmallRotarySwitch : RoundSmallBlackKnob {
	RoundSmallRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}
};

struct RoundBlackRotarySwitch : RoundBlackKnob {
	RoundBlackRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}
};

struct RoundLargeRotarySwitch : RoundLargeBlackKnob {
	RoundLargeRotarySwitch() {
		//minAngle = -0.83*M_PI;
		//maxAngle = 0.83*M_PI;
		snap = true;
		smooth = false;
	}
};



template <typename TBase>
struct PetiteLight : TSvgLight<TBase> {
	PetiteLight() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/PetiteLight.svg")));
	}
};

template <typename TBase>
struct PetiteLightHalfHalo : TSvgLight<TBase> {
	void drawHalo(const widget::Widget::DrawArgs& args) override {
		// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
		if (args.fb)
			return;

		const float halo = settings::haloBrightness;
		if (halo == 0.f)
			return;

		// If light is off, rendering the halo gives no effect.
		if (this->color.r == 0.f && this->color.g == 0.f && this->color.b == 0.f)
			return;

		math::Vec c = this->box.size.div(2);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		float oradius = radius + std::min(radius * 4.f, 15.f);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, c.x - oradius, c.y - oradius, 2 * oradius, 2 * oradius);

		NVGcolor icol = color::mult(this->color, halo/2.0);  // half brightness
		NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
		NVGpaint paint = nvgRadialGradient(args.vg, c.x, c.y, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}
	PetiteLightHalfHalo() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/PetiteLight.svg")));
	}
};

template <typename TBase>
struct MediumLightFlat : TSvgLight<TBase> {
	MediumLightFlat() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/MediumLightFlat.svg")));
	}
};

template <typename TBase>
struct SmallLightFlat : TSvgLight<TBase> {
	SmallLightFlat() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/SmallLightFlat.svg")));
	}
};

template <typename TBase>
struct TinyStealthLight : TSvgLight<TBase> {
	TinyStealthLight() {
		this->bgColor = nvgRGB(0xbb, 0xbb, 0xb0);
		this->borderColor = nvgRGB(0xbb, 0xbb, 0xb0);
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/TinyLightFlat.svg")));
	}
};

template <typename TBase>
struct PetiteStealthLight : TSvgLight<TBase> {
	void drawBackground(const Widget::DrawArgs& args) override {
			this->bgColor.a = this->color.a;
			this->borderColor.a = 0;
			LightWidget::drawBackground(args);
		};

	PetiteStealthLight() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/PetiteLightFlat.svg")));
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


template <typename TBase>
struct SmallLightTop : TSvgLight<TBase> {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		// Foreground
		if (this->color.a > 0.0) {
			nvgBeginPath(args.vg);
			float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
			nvgArc(args.vg, radius, radius, radius, 0, 3.141592653589793, 1);  // top
			nvgClosePath(args.vg);

			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}
	}

	void drawBackground(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 0, 3.141592653589793, 1);  // top
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
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
		// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
		if (args.fb)
			return;

		const float halo = settings::haloBrightness;
		if (halo == 0.f)
			return;

		// If light is off, rendering the halo gives no effect.
		if (this->color.r == 0.f && this->color.g == 0.f && this->color.b == 0.f)
			return;

		math::Vec c = this->box.size.div(2);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		float oradius = radius + std::min(radius * 4.f, 15.f);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, c.x - oradius, c.y - oradius, 2 * oradius, 2 * oradius);

		NVGcolor icol = color::mult(this->color, halo/2.0);  // half brightness
		NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
		NVGpaint paint = nvgRadialGradient(args.vg, c.x, c.y, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}

	SmallLightTop() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/SmallLightTop.svg")));
	}
};


template <typename TBase>
struct SmallLightBot : TSvgLight<TBase> {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		// Foreground
		if (this->color.a > 0.0) {
			nvgBeginPath(args.vg);
			float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
			nvgArc(args.vg, radius, radius, radius, 3.141592653589793, 0, 1);  // bot
			nvgClosePath(args.vg);

			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}
	}

	void drawBackground(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 3.141592653589793, 0, 1);  // bot
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
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
		// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
		if (args.fb)
			return;

		const float halo = settings::haloBrightness;
		if (halo == 0.f)
			return;

		// If light is off, rendering the halo gives no effect.
		if (this->color.r == 0.f && this->color.g == 0.f && this->color.b == 0.f)
			return;

		math::Vec c = this->box.size.div(2);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		float oradius = radius + std::min(radius * 4.f, 15.f);
	
		nvgBeginPath(args.vg);
		nvgRect(args.vg, c.x - oradius, c.y - oradius, 2 * oradius, 2 * oradius);

		NVGcolor icol = color::mult(this->color, halo/2.0);  // half brightness
		NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
		NVGpaint paint = nvgRadialGradient(args.vg, c.x, c.y, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}

	SmallLightBot() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/SmallLightBot.svg")));
	}
};


template <typename TBase>
struct PetiteLightTop : TSvgLight<TBase> {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		// Foreground
		if (this->color.a > 0.0) {
			nvgBeginPath(args.vg);
			float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
			nvgArc(args.vg, radius, radius, radius, 0, 3.141592653589793, 1);  // top
			nvgClosePath(args.vg);

			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}
	}

	void drawBackground(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 0, 3.141592653589793, 1);  // top
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
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
		// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
		if (args.fb)
			return;

		const float halo = settings::haloBrightness;
		if (halo == 0.f)
			return;

		// If light is off, rendering the halo gives no effect.
		if (this->color.r == 0.f && this->color.g == 0.f && this->color.b == 0.f)
			return;

		math::Vec c = this->box.size.div(2);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		float oradius = radius + std::min(radius * 4.f, 15.f);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, c.x - oradius, c.y - oradius, 2 * oradius, 2 * oradius);
	
		NVGcolor icol = color::mult(this->color, halo/2.0);  // half brightness
		NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
		NVGpaint paint = nvgRadialGradient(args.vg, c.x, c.y, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}

	PetiteLightTop() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/PetiteLightTop.svg")));
	}
};


template <typename TBase>
struct PetiteLightBot : TSvgLight<TBase> {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		// Foreground
		if (this->color.a > 0.0) {
			nvgBeginPath(args.vg);
			float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
			nvgArc(args.vg, radius, radius, radius, 3.141592653589793, 0, 1);  // bot
			nvgClosePath(args.vg);

			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}
	}

	void drawBackground(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgArc(args.vg, radius, radius, radius, 3.141592653589793, 0, 1);  // bot
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
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
		// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
		if (args.fb)
			return;

		const float halo = settings::haloBrightness;
		if (halo == 0.f)
			return;

		// If light is off, rendering the halo gives no effect.
		if (this->color.r == 0.f && this->color.g == 0.f && this->color.b == 0.f)
			return;

		math::Vec c = this->box.size.div(2);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		float oradius = radius + std::min(radius * 4.f, 15.f);
	
		nvgBeginPath(args.vg);
		nvgRect(args.vg, c.x - oradius, c.y - oradius, 2 * oradius, 2 * oradius);
	
		NVGcolor icol = color::mult(this->color, halo/2.0);  // half brightness
		NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
		NVGpaint paint = nvgRadialGradient(args.vg, c.x, c.y, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}

	PetiteLightBot() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/PetiteLightBot.svg")));
	}
};


struct TriangleLEDButton : SvgSwitch {
	TriangleLEDButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TriangleLEDButton0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TriangleLEDButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};

template <typename TBase>
struct TriangleLight : TSvgLight<TBase> {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		// Foreground
		if (this->color.a > 0.0) {
			nvgBeginPath(args.vg);
			float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
			nvgMoveTo(args.vg, 2.0 * radius, radius);  // right
			nvgLineTo(args.vg, radius / 2.0, 1.8660254 * radius);  // bottom left
			nvgLineTo(args.vg, radius / 2.0, 0.1339745 * radius);  // top left
			nvgLineTo(args.vg, 2.0 * radius, radius);  // right
			nvgClosePath(args.vg);

			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}
	}

	void drawBackground(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		nvgMoveTo(args.vg, 2.0 * radius, radius);  // right
		nvgLineTo(args.vg, radius / 2.0, 1.8660254 * radius);  // bottom left
		nvgLineTo(args.vg, radius / 2.0, 0.1339745 * radius);  // top left
		nvgLineTo(args.vg, 2.0 * radius, radius);  // right
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Border
		this->borderColor = nvgRGB(0x55, 0x5d, 0x50);
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.50);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
	// draw halo with half brightness
	void drawHalo(const widget::Widget::DrawArgs& args) override {
		// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
		if (args.fb)
			return;

		const float halo = settings::haloBrightness;
		if (halo == 0.f)
			return;

		// If light is off, rendering the halo gives no effect.
		if (this->color.r == 0.f && this->color.g == 0.f && this->color.b == 0.f)
			return;

		math::Vec c = this->box.size.div(2);
		float radius = std::min(this->box.size.x, this->box.size.y) / 2.0;
		float oradius = radius + std::min(radius * 4.f, 15.f);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, c.x - oradius, c.y - oradius, 2 * oradius, 2 * oradius);

		NVGcolor icol = color::mult(this->color, halo/2.0);  // half brightness
		NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
		NVGpaint paint = nvgRadialGradient(args.vg, c.x, c.y, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}

	TriangleLight() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/TriangleLightFlat.svg")));
	}
};
