#include "plugin.hpp"

struct PolySplit : Module {
	enum ParamIds {
		ENUMS(SELECT_PARAMS, 15),
		NUM_PARAMS
	};
	enum InputIds {
		POLY_IN,
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(POLY_OUTS, 4),
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(SELECT_LIGHTS, 15),
		ENUMS(CHANNEL_LIGHTS, 16),
		NUM_LIGHTS
	};
	
	PolySplit() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int c = 0; c < 15; c++)
			configButton(SELECT_PARAMS + c, std::to_string(c + 1));
		configInput(POLY_IN, "Poly split");
		configOutput(POLY_OUTS, "1st poly");
		configOutput(POLY_OUTS + 1, "2nd poly");
		configOutput(POLY_OUTS + 2, "3rd poly");
		configOutput(POLY_OUTS + 3, "4th poly");
		onReset();
	}

	int last_mode[15] = { -1 };
	int mode[15] = { -1 };
	bool modestate[15];
	int splits[4] = { -1 };

	void process(const ProcessArgs& args) override {
		// Update the triangular LED buttons, and dim buttons over fourth
		int num_on = 0;
		for (int c = 0; c < 15; c++) {
			mode[c] = clamp((int)(params[SELECT_PARAMS + c].getValue()), 0, 1);
			if (mode[c] == 1) {
				if (last_mode[c] == 0) {
					modestate[c] = !modestate[c];
					last_mode[c] = 1;
				}
			}
			else
				last_mode[c] = 0;

			if (modestate[c]) {  // count number of buttons on
				if (num_on < 4)
					splits[num_on] = c + 1;  // update splits table
				num_on++;
			}
			lights[SELECT_LIGHTS + c].setBrightness(num_on < 5 ? modestate[c] : 0.20 * modestate[c]);
		}
		// Max out unused splits
		for (int i = num_on; i < 4; i++)
			splits[i] = 16;

		// Display the channel lights (dim channels not output)
		int channels = inputs[POLY_IN].getChannels();
		for (int c = 0; c < channels; c++) {
			lights[CHANNEL_LIGHTS + c].setBrightness(c < splits[3] ? 1.f : 0.20f);
		}
		for (int c = channels; c < 16; c++)
			lights[CHANNEL_LIGHTS + c].setBrightness(false);

		// Split out channels to each output
		int c = 0;  // input channels
		for (int n = 0; n < 4; n++) {  // n defines output (0-3)
			if (c < channels) {
				int i = 0;  // output channels
				for ( ; c < splits[n] && c < channels; i++, c++)
					outputs[POLY_OUTS + n].setVoltage(inputs[POLY_IN].getVoltage(c), i);
				outputs[POLY_OUTS + n].channels = i;
			} else
				outputs[POLY_OUTS + n].channels = 0;
		}
	}
	
	void onReset() override {
		for (int c = 0; c < 15; c++)
			modestate[c] = false;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();

		// modestates
		json_t* modestatesJ = json_array();
		for (int i = 0; i < 15; i++) {
			json_t* modestateJ = json_boolean(modestate[i]);
			json_array_append_new(modestatesJ, modestateJ);
		}
		json_object_set_new(rootJ, "modestates", modestatesJ);

		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// modestates
		json_t* modestatesJ = json_object_get(rootJ, "modestates");
		if (modestatesJ) {
			for (int i = 0; i < 15; i++) {
				json_t* modestateJ = json_array_get(modestatesJ, i);
				if (modestateJ)
					modestate[i] = json_boolean_value(modestateJ);
			}
		}
	}
};


struct PolySplitWidget : ModuleWidget {
	PolySplitWidget(PolySplit* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PolySplit.svg")));

		for (int i = 0; i < 15; i++) {
			addChild(createLightCentered<TriangleLight<RedLight>>(mm2px(Vec(2.25, 14.45968 + 3.32718 * i)), module, PolySplit::SELECT_LIGHTS + i));
			addParam(createParam<TriangleLEDButton>(mm2px(Vec(2.25 - 1.65, 14.45968 - 1.65 + 3.32718 * i)), module, PolySplit::SELECT_PARAMS + i));
		}
		for (int i = 0; i < 16; i++) {
			addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(8.25, 12.79609 + 3.32718 * i)), module, PolySplit::CHANNEL_LIGHTS + i));
		}

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 74.25)), module, PolySplit::POLY_IN));
		for (int i = 0; i < 4; i++) {
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 85.00 + 10.00 * i)), module, PolySplit::POLY_OUTS + i));
		}

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};


Model* modelPolySplit = createModel<PolySplit, PolySplitWidget>("PolySplit");
