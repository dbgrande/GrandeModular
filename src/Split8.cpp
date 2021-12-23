#include "plugin.hpp"

struct Split8 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		POLY_IN_B,
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUTPUTS_B, 8),
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	Split8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(POLY_IN_B, "Polyphonic");
		configOutput(OUTPUTS_B, "Channel 1");
		configOutput(OUTPUTS_B + 1, "Channel 2");
		configOutput(OUTPUTS_B + 2, "Channel 3");
		configOutput(OUTPUTS_B + 3, "Channel 4");
		configOutput(OUTPUTS_B + 4, "Channel 5");
		configOutput(OUTPUTS_B + 5, "Channel 6");
		configOutput(OUTPUTS_B + 6, "Channel 7");
		configOutput(OUTPUTS_B + 7, "Channel 8");
	}

	void process(const ProcessArgs& args) override {
		for (int c = 0; c < 8; c++) {
			float v = inputs[POLY_IN_B].getVoltage(c);
			outputs[OUTPUTS_B + c].setVoltage(v);
		}
	}
};


struct Split8Widget : ModuleWidget {
	Split8Widget(Split8* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Split8.svg")));

		{
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 19.50)), module, Split8::POLY_IN_B));
			for(int i = 0; i < 8; i++) {
				addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 35.75 + 11.25 * i)), module, Split8::OUTPUTS_B + i));
			}
		}
		
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};


Model* modelSplit8 = createModel<Split8, Split8Widget>("Split8");
