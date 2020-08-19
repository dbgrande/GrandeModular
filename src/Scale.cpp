#include "plugin.hpp"


struct Scale : Module {
	enum ParamIds {
		NOTE0_PARAM,
		NOTE1_PARAM,
		NOTE2_PARAM,
		NOTE3_PARAM,
		NOTE4_PARAM,
		NOTE5_PARAM,
		NOTE6_PARAM,
		NOTE7_PARAM,
		NOTE8_PARAM,
		NOTE9_PARAM,
		NOTE10_PARAM,
		NOTE11_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		SCALE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Scale() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(NOTE0_PARAM, 0.0, 1.0, 1.0, "Note0", "");  // Root note
		configParam(NOTE1_PARAM, 0.0, 1.0, 0.0, "Note1", "");
		configParam(NOTE2_PARAM, 0.0, 1.0, 1.0, "Note2", "");
		configParam(NOTE3_PARAM, 0.0, 1.0, 0.0, "Note3", "");
		configParam(NOTE4_PARAM, 0.0, 1.0, 1.0, "Note4", "");
		configParam(NOTE5_PARAM, 0.0, 1.0, 1.0, "Note5", "");
		configParam(NOTE6_PARAM, 0.0, 1.0, 0.0, "Note6", "");
		configParam(NOTE7_PARAM, 0.0, 1.0, 1.0, "Note7", "");
		configParam(NOTE8_PARAM, 0.0, 1.0, 0.0, "Note8", "");
		configParam(NOTE9_PARAM, 0.0, 1.0, 1.0, "Note9", "");
		configParam(NOTE10_PARAM, 0.0, 1.0, 0.0, "Note10", "");
		configParam(NOTE11_PARAM, 0.0, 1.0, 1.0, "Note11", "");
	}

	dsp::PulseGenerator pulseGenerators[16];

	int param_timer = 0;
	float input_scale[12];

	void process(const ProcessArgs &args) override {
		if (param_timer == 0) {
			// read parameters
			param_timer = 50;  // how often to update params (audio cycles)

			// scale is set by buttons (root on bottom)
			input_scale[0] = 10 * std::round(params[NOTE0_PARAM].getValue());
			input_scale[1] = 10 * std::round(params[NOTE1_PARAM].getValue());
			input_scale[2] = 10 * std::round(params[NOTE2_PARAM].getValue());
			input_scale[3] = 10 * std::round(params[NOTE3_PARAM].getValue());
			input_scale[4] = 10 * std::round(params[NOTE4_PARAM].getValue());
			input_scale[5] = 10 * std::round(params[NOTE5_PARAM].getValue());
			input_scale[6] = 10 * std::round(params[NOTE6_PARAM].getValue());
			input_scale[7] = 10 * std::round(params[NOTE7_PARAM].getValue());
			input_scale[8] = 10 * std::round(params[NOTE8_PARAM].getValue());
			input_scale[9] = 10 * std::round(params[NOTE9_PARAM].getValue());
			input_scale[10] = 10 * std::round(params[NOTE10_PARAM].getValue());
			input_scale[11] = 10 * std::round(params[NOTE11_PARAM].getValue());
		}
		else
			param_timer--;  // just decrement timer

		// Always output all 12 buttons.
		for (int c = 0; c < 12; c++)
			outputs[SCALE_OUTPUT].setVoltage(input_scale[c], c);
		outputs[SCALE_OUTPUT].setChannels(12);
	}
};

struct ScaleWidget : ModuleWidget {
	ScaleWidget(Scale* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Scale.svg")));

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 19.20)), module, Scale::SCALE_OUTPUT));

		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 25.0)), module, Scale::NOTE11_PARAM));
		addParam(createParam<BlackButton>(mm2px(Vec(1.58, 33.0)), module, Scale::NOTE10_PARAM));
		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 41.0)), module, Scale::NOTE9_PARAM));
		addParam(createParam<BlackButton>(mm2px(Vec(1.58, 49.0)), module, Scale::NOTE8_PARAM));
		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 57.0)), module, Scale::NOTE7_PARAM));
		addParam(createParam<BlackButton>(mm2px(Vec(1.58, 65.0)), module, Scale::NOTE6_PARAM));
		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 73.0)), module, Scale::NOTE5_PARAM));
		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 81.0)), module, Scale::NOTE4_PARAM));
		addParam(createParam<BlackButton>(mm2px(Vec(1.58, 89.0)), module, Scale::NOTE3_PARAM));
		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 97.0)), module, Scale::NOTE2_PARAM));
		addParam(createParam<BlackButton>(mm2px(Vec(1.58, 105.0)), module, Scale::NOTE1_PARAM));
		addParam(createParam<WhiteButton>(mm2px(Vec(1.58, 113.0)), module, Scale::NOTE0_PARAM));
	}
};


Model* modelScale = createModel<Scale, ScaleWidget>("Scale");
