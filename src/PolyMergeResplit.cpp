#include "plugin.hpp"


struct PolyMergeResplit : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(M_INPUTS, 4),
		R_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		M_OUTPUT,
		ENUMS(R_OUTPUTS, 4),
		NUM_OUTPUTS
	};
	enum LightIds {
		M_OVER_LIGHT,
		ENUMS(R_TOP_LIGHTS, 4 * 3),
		ENUMS(R_BOT_LIGHTS, 4 * 3),
		NUM_LIGHTS
	};

	PolyMergeResplit() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(M_INPUTS, "1st poly");
		configInput(M_INPUTS + 1, "2nd poly");
		configInput(M_INPUTS + 2, "3rd poly");
		configInput(M_INPUTS + 3, "4th poly");
		configOutput(M_OUTPUT, "Poly merge");
		configInput(R_INPUT, "Poly resplit");
		configOutput(R_OUTPUTS, "1st poly");
		configOutput(R_OUTPUTS + 1, "2nd poly");
		configOutput(R_OUTPUTS + 2, "3rd poly");
		configOutput(R_OUTPUTS + 3, "4th poly");
	}

	void process(const ProcessArgs &args) override {
		int ch_m[4] = { 0 };
		for (int n = 0; n < 4; n++) {
			if (inputs[M_INPUTS + n].isConnected())
				ch_m[n] = inputs[M_INPUTS + n].getChannels();
		}
		int sum_m = 0;
		bool overflow_m = false;

		// Merge //
		for (int n = 0; n < 4; n++) {
			for (int c = 0; c < ch_m[n]; c++) {
				if (sum_m < 16) {
					float vm = inputs[M_INPUTS + n].getVoltage(c);
					outputs[M_OUTPUT].setVoltage(vm, sum_m);
				}
				else {
					overflow_m = true;
					break;
				}
				sum_m++;
			}
		}
		if (sum_m > 16)
			sum_m = 16;

		// outputs
		outputs[M_OUTPUT].channels = sum_m;

		// lights
		lights[M_OVER_LIGHT].setBrightness(overflow_m);

		// Resplit //
		// Output channel split points defined by Merge inputs
		int ch_r = 0;
		if (inputs[R_INPUT].isConnected())
			ch_r = inputs[R_INPUT].getChannels();
		int num_r[4] = { 0 };
		for (int c = 0; c < ch_r; c++) {
			float vr = inputs[R_INPUT].getVoltage(c);
			if ((ch_m[1] + ch_m[2] + ch_m[3] == 0) || (c < ch_m[0])) {
				outputs[R_OUTPUTS].setVoltage(vr, c);
				num_r[0]++;
			}
			else if ((ch_m[2] + ch_m[3] == 0) || (c < ch_m[0] + ch_m[1])) {
				outputs[R_OUTPUTS + 1].setVoltage(vr, c - ch_m[0]);
				num_r[1]++;
			}
			else if ((ch_m[3] == 0) || (c < ch_m[0] + ch_m[1] + ch_m[2])) {
				outputs[R_OUTPUTS + 2].setVoltage(vr, c - ch_m[0] - ch_m[1]);
				num_r[2]++;
			}
			else {
				outputs[R_OUTPUTS + 3].setVoltage(vr, c - ch_m[0] - ch_m[1] - ch_m[2]);
				num_r[3]++;
			}
		}
		// outputs
		for (int n = 0; n < 4; n++)
			outputs[R_OUTPUTS + n].channels = num_r[n];

		// lights
		for (int n = 0; n < 4; n++) {
			if (ch_r == 0 || num_r[n] == ch_m[n]) {  // input unconnected, or match
				lights[R_TOP_LIGHTS + n*3].setBrightness(0);
				lights[R_BOT_LIGHTS + n*3].setBrightness(0);
				lights[R_TOP_LIGHTS + n*3 + 2].setBrightness(num_r[n] > 0);
				lights[R_BOT_LIGHTS + n*3 + 2].setBrightness(num_r[n] > 0);
			}
			else {  // mismatch
				lights[R_TOP_LIGHTS + n*3].setBrightness(num_r[n] > 0 && num_r[n] > ch_m[n] ? 1.f : 0.1f);
				lights[R_BOT_LIGHTS + n*3].setBrightness(num_r[n] > 0 && num_r[n] > ch_m[n] ? 0.1f : 1.f);
				lights[R_TOP_LIGHTS + n*3 + 2].setBrightness(0);
				lights[R_BOT_LIGHTS + n*3 + 2].setBrightness(0);
			}
		}
	}
};


struct PolyMergeResplitWidget : ModuleWidget {
	PolyMergeResplitWidget(PolyMergeResplit* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/PolyMergeResplit.svg"),
			asset::plugin(pluginInstance, "res/PolyMergeResplit-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Merge //
		for (int i = 0; i < 4; i++) {
			addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 17.00 + 10*i)), module, PolyMergeResplit::M_INPUTS + i));
		}
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 59.25)), module, PolyMergeResplit::M_OUTPUT));
		// light
		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.40, 55.00)), module, PolyMergeResplit::M_OVER_LIGHT));

		// Resplit //
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 74.25)), module, PolyMergeResplit::R_INPUT));
		for (int i =  0; i < 4; i++) {
			addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 85.00 + 10*i)), module, PolyMergeResplit::R_OUTPUTS + i));
			// lights
			addChild(createLightCentered<PetiteLightTop<RedGreenBlueLight>>(mm2px(Vec(8.40, 81.00 + 10*i)), module, PolyMergeResplit::R_TOP_LIGHTS + i*3));
			addChild(createLightCentered<PetiteLightBot<RedGreenBlueLight>>(mm2px(Vec(8.40, 81.00 + 10*i)), module, PolyMergeResplit::R_BOT_LIGHTS + i*3));
		}
	}
};


Model* modelPolyMergeResplit = createModel<PolyMergeResplit, PolyMergeResplitWidget>("PolyMergeResplit");
