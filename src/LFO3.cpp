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


struct LFO3 : Module {
	enum ParamIds {
		FREQA_PARAM,
		FREQB_PARAM,
		FREQC_PARAM,
		FMA_PARAM,
		FMB_PARAM,
		FMC_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FMA_INPUT,
		FMB_INPUT,
		FMC_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINA_OUTPUT,
		SINB_OUTPUT,
		SINC_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	LowFrequencyOscillator<float_4> oscillatorsA[4];
	LowFrequencyOscillator<float_4> oscillatorsB[4];
	LowFrequencyOscillator<float_4> oscillatorsC[4];

	LFO3() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(FREQA_PARAM, -8.f, 10.f, 1.f, "Frequency A", " Hz", 2, 1);
		configParam(FMA_PARAM, -1.f, 1.f, 0.f, "Frequency modulation A", "%", 0.f, 100.f);
		configInput(FMA_INPUT, "Frequency modulation A");
		configOutput(SINA_OUTPUT, "Sine A");
		configParam(FREQB_PARAM, -8.f, 10.f, 1.f, "Frequency B", " Hz", 2, 1);
		configParam(FMB_PARAM, -1.f, 1.f, 0.f, "Frequency modulation B", "%", 0.f, 100.f);
		configInput(FMB_INPUT, "Frequency modulation B");
		configOutput(SINB_OUTPUT, "Sine B");
		configParam(FREQC_PARAM, -8.f, 10.f, 1.f, "Frequency C", " Hz", 2, 1);
		configParam(FMC_PARAM, -1.f, 1.f, 0.f, "Frequency modulation C", "%", 0.f, 100.f);
		configInput(FMC_INPUT, "Frequency modulation C");
		configOutput(SINC_OUTPUT, "Sine C");
	}

	void process(const ProcessArgs& args) override {
		float freqAParam = params[FREQA_PARAM].getValue();
		float freqBParam = params[FREQB_PARAM].getValue();
		float freqCParam = params[FREQC_PARAM].getValue();
		float fmAParam = params[FMA_PARAM].getValue();
		float fmBParam = params[FMB_PARAM].getValue();
		float fmCParam = params[FMC_PARAM].getValue();

		int channelsA = std::max(1, inputs[FMA_INPUT].getChannels());
		int channelsB = std::max(1, inputs[FMB_INPUT].getChannels());
		int channelsC = std::max(1, inputs[FMC_INPUT].getChannels());

		for (int c = 0; c < channelsA; c += 4) {
			auto* oscillatorA = &oscillatorsA[c / 4];

			float_4 pitch = freqAParam;
			pitch += inputs[FMA_INPUT].getVoltageSimd<float_4>(c) * fmAParam;
			oscillatorA->setPitch(pitch);

			oscillatorA->step(args.sampleTime);

			// Outputs
			if (outputs[SINA_OUTPUT].isConnected())
				outputs[SINA_OUTPUT].setVoltageSimd(5.f * oscillatorA->sin(), c);
		}

		for (int c = 0; c < channelsB; c += 4) {
			auto* oscillatorB = &oscillatorsB[c / 4];

			float_4 pitch = freqBParam;
			pitch += inputs[FMB_INPUT].getVoltageSimd<float_4>(c) * fmBParam;
			oscillatorB->setPitch(pitch);

			oscillatorB->step(args.sampleTime);

			// Outputs
			if (outputs[SINB_OUTPUT].isConnected())
				outputs[SINB_OUTPUT].setVoltageSimd(5.f * oscillatorB->sin(), c);
		}

		for (int c = 0; c < channelsC; c += 4) {
			auto* oscillatorC = &oscillatorsC[c / 4];

			float_4 pitch = freqCParam;
			pitch += inputs[FMC_INPUT].getVoltageSimd<float_4>(c) * fmCParam;
			oscillatorC->setPitch(pitch);

			oscillatorC->step(args.sampleTime);

			// Outputs
			if (outputs[SINC_OUTPUT].isConnected())
				outputs[SINC_OUTPUT].setVoltageSimd(5.f * oscillatorC->sin(), c);
		}

		outputs[SINA_OUTPUT].setChannels(channelsA);
		outputs[SINB_OUTPUT].setChannels(channelsB);
		outputs[SINC_OUTPUT].setChannels(channelsC);

	}
};



struct LFO3Widget : ModuleWidget {
	LFO3Widget(LFO3* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFO3.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 18.00)), module, LFO3::FREQA_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 26.25)), module, LFO3::SINA_OUTPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 35.00)), module, LFO3::FMA_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 42.50)), module, LFO3::FMA_PARAM));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 55.00)), module, LFO3::FREQB_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 63.25)), module, LFO3::SINB_OUTPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 72.00)), module, LFO3::FMB_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 79.50)), module, LFO3::FMB_PARAM));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 92.00)), module, LFO3::FREQC_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 100.25)), module, LFO3::SINC_OUTPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 109.00)), module, LFO3::FMC_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 116.50)), module, LFO3::FMC_PARAM));
	}
};


Model* modelLFO3 = createModel<LFO3, LFO3Widget>("LFO3");

