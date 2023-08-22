#include "plugin.hpp"


struct Tails4 : Module {
	enum ParamIds {
		VOCT_MODE_PARAM,
		GATE_MODE_PARAM,
		CHANNEL_PARAM,
		VCA_GAIN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(VOCT_INPUTS, 4),
		ENUMS(GATE_INPUTS, 4),
		RESPLIT_INPUT,
		VCA_INPUT,
		VCA_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		VOCT_OUTPUT,
		GATE_OUTPUT,
		ENUMS(RESPLIT_OUTPUTS, 4),
		VCA_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		VOCT_MODE_LIGHT,
		GATE_MODE_LIGHT,
		ENUMS(VOCT_LIGHTS, 4),
		ENUMS(GATE_LIGHTS, 4),
		VOCT_OVER_LIGHT,
		GATE_OVER_LIGHT,
		CHANNEL_OVER_LIGHT,
		ENUMS(RESPLIT_TOP_LIGHTS, 4 * 3),
		ENUMS(RESPLIT_BOT_LIGHTS, 4 * 3),
		NUM_LIGHTS
	};

	Tails4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(CHANNEL_PARAM, 1, 5, 2, "Channels per note", "");
		getParamQuantity(CHANNEL_PARAM)->randomizeEnabled = false;
		configParam(VCA_GAIN_PARAM, 0.f, 1.f, 1.f, "VCA gain", "");

		configButton(VOCT_MODE_PARAM, "Pitch poly-merge mode");
		configButton(GATE_MODE_PARAM, "Gate poly-merge mode");

		configInput(VOCT_INPUTS, "Pitch 1");
		configInput(VOCT_INPUTS + 1, "Pitch 2");
		configInput(VOCT_INPUTS + 2, "Pitch 3");
		configInput(VOCT_INPUTS + 3, "Pitch 4");
		configInput(GATE_INPUTS, "Gate 1");
		configInput(GATE_INPUTS + 1, "Gate 2");
		configInput(GATE_INPUTS + 2, "Gate 3");
		configInput(GATE_INPUTS + 3, "Gate 4");
		configInput(VCA_INPUT, "VCA");
		configInput(VCA_CV_INPUT, "VCA CV");
		configInput(RESPLIT_INPUT, "Poly-resplit");

		configOutput(VOCT_OUTPUT, "Pitch");
		configOutput(GATE_OUTPUT, "Gate/trigger");
		configOutput(VCA_OUTPUT, "VCA");
		configOutput(RESPLIT_OUTPUTS, "1st poly");
		configOutput(RESPLIT_OUTPUTS + 1, "2nd poly");
		configOutput(RESPLIT_OUTPUTS + 2, "3rd poly");
		configOutput(RESPLIT_OUTPUTS + 3, "4th poly");

		configBypass(VOCT_INPUTS, VOCT_OUTPUT);
		configBypass(GATE_INPUTS, GATE_OUTPUT);
		configBypass(VCA_INPUT, VCA_OUTPUT);

