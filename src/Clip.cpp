#include "plugin.hpp"


struct Clip : Module {
	enum ParamIds {
		VCLIP_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VCLIP_INPUT,
		A_INPUT,
		B_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		A_OUTPUT,
		B_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(A_LIGHTS, 16*3),
		ENUMS(B_LIGHTS, 16*3),
		NUM_LIGHTS
	};

	Clip() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(VCLIP_PARAM, 0.f, 10.f, 5.f, "Vclip", "V");
		configInput(A_INPUT, "A");
		configOutput(A_OUTPUT, "A");
		configInput(B_INPUT, "B");
		configOutput(B_OUTPUT, "B");
		configInput(VCLIP_INPUT, "Vclip");
		configBypass(A_INPUT, A_OUTPUT);
		configBypass(B_INPUT, B_OUTPUT);
		onReset();
	}

	enum State { LowIdle, LowTimer, HighIdle, HighTimer };

	const float max_voltage = 20.f;  // max allowed clipping voltage
	float vclip[16] = { 5.f };
	int check_lights = 0;

	bool aclip[16] = { false };
	State astate[16] = { LowIdle };
	int acount[16] = { 0 };

	bool bclip[16] = { false };
	State bstate[16] = { LowIdle };
	int bcount[16] = { 0 };

	bool logic_mon = false;


	void process(const ProcessArgs &args) override {
		// minimum length of time to keep clip light on (100 ms)
		int high_time = (int)(0.100f * args.sampleRate);
		// minimum length of time to keep clip light off (100 ms)
		int low_time = (int)(0.100f * args.sampleRate);

		float vclip_knob = clamp((params[VCLIP_PARAM].getValue()), 0.f, 10.f);

		int achannels = inputs[A_INPUT].getChannels();
		int bchannels = inputs[B_INPUT].getChannels();
		int max_channels = std::max(achannels, bchannels);

		// vclip voltage
		int vchannels = inputs[VCLIP_INPUT].getChannels();
		if (vchannels > 0) {  // vclip input is connected
			int c = 0;
			for ( ; c < vchannels; c++) {
				vclip[c] = std::abs(inputs[VCLIP_INPUT].getVoltage(c));
				if (vclip[c] > max_voltage)
					vclip[c] = max_voltage;
			}
			for ( ; c < max_channels; c++)
				vclip[c] = vclip[vchannels - 1];
		}
		else {  // not connected, set to knob value (10V max)
			for (int c = 0; c < max_channels; c++)
				vclip[c] = vclip_knob;
		}

		// Process A input
		for (int c = 0; c < achannels; c++) {
			float vin = inputs[A_INPUT].getVoltage(c);
			float vout;
			if (vin > vclip[c]) {
				vout = vclip[c];
				aclip[c] = true;
			}
			else if (vin < -vclip[c]) {
				vout = -vclip[c];
				aclip[c] = true;
			}
			else {
				vout = vin;
				aclip[c] = false;
			}
			outputs[A_OUTPUT].setVoltage(vout, c);
		}
		outputs[A_OUTPUT].setChannels(achannels);

		// Process B input
		for (int c = 0; c < bchannels; c++) {
			float vin = inputs[B_INPUT].getVoltage(c);
			float vout;
			if (vin > vclip[c]) {
				vout = vclip[c];
				bclip[c] = true;
			}
			else if (vin < -vclip[c]) {
				vout = -vclip[c];
				bclip[c] = true;
			}
			else {
				vout = vin;
				bclip[c] = false;
			}
			outputs[B_OUTPUT].setVoltage(vout, c);
		}
		outputs[B_OUTPUT].setChannels(bchannels);

		// Process A states
		for (int c = 0; c < achannels; c++) {
			// A states
			switch(astate[c]) {

				case LowTimer:
					if (acount[c] == 0)  // low timed out
						astate[c] = LowIdle;
					else
						--acount[c];  // decrement timer
					break;

				case LowIdle:
					if (aclip[c]) {  // clipping true
						astate[c] = HighTimer;
						acount[c] = high_time;
					}
					break;

				case HighTimer:
					if (!logic_mon && aclip[c])  // clip mode, need continuous low to go low
						acount[c] = high_time;  // reset counter each high
					else if (acount[c] == 0) // high timed out
						astate[c] = HighIdle;
					else
						--acount[c];  // decrement timer
					break;

				case HighIdle:
					if (!aclip[c]) {  // clipping false
						astate[c] = LowTimer;
						acount[c] = low_time;
					}
					break;

				default:
					break;  // shouldn't get here
			}
		}

		// Process B states
		for (int c = 0; c < bchannels; c++) {
			// B states
			switch(bstate[c]) {

				case LowTimer:
					if (bcount[c] == 0)  // low timed out
						bstate[c] = LowIdle;
					else
						--bcount[c];  // decrement timer
					break;

				case LowIdle:
					if (bclip[c]) {  // clipping true
						bstate[c] = HighTimer;
						bcount[c] = high_time;
					}
					break;

				case HighTimer:
					if (!logic_mon && bclip[c])  // clip mode, need continuous low to go low
						bcount[c] = high_time;  // reset counter each high
					else if (bcount[c] == 0)  // high timed out
						bstate[c] = HighIdle;
					else
						--bcount[c];  // decrement timer
					break;

				case HighIdle:
					if (!bclip[c]) {  // clipping false
						bstate[c] = LowTimer;
						bcount[c] = low_time;
					}
					break;

				default:
					break;  // shouldn't get here
			}
		}

		// Process lights
		if (check_lights == 0) {
			check_lights = 10;  // every this number of audio cycles

			for (int c = 0; c < 16; c++) {
				// A lights
				if (c < achannels) {  // active A channel
					if (astate[c] == HighIdle || astate[c] == HighTimer) {
						// red light
						lights[A_LIGHTS + 3*c].setBrightness(true);
						lights[A_LIGHTS + 3*c + 2].setBrightness(false);
					}
					else {  // LowIdle or LowTimer
						// blue light
						lights[A_LIGHTS + 3*c].setBrightness(false);
						lights[A_LIGHTS + 3*c + 2].setBrightness(true);
					}
				}
				else {  // inactive A channel
					// light off
					lights[A_LIGHTS + 3*c].setBrightness(false);
					lights[A_LIGHTS + 3*c + 2].setBrightness(false);
				}

				// B lights
				if (c < bchannels) {  // active B channel
					if (bstate[c] == HighIdle || bstate[c] == HighTimer) {
						// red light
						lights[B_LIGHTS + 3*c].setBrightness(true);
						lights[B_LIGHTS + 3*c + 2].setBrightness(false);
					}
					else {  // LowIdle or LowTimer
						// blue light
						lights[B_LIGHTS + 3*c].setBrightness(false);
						lights[B_LIGHTS + 3*c + 2].setBrightness(true);
					}
				}
				else {  // inactive B channel
					// light off
					lights[B_LIGHTS + 3*c].setBrightness(false);
					lights[B_LIGHTS + 3*c + 2].setBrightness(false);
				}
			}
		}
		else {
			--check_lights;  // decrement counter
		}
	}

	void onReset() override {
		// disable logic monitoring mode
		logic_mon = false;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		// logic_mon
		json_object_set_new(rootJ, "logic_mon", json_boolean(logic_mon));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// logic_mon
		json_t* logic_monJ = json_object_get(rootJ, "logic_mon");
		if (logic_monJ)
			logic_mon = json_boolean_value(logic_monJ);
	}
};

struct ClipWidget : ModuleWidget {
	ClipWidget(Clip* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Clip.svg"),
			asset::plugin(pluginInstance, "res/Clip-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 65.00)), module, Clip::A_INPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 74.50)), module, Clip::A_OUTPUT));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 86.25)), module, Clip::B_INPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 95.75)), module, Clip::B_OUTPUT));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 107.50)), module, Clip::VCLIP_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 115.75)), module, Clip::VCLIP_PARAM));

		for (int i = 0; i < 16; i++) {
			addChild(createLightCentered<PetiteLightHalfHalo<RedGreenBlueLight>>(mm2px(Vec(3.40, 16.75 + 2.42*i)), module, Clip::A_LIGHTS + i*3));
			addChild(createLightCentered<PetiteLightHalfHalo<RedGreenBlueLight>>(mm2px(Vec(6.68, 16.75 + 2.42*i)), module, Clip::B_LIGHTS + i*3));
		}
	}

	void appendContextMenu(Menu* menu) override {
		Clip* module = dynamic_cast<Clip*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createBoolPtrMenuItem("Logic Monitoring", "", &module->logic_mon));
	}
};

Model* modelClip = createModel<Clip, ClipWidget>("Clip");
