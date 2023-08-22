#include "plugin.hpp"


struct Push : Module {
	enum ParamIds {
		ENUMS(BUTTON_PARAMS, 16),
		ENUMS(SELECT_PARAMS, 15),
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		POLY_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(SELECT_LIGHTS, 15),
		NUM_LIGHTS
	};

	Push() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 16; i++)
			configParam(BUTTON_PARAMS + i , 0.0, 1.0, 0.0, "Channel " + std::to_string(i + 1), "");
		for (int c = 0; c < 15; c++)
			configButton(SELECT_PARAMS + c, std::to_string(c + 1));
		configOutput(POLY_OUTPUT, "Poly");
		onReset();
	}


	int param_timer = 0;
	bool input_values[16];
	bool modestate[15] = { false };
	int split = 15;
	bool last_one_hot = true;
	bool one_hot = false;
	int last_select = -1;
	int push_time = -1;
	bool long_press = false;

	void process(const ProcessArgs &args) override {
		if (param_timer == 0) {
			// read parameters at much less than audio rate
			param_timer = 10;  // how often to update params (audio cycles)

			// radio button mode just enabled, clear all but first button
			if (one_hot == true && last_one_hot == false) {
				int i = 0;
				for ( ; i < 16; i++) {  // find first enabled button
					if (input_values[i]) {
						i++;
						break;
					}
				}
				for ( ; i < 16; i++) {  // clear all following buttons
					input_values[i] = false;
					params[BUTTON_PARAMS + i].setValue(0);
				}
			}
			// update last values
			last_one_hot = one_hot;

			// Update the triangular LED select buttons, need one-hot
			// Require long-press of one second to toggle, so don't accidentally change
			if (last_select == -1) {  // no select buttons pressed
				for (int c = 0; c < 15; c++) {  // check if any button pressed
					if (params[SELECT_PARAMS + c].getValue() > 0) {  // select button just pressed
						last_select = c;  // last_select set to button that was selected
						push_time = 0;
						long_press = false;
						break;
					}
				}
			}
			else {  // select button previously pressed
				if (params[SELECT_PARAMS + last_select].getValue() > 0) {  // select button still pressed
					push_time += param_timer;
					if (!long_press && push_time > args.sampleRate) {  // just hit 1 second long press
						modestate[last_select] = !modestate[last_select];  // toggle select state
						if (modestate[last_select] == true) {  // just turned on
							split = last_select;
							for (int i = 0; i < split; i++)  // turn off any previous switchpoints
								modestate[i] = false;
							for (int i = split + 1; i < 15; i++)  // turn off any following switchpoints
								modestate[i] = false;
						}
						else {  // just turned off
							for (int i = 0; i < 15; i++)  // turn off all switchpoints
								modestate[i] = false;
							split = 15;
						}
						long_press = true;
					}
				}
				else {  // select button released
					last_select = -1;
					push_time = -1;
					long_press = false;
				}
			}

			// update select button lights
			for (int i = 0; i < 15; i++)
				lights[SELECT_LIGHTS + i].setBrightness(modestate[i]);

			// read buttons
			if (one_hot == false) {  // normal buttons
				for (int i = 0; i < 16; i++) {
					input_values[i] = params[BUTTON_PARAMS + i].getValue() > 0;
				}
			}
			else {  // one_hot radio buttons
				for (int i = 0 ; i < 16; i++) {
					bool button = params[BUTTON_PARAMS + i].getValue() > 0;
					// if new button toggled, clear old ones
					if (input_values[i] != button) {
						input_values[i] = button;
						for (int j = 0; j < 16; j++) {
							if (j != i) {
								input_values[j] = false;
								params[BUTTON_PARAMS + j].setValue(0);
							}
						}
						break;
					}
				}
			}

			// dim buttons above split
			for (int i = 0; i < split + 1; i++) {
				if (input_values[i])
					params[BUTTON_PARAMS + i].setValue(2);  // bright svg
			}
			for (int i = split + 1; i < 16; i++) {
				if (input_values[i])
					params[BUTTON_PARAMS + i].setValue(1);  //dim svg
			}

			// output up to split channels
			for (int c = 0; c < split + 1; c++) {
				outputs[POLY_OUTPUT].setVoltage(10.f * input_values[c], c);
			}
			outputs[POLY_OUTPUT].setChannels(split + 1);
		}
		else
			param_timer--;  // just decrement timer
	}

	void onReset() override {
		// disable all select buttons, 16 channels selected
		split = 15;
		for (int c = 0; c < 15; c++)
			modestate[c] = false;
		// disable one-hot mode
		one_hot = false;
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
		// one_hot
		json_object_set_new(rootJ, "one_hot", json_boolean(one_hot));

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
			// calculate split value from state
			split = 15;
			for (int i = 0; i < 15; i++) {
				if (modestate[i] == true) {
					split = i;
					break;
				}
			}
		}
		// one_hot
		json_t* one_hotJ = json_object_get(rootJ, "one_hot");
		if (one_hotJ)
			one_hot = json_boolean_value(one_hotJ);
	}
};

struct PushWidget : ModuleWidget {
	PushWidget(Push* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Push.svg"),
			asset::plugin(pluginInstance, "res/Push-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		for (int i = 0; i < 16; i++) {
			addParam(createParam<SquareButtonDim>(mm2px(Vec(6.40 - 2.5, 15.0 - 2.5 + 5.9*i)), module, Push::BUTTON_PARAMS + i));
		}

		for (int i = 0; i < 15; i++) {
			addChild(createLightCentered<TriangleLight<RedLight>>(mm2px(Vec(2.25, 17.95 + 5.9 * i)), module, Push::SELECT_LIGHTS + i));
			addParam(createParam<TriangleLEDButton>(mm2px(Vec(2.25 - 1.65, 17.95 - 1.65 + 5.9 * i)), module, Push::SELECT_PARAMS + i));
		}

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 115.00)), module, Push::POLY_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		Push* module = dynamic_cast<Push*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createBoolPtrMenuItem("Radio Buttons", "", &module->one_hot));
	}
};


Model* modelPush = createModel<Push, PushWidget>("Push");
