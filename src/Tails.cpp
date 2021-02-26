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
		configParam(GAIN_PARAM, 0.f, 1.f, 1.f, "Gain", "");
		configParam(CHANNEL_PARAM, 1, 5, 2, "Channels out", "");
	}

	dsp::SchmittTrigger trigger;
	int last_channels = 0;
	int chan_ptr = 0;
	float va[16] = { 0.f };
	float cv[16] = { 0.f };
	float voct[5] = { 0.f };

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
		float gv = inputs[GATE_INPUT].getVoltage();

		// rising edge of gate increments output channel
		// and latches v/oct value to this channel
		if (trigger.process(rescale(gv, 0.1f, 2.f, 0.f, 1.f))) {
			chan_ptr = (chan_ptr + 1) % channels;
			voct[chan_ptr] = inputs[VOCT_INPUT].getVoltage();
		}
		// All enabled v/oct output channels always output each channel's last latched value.
		// The current gate output channel outputs 10V while its input remains high,
		// then it outputs 0V, along with all the other gate output channels.
		for (int c = 0; c < channels; c++) {
			outputs[VOCT_OUTPUT].setVoltage(voct[c], c);
			if (c == chan_ptr)
				outputs[GATE_OUTPUT].setVoltage((trigger.isHigh() ? 10.f : 0.f), c);
			else
				outputs[GATE_OUTPUT].setVoltage(0.f, c);
		}
		outputs[VOCT_OUTPUT].setChannels(channels);
		outputs[GATE_OUTPUT].setChannels(channels);

	}
};

struct TailsWidget : ModuleWidget {
	TailsWidget(Tails* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Tails.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 21.5)), module, Tails::IN_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 33.5)), module, Tails::OUT_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 46.0)), module, Tails::CV_INPUT));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 57.0)), module, Tails::GAIN_PARAM));


		addInput(createInputCentered<SmallPort>(mm2px(Vec(5.08, 74.0)), module, Tails::VOCT_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 82.8)), module, Tails::VOCT_OUTPUT));

		addInput(createInputCentered<SmallPort>(mm2px(Vec(5.08, 94.5)), module, Tails::GATE_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 103.3)), module, Tails::GATE_OUTPUT));

		addParam(createParamCentered<RoundTinyRotarySwitchNoRandom>(mm2px(Vec(5.08, 115.0)), module, Tails::CHANNEL_PARAM));
	}
};


Model* modelTails = createModel<Tails, TailsWidget>("Tails");
