#include "plugin.hpp"


struct NoteMT : Module {
	enum ParamIds {
		PITCH_PARAM,
		OCTAVE_PARAM,
		SIZE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VOCT_INPUT,
		THRESH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		VOCT_OUTPUT,
		OCTAVE_OUTPUT,
		NOTE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		TEST_LIGHT,
		NUM_LIGHTS
	};

	NoteMT() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PITCH_PARAM, 0.f, 33.f, 0.f, "Note", "");
		configParam(OCTAVE_PARAM, -5, 5, 0, "Octave", "");
		configParam(SIZE_PARAM, 1, 34, 12, "Notes per Octave", "");
	}

	void process(const ProcessArgs &args) override {
		int pitch = clamp((int)(params[PITCH_PARAM].getValue()), 0, 33);
		int octave = clamp((int)(params[OCTAVE_PARAM].getValue()), -5, 5);
		int size = clamp((int)(params[SIZE_PARAM].getValue()), 1, 34);

		if (pitch >= size)
			pitch = size - 1;
		float vout = octave + ((float) pitch / size);
		outputs[VOCT_OUTPUT].setVoltage(vout);

		float vin;
		int channels = inputs[VOCT_INPUT].getChannels();
		if (channels == 0)
			vin = vout;
		else
			vin = inputs[VOCT_INPUT].getVoltage();

		float intPart;
		float fracPart = modff(vin, &intPart);
		if (intPart < 0.f || fracPart < 0.f) { // round to -∞
			if (abs(fracPart) < 1e-7)
				fracPart = 0.f;
			else {
				fracPart += 1.f;
				intPart -= 1.f;
			}
		}
		float scaled_note = fracPart * size;
		if (scaled_note > size - 0.5f) {
			scaled_note -= size;
			intPart += 1.f;
		}
		outputs[OCTAVE_OUTPUT].setVoltage(intPart);
		outputs[NOTE_OUTPUT].setVoltage(scaled_note);

		float nr = fmodf(scaled_note, 1.f);
		lights[TEST_LIGHT].setBrightness(nr < 0.001f || nr > 0.999f);
	}
};

struct NoteMTWidget : ModuleWidget {
	NoteMTWidget(NoteMT* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NoteMT.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(12.70, 12.00)), module, NoteMT::TEST_LIGHT));

		addParam(createParamCentered<RoundBlackRotarySwitch>(mm2px(Vec(12.70, 23.00)), module, NoteMT::OCTAVE_PARAM));

		addParam(createParamCentered<RoundLargeRotarySwitch>(mm2px(Vec(12.70, 42.00)), module, NoteMT::PITCH_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.70, 61.00)), module, NoteMT::VOCT_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.70, 77.00)), module, NoteMT::VOCT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.35, 93.00)), module, NoteMT::OCTAVE_OUTPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.05, 93.00)), module, NoteMT::NOTE_OUTPUT));

		addParam(createParamCentered<RoundLargeRotarySwitch>(mm2px(Vec(12.70, 111.00)), module, NoteMT::SIZE_PARAM));
	}
};


Model* modelNoteMT = createModel<NoteMT, NoteMTWidget>("NoteMT");