		onReset();
	}

	dsp::SchmittTrigger trigger[4];

	bool voct_mode = false;
	bool gate_mode = false;
	int latch_mode = 0;

	int last_vmode_in = -1;
	int last_gmode_in = -1;
	int last_channels = 0;

	int voct_chans[4] = { 0 };
	float voct_vin[4] = { 0.f };
	int voct_n = 0;
	int gate_chans[4] = { 0 };
	float gate_vin[4] = { 0.f };
	int gate_n = 0;

	int chan_ptr[4] = { 0 };
	float voct[4][5] = { 0.f };
	float voct_delay[4][6] = { 0.f };
	float gate_delay[4][6] = { 0.f };

	float vca_vin[16] = { 0.f };
	float vca_cvin[16] = { 0.f };


	void process(const ProcessArgs &args) override {
		// Read VOCT Mode button
		int vmode_in = clamp((int)(params[VOCT_MODE_PARAM].getValue()), 0, 1);
		if (vmode_in == 1) {
			if (last_vmode_in == 0) {
				voct_mode = !voct_mode;
				last_vmode_in = 1;
			}
		}
		else
			last_vmode_in = 0;
		lights[VOCT_MODE_LIGHT].setBrightness(voct_mode);

		// Read GATE Mode button
		int gmode_in = clamp((int)(params[GATE_MODE_PARAM].getValue()), 0, 1);
		if (gmode_in == 1) {
			if (last_gmode_in == 0) {
				gate_mode = !gate_mode;
				last_gmode_in = 1;
			}
		}
		else
			last_gmode_in = 0;
		lights[GATE_MODE_LIGHT].setBrightness(gate_mode);

	// Check input connections
		int voct_top = -1;
		int gate_top = -1;
		for (int i = 0; i < 4; i++) {
			voct_chans[i] = inputs[VOCT_INPUTS + i].getChannels();
			if (voct_chans[i] > 0)  // voct connected
				voct_top = i;
			gate_chans[i] = inputs[GATE_INPUTS + i].getChannels();
			if (gate_chans[i] > 0)  // gate_connected
				gate_top = i;
		}
		// VOCT inputs
		if (voct_mode == false) {  // monophonic mode, normalize to lower channels
			for (int i = 3; i >= 0; i--) {
				if (i > voct_top) { // unconnected, no value
					voct_vin[i] = 0.f;
					lights[VOCT_LIGHTS + i].setBrightness(false);
				}
				else {
					if (voct_chans[i] > 0)  // connected
						voct_vin[i] = inputs[VOCT_INPUTS + i].getVoltage();
					else  // unconnected, but higher value
						voct_vin[i] = voct_vin[i + 1];
					lights[VOCT_LIGHTS + i].setBrightness(true);
				}
			}
		}
		else {  // voct_mode == polyphonic merge mode
			voct_n = 0;
			bool voct_over = false;
			for (int i = 0; i < 4; i++) {
				if (voct_chans[i] > 0) {  // connected
					for (int c = 0; c < voct_chans[i]; c++) {
						if (voct_n < 4) {
							voct_vin[voct_n] = inputs[VOCT_INPUTS + i].getVoltage(c);
							lights[VOCT_LIGHTS + voct_n].setBrightness(true);
							voct_n++;
						}
						else {
							lights[VOCT_OVER_LIGHT].setBrightness(true);
							voct_over = true;
							break;
						}
					}
				}
			}
			for (int i = voct_n; i < 4; i++) {
				lights[VOCT_LIGHTS + i].setBrightness(false);
			}
			if (voct_over == false)  // didn't go over 4 total channels
				lights[VOCT_OVER_LIGHT].setBrightness(false);
		}
		// Gate inputs
		if (gate_mode == false) {  // monophonic mode, normalize to lower channels
			for (int i = 3; i >= 0; i--) {
				if (i > gate_top) { // unconnected, no value
					gate_vin[i] = 0.f;
					lights[GATE_LIGHTS + i].setBrightness(false);
				}
				else {
					if (gate_chans[i] > 0)  // connected
						gate_vin[i] = inputs[GATE_INPUTS + i].getVoltage();
					else  // unconnected, but higher value
						gate_vin[i] = gate_vin[i + 1];
					lights[GATE_LIGHTS + i].setBrightness(true);
				}
			}
		}
		else {  // gate_mode == polyphonic merge mode
			gate_n = 0;
			bool gate_over = false;
			for (int i = 0; i < 4; i++) {
				if (gate_chans[i] > 0) {  // connected
					for (int c = 0; c < gate_chans[i]; c++) {
						if (gate_n < 4) {
							gate_vin[gate_n] = inputs[GATE_INPUTS + i].getVoltage(c);
							lights[GATE_LIGHTS + gate_n].setBrightness(true);
							gate_n++;
						}
						else {
							lights[GATE_OVER_LIGHT].setBrightness(true);
							gate_over = true;
							break;
						}
					}
				}
			}
			for (int i = gate_n; i < 4; i++) {
				lights[GATE_LIGHTS + i].setBrightness(false);
			}
			if (gate_over == false)  // didn't go over 4 total channels
				lights[GATE_OVER_LIGHT].setBrightness(false);
		}
		// Tails mode (compare voct_top and gate_top)
		int tails_count = 1;
		for (int i = 0; i < 4; i++) {
			if (((!voct_mode && i < voct_top) || (voct_mode && i < (voct_n - 1))) && ((!gate_mode && i < gate_top) || (gate_mode && i < (gate_n - 1))))
				tails_count += 1;
		}

	// Channel selection knob
		int channels = clamp((int)(params[CHANNEL_PARAM].getValue()), 1, 5);

		// Check if too many total channels
		if (channels == 5 && tails_count == 4) {
			channels = 4;
			lights[CHANNEL_OVER_LIGHT].setBrightness(true);
		}
		else {
			lights[CHANNEL_OVER_LIGHT].setBrightness(false);
		}

		// check for number of channels being decreased
		if (channels < last_channels) {
			for (int t = 0; t < tails_count; t++) {
				if (chan_ptr[t] >= channels) {
					voct[t][0] = voct[t][chan_ptr[t]];  // copy current voct to 0, in case gate still on
					chan_ptr[t] = 0;  // wrap pointer to 0 if now pointing to disabled channel
				}
				for (int c = channels; c < last_channels; c++)
					voct[t][c] = 0.f;  // clear channels as they're turned off
			}
		}
		last_channels = channels;

	// Process up to 4 Tails streams
		// sequential gates distributed across selected number of poly channels

		for (int t = 0; t < tails_count; t++) {
			// read inputs, keeping delayed versions
			for (int i = 5; i > 0; i--) {
				voct_delay[t][i] = voct_delay[t][i - 1];
				gate_delay[t][i] = gate_delay[t][i - 1];
			}
			voct_delay[t][0] = voct_vin[t];
			gate_delay[t][0] = gate_vin[t];

			// rising edge of gate increments output channel
			// and latches v/oct value to this channel
			if (latch_mode == 2) {  // latch on next note's gate rising
				if (trigger[t].process(rescale(gate_delay[t][0], 0.1f, 2.f, 0.f, 1.f))) {
					chan_ptr[t] = (chan_ptr[t] + 1) % channels;
				}
				// pass v/oct until next note's gate rising
				voct[t][chan_ptr[t]] = voct_delay[t][5];  // v/oct delayed 5 cycles
			}
			else if (latch_mode == 1) {  // latch on this note's gate falling
				if (trigger[t].process(rescale(gate_delay[t][0], 2.f, 0.1f, 0.f, 1.f))) {
					chan_ptr[t] = (chan_ptr[t] + 1) % channels;
				}
				if (! trigger[t].isHigh())  // trigger inverted, pass v/oct until this note's gate goes low
					voct[t][chan_ptr[t]] = voct_delay[t][0];
			}
			else {  // latch_mode == 0 or any other value, latch on this note's gate rising
				if (trigger[t].process(rescale(gate_delay[t][5], 0.1f, 2.f, 0.f, 1.f))) {  // gate delayed 5 cycles
					chan_ptr[t] = (chan_ptr[t] + 1) % channels;
					voct[t][chan_ptr[t]] = voct_delay[t][0];  // v/oct latched immediately
				}
			}
		}
		// All enabled v/oct output channels always output each channel's last latched value.
		// The current gate output channel outputs 10V while its input remains high,
		// then it outputs 0V, along with all the other gate output channels.
		for (int t = 0; t < tails_count; t++) {
			for (int c = 0; c < channels; c++) {
				outputs[VOCT_OUTPUT].setVoltage(voct[t][c], t * channels + c);
				if (c == chan_ptr[t]) {
					if (latch_mode == 1)  // trigger on fall (trigger is inverted)
						outputs[GATE_OUTPUT].setVoltage((trigger[t].isHigh() ? 0.f : 10.f), t * channels + c);
					else  // trigger on rise
						outputs[GATE_OUTPUT].setVoltage((trigger[t].isHigh() ? 10.f : 0.f), t * channels + c);
				}
				else
					outputs[GATE_OUTPUT].setVoltage(0.f, t * channels + c);
			}
		}
		outputs[VOCT_OUTPUT].setChannels(tails_count * channels);
		outputs[GATE_OUTPUT].setChannels(tails_count * channels);

	// Resplit //
		// Each active output should have 'channels' channels
		int ch_m[4];
		for (int t = 0; t < 4; t++) {
			if (t < tails_count)
				ch_m[t] = channels;
			else
				ch_m[t] = 0;
		}
		int ch_r = 0;
		if (inputs[RESPLIT_INPUT].isConnected())
			ch_r = inputs[RESPLIT_INPUT].getChannels();
		int num_r[4] = { 0 };
		for (int c = 0; c < ch_r; c++) {
			float vr = inputs[RESPLIT_INPUT].getVoltage(c);
			if ((ch_m[1] + ch_m[2] + ch_m[3] == 0) || (c < ch_m[0])) {
				outputs[RESPLIT_OUTPUTS].setVoltage(vr, c);
				num_r[0]++;
			}
			else if ((ch_m[2] + ch_m[3] == 0) || (c < ch_m[0] + ch_m[1])) {
				outputs[RESPLIT_OUTPUTS + 1].setVoltage(vr, c - ch_m[0]);
				num_r[1]++;
			}
			else if ((ch_m[3] == 0) || (c < ch_m[0] + ch_m[1] + ch_m[2])) {
				outputs[RESPLIT_OUTPUTS + 2].setVoltage(vr, c - ch_m[0] - ch_m[1]);
				num_r[2]++;
			}
			else {
				outputs[RESPLIT_OUTPUTS + 3].setVoltage(vr, c - ch_m[0] - ch_m[1] - ch_m[2]);
				num_r[3]++;
			}
		}
		// outputs
		for (int n = 0; n < 4; n++)
			outputs[RESPLIT_OUTPUTS + n].channels = num_r[n];

		// lights
		for (int n = 0; n < 4; n++) {
			if (ch_r == 0 || num_r[n] == ch_m[n]) {  // input unconnected, or match
				lights[RESPLIT_TOP_LIGHTS + n*3].setBrightness(0);
				lights[RESPLIT_BOT_LIGHTS + n*3].setBrightness(0);
				lights[RESPLIT_TOP_LIGHTS + n*3 + 2].setBrightness(num_r[n] > 0);
				lights[RESPLIT_BOT_LIGHTS + n*3 + 2].setBrightness(num_r[n] > 0);
			}
			else {  // mismatch
				lights[RESPLIT_TOP_LIGHTS + n*3].setBrightness(num_r[n] > 0 && num_r[n] > ch_m[n] ? 1.f : 0.1f);
				lights[RESPLIT_BOT_LIGHTS + n*3].setBrightness(num_r[n] > 0 && num_r[n] > ch_m[n] ? 0.1f : 1.f);
				lights[RESPLIT_TOP_LIGHTS + n*3 + 2].setBrightness(0);
				lights[RESPLIT_BOT_LIGHTS + n*3 + 2].setBrightness(0);
			}
		}

	// VCA
		float gain = clamp((params[VCA_GAIN_PARAM].getValue()), 0.f, 1.f);

		int channels_in = inputs[VCA_INPUT].getChannels();
		if (channels_in > 16)
			channels_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channels_in; c++)
			vca_vin[c] = inputs[VCA_INPUT].getVoltage(c);

		int channels_cv = inputs[VCA_CV_INPUT].getChannels();
		// clamp VCA cv input voltages between 0-10V
		if (channels_cv > 0) {  // connected
			int c = 0;
			for ( ; c < channels_cv; c++)
				vca_cvin[c] = clamp(inputs[VCA_CV_INPUT].getVoltage(c), 0.f, 10.f) / 10.f;
			for ( ; c < channels_in; c++)
				vca_cvin[c] = vca_cvin[channels_cv - 1];
		}
		else {  // not connected, set all to one
			for (int c = 0; c < channels_in; c++)
				vca_cvin[c] = 1.f;
		}
		for (int c = 0; c < channels_in; c++)
			outputs[VCA_OUTPUT].setVoltage(vca_vin[c] * vca_cvin[c] * gain, c);

		outputs[VCA_OUTPUT].setChannels(channels_in);
	}

	void onReset() override {
		voct_mode = false;
		gate_mode = false;
		latch_mode = 0;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		// voct_mode
		json_object_set_new(rootJ, "voct_mode", json_boolean(voct_mode));
		// gate_mode
		json_object_set_new(rootJ, "gate_mode", json_boolean(gate_mode));
		// latch_mode
		json_object_set_new(rootJ, "latch_mode", json_integer(latch_mode));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// voct_mode
		json_t* voct_modeJ = json_object_get(rootJ, "voct_mode");
		if (voct_modeJ)
			voct_mode = json_boolean_value(voct_modeJ);
		// gate_mode
		json_t* gate_modeJ = json_object_get(rootJ, "gate_mode");
		if (gate_modeJ)
			gate_mode = json_boolean_value(gate_modeJ);
		// latch_mode
		json_t* latch_modeJ = json_object_get(rootJ, "latch_mode");
		if (latch_modeJ)
			latch_mode = json_integer_value(latch_modeJ);
		// Legacy pass_notes support (if pass_notes was true, convert to latch_mode 2)
		json_t* pass_notesJ = json_object_get(rootJ, "pass_notes");
		if (pass_notesJ && json_boolean_value(pass_notesJ))
			latch_mode = 2;
	}
};

