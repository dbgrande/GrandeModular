#include "plugin.hpp"


using simd::float_4;


template <typename T>
struct LowFrequencyOscillator {
	T phase = 0.f;
	T pw = 0.5f;
	T freq = 1.f;
	bool invert = false;
	bool bipolar = true;
	T resetState = T::mask();

	void setPitch(T pitch) {
		pitch = simd::fmin(pitch, 10.f);
		freq = dsp::approxExp2_taylor5(pitch + 30) / 1073741824;
	}
	void setPulseWidth(T pw) {
		const T pwMin = 0.01f;
		this->pw = clamp(pw, pwMin, 1.f - pwMin);
	}
	void setReset(T reset) {
		reset = simd::rescale(reset, 0.1f, 2.f, 0.f, 1.f);
		T on = (reset >= 1.f);
		T off = (reset <= 0.f);
		T triggered = ~resetState & on;
		resetState = simd::ifelse(off, 0.f, resetState);
		resetState = simd::ifelse(on, T::mask(), resetState);
		phase = simd::ifelse(triggered, 0.f, phase);
	}
	void step(float dt) {
		T deltaPhase = simd::fmin(freq * dt, 0.5f);
		phase += deltaPhase;
		phase -= (phase >= 1.f) & 1.f;
	}
	T sin() {
		T p = phase;
		if (!bipolar)
			p -= 0.25f;
		T v = simd::sin(2 * M_PI * p);
		if (invert)
			v *= -1.f;
		if (!bipolar)
			v += 1.f;
		return v;
	}
};


struct LFO4 : Module {
	enum ParamIds {
		FM_PARAM,
		FREQA_PARAM,
		FREQB_PARAM,
		FREQC_PARAM,
		FREQD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FM_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINA_OUTPUT,
		SINB_OUTPUT,
		SINC_OUTPUT,
		SIND_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		A_LIGHT,
		B_LIGHT,
		C_LIGHT,
		D_LIGHT,
		NUM_LIGHTS
	};

	LowFrequencyOscillator<float_4> oscillatorsA[4];
	LowFrequencyOscillator<float_4> oscillatorsB[4];
	LowFrequencyOscillator<float_4> oscillatorsC[4];
	LowFrequencyOscillator<float_4> oscillatorsD[4];

