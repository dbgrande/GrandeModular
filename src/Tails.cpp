#include "plugin.hpp"


struct Tails : Module {
	enum ParamIds {
		GAIN_PARAM,
		CHANNEL_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		CV_INPUT,
		VOCT_INPUT,
		GATE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		VOCT_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Tails() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GAIN_PARAM, 0.f, 1.f, 1.f, "VCA gain", "");
		configParam(CHANNEL_PARAM, 1, 5, 2, "Channels out", "");
		getParamQuantity(CHANNEL_PARAM)->randomizeEnabled = false;
		configInput(IN_INPUT, "VCA");
		configOutput(OUT_OUTPUT, "VCA");
		configInput(CV_INPUT, "VCA CV");
		configInput(VOCT_INPUT, "Mono pitch");
		configOutput(VOCT_OUTPUT, "Poly pitch");
		configInput(GATE_INPUT, "Mono gate/trigger");
		configOutput(GATE_OUTPUT, "Poly gate/trigger");
		configBypass(IN_INPUT, OUT_OUTPUT);
		configBypass(VOCT_INPUT, VOCT_OUTPUT);
		configBypass(GATE_INPUT, GATE_OUTPUT);
		onReset();
	}

	dsp::SchmittTrigger trigger;
	int last_channels = 0;
	int chan_ptr = 0;
	float va[16] = { 0.f };
	float cv[16] = { 0.f };
	float voct[5] = { 0.f };
	float voct_delay[6] = { 0.f };
	float gate_delay[6] = { 0.f };
	int latch_mode = 0;

	void process(const ProcessArgs &args) override {

	// VCA
		float gain = clamp((params[GAIN_PARAM].getValue()), 0.f, 1.f);

		int channels_in = inputs[IN_INPUT].getChannels();
		if (channels_in > 16)
			channels_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channels_in; c++)
			va[c] = inputs[IN_INPUT].getVoltage(c);

		int channels_cv = inputs[CV_INPUT].getChannels();
		// clamp VCA cv input voltages between 0-10V
		if (channels_cv > 0) {  // connected
			int c = 0;
			for ( ; c < channels_cv; c++)
				cv[c] = clamp(inputs[CV_INPUT].getVoltage(c), 0.f, 10.f) / 10.f;
			for ( ; c < channels_in; c++)
				cv[c] = cv[channels_cv - 1];
		}
		else {  // not connected, set all to one
			for (int c = 0; c < channels_in; c++)
				cv[c] = 1.f;
		}

		for (int c = 0; c < channels_in; c++)
			outputs[OUT_OUTPUT].setVoltage(va[c] * cv[c] * gain, c);

		outputs[OUT_OUTPUT].setChannels(channels_in);

	// Notes splitter
		int channels = clamp((int)(params[CHANNEL_PARAM].getValue()), 1, 5);

		// check for number of channels being decreased
		if (channels < last_channels) {
			if (chan_ptr >= channels) {
				voct[0] = voct[chan_ptr];  // copy current voct to 0, in case gate still on
				chan_ptr = 0;  // wrap pointer to 0 if now pointing to disabled channel
			}
			for (int c = channels; c < last_channels; c++)
				voct[c] = 0.f;  // clear channels as they're turned off
		}
		last_channels = channels;

		// sequential gates distributed across selected number of poly channels

		// read inputs, keeping delayed versions
		for (int i = 5; i > 0; i--) {
			voct_delay[i] = voct_delay[i - 1];
			gate_delay[i] = gate_delay[i - 1];
		}
		voct_delay[0] = inputs[VOCT_INPUT].getVoltage();
		gate_delay[0] = inputs[GATE_INPUT].getVoltage();

		// rising edge of gate increments output channel
		// and latches v/oct value to this channel
		if (latch_mode == 2) {  // latch on next note's gate rising
			if (trigger.process(rescale(gate_delay[0], 0.1f, 2.f, 0.f, 1.f))) {
				chan_ptr = (chan_ptr + 1) % channels;
			}
			// pass v/oct until next note's gate rising
			voct[chan_ptr] = voct_delay[5];  // v/oct delayed 5 cycles
		}
		else if (latch_mode == 1) {  // latch on this note's gate falling
			if (trigger.process(rescale(gate_delay[0], 2.f, 0.1f, 0.f, 1.f))) {
				chan_ptr = (chan_ptr + 1) % channels;
			}
			if (! trigger.isHigh())  // trigger inverted, pass v/oct until this note's gate goes low
				voct[chan_ptr] = voct_delay[0];
		}
		else {  // latch_mode == 0 or any other value, latch on this note's gate rising
			if (trigger.process(rescale(gate_delay[5], 0.1f, 2.f, 0.f, 1.f))) {  // gate delayed 5 cycles
				chan_ptr = (chan_ptr + 1) % channels;
				voct[chan_ptr] = voct_delay[0];  // v/oct latched immediately
			}
		}
		// All enabled v/oct output channels always output each channel's last latched value.
		// The current gate output channel outputs 10V while its input remains high,
		// then it outputs 0V, along with all the other gate output channels.
		for (int c = 0; c < channels; c++) {
			outputs[VOCT_OUTPUT].setVoltage(voct[c], c);
			if (c == chan_ptr) {
				if (latch_mode == 1)  // trigger on fall
					outputs[GATE_OUTPUT].setVoltage((trigger.isHigh() ? 0.f : 10.f), c);
				else  // trigger on rise
					outputs[GATE_OUTPUT].setVoltage((trigger.isHigh() ? 10.f : 0.f), c);
			}
			else
				outputs[GATE_OUTPUT].setVoltage(0.f, c);
		}
		outputs[VOCT_OUTPUT].setChannels(channels);
		outputs[GATE_OUTPUT].setChannels(channels);
	}

	void onReset() override {
		latch_mode = 0;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		// latch_mode
		json_object_set_new(rootJ, "latch_mode", json_integer(latch_mode));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
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

struct TailsWidget : ModuleWidget {
	TailsWidget(Tails* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Tails.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 21.6)), module, Tails::IN_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 33.4)), module, Tails::OUT_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 45.2)), module, Tails::CV_INPUT));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 56.2)), module, Tails::GAIN_PARAM));


		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 73.5)), module, Tails::VOCT_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 82.9)), module, Tails::VOCT_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 94.8)), module, Tails::GATE_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 104.3)), module, Tails::GATE_OUTPUT));

		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(5.08, 115.3)), module, Tails::CHANNEL_PARAM));
	}

	void appendContextMenu(Menu* menu) override {
		Tails* module = dynamic_cast<Tails*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Latch mode", {"Gate rise", "Gate fall", "Next note"}, &module->latch_mode));
	}
};


Model* modelTails = createModel<Tails, TailsWidget>("Tails");