struct Tails4Widget : ModuleWidget {
	Tails4Widget(Tails4* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Tails4.svg"),
			asset::plugin(pluginInstance, "res/Tails4-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(0, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Tails IOs
		for (int i = 0; i < 4; i++) {
			addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.444, 18.75 + 9.25*i)), module, Tails4::VOCT_INPUTS + i));
			addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(14.877, 18.75 + 9.25*i)), module, Tails4::GATE_INPUTS + i));
		}
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.444, 59.75)), module, Tails4::VOCT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(14.877, 59.75)), module, Tails4::GATE_OUTPUT));

		// Tails IO lights
		for (int i =  0; i < 4; i++) {
			addChild(createLightCentered<PetiteLight<BlueLight>>(mm2px(Vec(8.726, 14.75 + 9.25*i)), module, Tails4::VOCT_LIGHTS + i));
			addChild(createLightCentered<PetiteLight<BlueLight>>(mm2px(Vec(18.16, 14.75 + 9.25*i)), module, Tails4::GATE_LIGHTS + i));
		}
		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.726, 55.75)), module, Tails4::VOCT_OVER_LIGHT));
		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(18.16, 55.75)), module, Tails4::GATE_OVER_LIGHT));

		// Tails mode buttons
        addChild(createLightCentered<SmallLightFlat<BlueLight>>(mm2px(Vec(2.00, 51.30)), module, Tails4::VOCT_MODE_LIGHT));
        addParam(createParam<SmallLEDButton>(mm2px(Vec(2.00-1.5, 51.30-1.5)), module, Tails4::VOCT_MODE_PARAM));

        addChild(createLightCentered<SmallLightFlat<BlueLight>>(mm2px(Vec(11.433, 51.30)), module, Tails4::GATE_MODE_LIGHT));
        addParam(createParam<SmallLEDButton>(mm2px(Vec(11.433-1.5, 51.30-1.5)), module, Tails4::GATE_MODE_PARAM));

		// Channel knob
		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(5.08, 71.50)), module, Tails4::CHANNEL_PARAM));
		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.40, 68.80)), module, Tails4::CHANNEL_OVER_LIGHT));

		// VCA
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 86.0)), module, Tails4::VCA_INPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 97.0)), module, Tails4::VCA_OUTPUT));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 108.60)), module, Tails4::VCA_CV_INPUT));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 116.26)), module, Tails4::VCA_GAIN_PARAM));
		
		// Resplit //
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(15.24, 74.25)), module, Tails4::RESPLIT_INPUT));
		for (int i =  0; i < 4; i++) {
			addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(15.24, 85.00 + 10*i)), module, Tails4::RESPLIT_OUTPUTS + i));
			// lights
			addChild(createLightCentered<PetiteLightTop<RedGreenBlueLight>>(mm2px(Vec(18.56, 81.00 + 10*i)), module, Tails4::RESPLIT_TOP_LIGHTS + i*3));
			addChild(createLightCentered<PetiteLightBot<RedGreenBlueLight>>(mm2px(Vec(18.56, 81.00 + 10*i)), module, Tails4::RESPLIT_BOT_LIGHTS + i*3));
		}
	}

	void appendContextMenu(Menu* menu) override {
		Tails4* module = dynamic_cast<Tails4*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Latch mode", {"Gate rise", "Gate fall", "Next note"}, &module->latch_mode));
	}
};


Model* modelTails4 = createModel<Tails4, Tails4Widget>("Tails4");