	LFO4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(FM_PARAM, -6.f, 8.f, 1.f, "Frequency", "Hz", 2, 1);
		configInput(FM_INPUT, "Frequency CV");
		configParam(FREQA_PARAM, -8.f, 8.f, 0.f, "A scaling ratio", "×", 2, 1);
		configOutput(SINA_OUTPUT, "Sine A");
		configParam(FREQB_PARAM, -8.f, 8.f, 0.f, "B scaling ratio", "×", 2, 1);
		configOutput(SINB_OUTPUT, "Sine B");
		configParam(FREQC_PARAM, -8.f, 8.f, 0.f, "C scaling ratio", "×", 2, 1);
		configOutput(SINC_OUTPUT, "Sine C");
		configParam(FREQD_PARAM, -8.f, 8.f, 0.f, "D scaling ratio", "×", 2, 1);
		configOutput(SIND_OUTPUT, "Sine D");
	}

	void process(const ProcessArgs& args) override {
		float fmParam = params[FM_PARAM].getValue();
		float freqAParam = params[FREQA_PARAM].getValue();
		float freqBParam = params[FREQB_PARAM].getValue();
		float freqCParam = params[FREQC_PARAM].getValue();
		float freqDParam = params[FREQD_PARAM].getValue();

		int channelsA = std::max(1, inputs[FM_INPUT].getChannels());
		int channelsB = channelsA;
		int channelsC = channelsA;
		int channelsD = channelsA;

		for (int c = 0; c < channelsA; c += 4) {
			auto* oscillatorA = &oscillatorsA[c / 4];

			float_4 pitch = inputs[FM_INPUT].getVoltageSimd<float_4>(c) + fmParam;
			pitch += freqAParam;
			oscillatorA->setPitch(pitch);

			oscillatorA->step(args.sampleTime);

			// Outputs
			if (outputs[SINA_OUTPUT].isConnected())
				outputs[SINA_OUTPUT].setVoltageSimd(5.f * oscillatorA->sin(), c);
		}

		for (int c = 0; c < channelsB; c += 4) {
			auto* oscillatorB = &oscillatorsB[c / 4];

			float_4 pitch = inputs[FM_INPUT].getVoltageSimd<float_4>(c) + fmParam;
			pitch += freqBParam;
			oscillatorB->setPitch(pitch);

			oscillatorB->step(args.sampleTime);

			// Outputs
			if (outputs[SINB_OUTPUT].isConnected())
				outputs[SINB_OUTPUT].setVoltageSimd(5.f * oscillatorB->sin(), c);
		}

		for (int c = 0; c < channelsC; c += 4) {
			auto* oscillatorC = &oscillatorsC[c / 4];

			float_4 pitch = inputs[FM_INPUT].getVoltageSimd<float_4>(c) + fmParam;
			pitch += freqCParam;
			oscillatorC->setPitch(pitch);

			oscillatorC->step(args.sampleTime);

			// Outputs
			if (outputs[SINC_OUTPUT].isConnected())
				outputs[SINC_OUTPUT].setVoltageSimd(5.f * oscillatorC->sin(), c);
		}

		for (int c = 0; c < channelsD; c += 4) {
			auto* oscillatorD = &oscillatorsD[c / 4];

			float_4 pitch = inputs[FM_INPUT].getVoltageSimd<float_4>(c) + fmParam;
			pitch += freqDParam;
			oscillatorD->setPitch(pitch);

			oscillatorD->step(args.sampleTime);

			// Outputs
			if (outputs[SIND_OUTPUT].isConnected())
				outputs[SIND_OUTPUT].setVoltageSimd(5.f * oscillatorD->sin(), c);
		}

		outputs[SINA_OUTPUT].setChannels(channelsA);
		outputs[SINB_OUTPUT].setChannels(channelsB);
		outputs[SINC_OUTPUT].setChannels(channelsC);
		outputs[SIND_OUTPUT].setChannels(channelsD);


		// Warning lights - approximate point when LFOs reach their upper or lower limits
		// Use stealth lights, since they only work when the fm input is not connected.
		if (inputs[FM_INPUT].isConnected() == false) {
			lights[A_LIGHT].setBrightness((fmParam + freqAParam) > 10.f || (fmParam + freqAParam) < -10.);
			lights[B_LIGHT].setBrightness((fmParam + freqBParam) > 10.f || (fmParam + freqBParam) < -10.f);
			lights[C_LIGHT].setBrightness((fmParam + freqCParam) > 10.f || (fmParam + freqCParam) < -10.f);
			lights[D_LIGHT].setBrightness((fmParam + freqDParam) > 10.f || (fmParam + freqDParam) < -10.f);
		}
		else {
			lights[A_LIGHT].setBrightness(false);
			lights[B_LIGHT].setBrightness(false);
			lights[C_LIGHT].setBrightness(false);
			lights[D_LIGHT].setBrightness(false);
		}
	}
};



struct LFO4Widget : ModuleWidget {
	LFO4Widget(LFO4* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFO4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 18.25)), module, LFO4::FM_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 27.00)), module, LFO4::FM_INPUT));

		addChild(createLightCentered<PetiteStealthLight<RedLight>>(mm2px(Vec(8.20, 38.00)), module, LFO4::A_LIGHT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 40.80)), module, LFO4::FREQA_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 49.30)), module, LFO4::SINA_OUTPUT));

		addChild(createLightCentered<PetiteStealthLight<RedLight>>(mm2px(Vec(8.20, 60.00)), module, LFO4::B_LIGHT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 62.80)), module, LFO4::FREQB_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 71.30)), module, LFO4::SINB_OUTPUT));

		addChild(createLightCentered<PetiteStealthLight<RedLight>>(mm2px(Vec(8.20, 82.00)), module, LFO4::C_LIGHT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 84.80)), module, LFO4::FREQC_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 93.30)), module, LFO4::SINC_OUTPUT));

		addChild(createLightCentered<PetiteStealthLight<RedLight>>(mm2px(Vec(8.20, 104.00)), module, LFO4::D_LIGHT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 106.80)), module, LFO4::FREQD_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 115.30)), module, LFO4::SIND_OUTPUT));
	}
};


Model* modelLFO4 = createModel<LFO4, LFO4Widget>("LFO4");

