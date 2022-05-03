#include "plugin.hpp"


struct VCA4 : Module {
	enum ParamIds {
		GAINA_PARAM,
		GAINB_PARAM,
		GAINC_PARAM,
		GAIND_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INA_INPUT,
		INB_INPUT,
		INC_INPUT,
		IND_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTA_OUTPUT,
		OUTB_OUTPUT,
		OUTC_OUTPUT,
		OUTD_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	VCA4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GAINA_PARAM, 0.f, 1.f, 1.f, "Gain A", "");
		configParam(GAINB_PARAM, 0.f, 1.f, 1.f, "Gain B", "");
		configParam(GAINC_PARAM, 0.f, 1.f, 1.f, "Gain C", "");
		configParam(GAIND_PARAM, 0.f, 1.f, 1.f, "Gain D", "");
		configInput(INA_INPUT, "A");
		configOutput(OUTA_OUTPUT, "A");
		configBypass(INA_INPUT, OUTA_OUTPUT);
		configInput(INB_INPUT, "B");
		configOutput(OUTB_OUTPUT, "B");
		configBypass(INB_INPUT, OUTB_OUTPUT);
		configInput(INC_INPUT, "C");
		configOutput(OUTC_OUTPUT, "C");
		configBypass(INC_INPUT, OUTC_OUTPUT);
		configInput(IND_INPUT, "D");
		configOutput(OUTD_OUTPUT, "D");
		configBypass(IND_INPUT, OUTD_OUTPUT);
	}

	dsp::SchmittTrigger trigger;
	float vaa[16] = { 0.f };
	float vab[16] = { 0.f };
	float vac[16] = { 0.f };
	float vad[16] = { 0.f };

	void process(const ProcessArgs &args) override {

	// VCA A
		float gaina = clamp((params[GAINA_PARAM].getValue()), 0.f, 1.f);

		int channelsa_in = inputs[INA_INPUT].getChannels();
		if (channelsa_in > 16)
			channelsa_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsa_in; c++)
			vaa[c] = inputs[INA_INPUT].getVoltage(c);

		for (int c = 0; c < channelsa_in; c++)
			outputs[OUTA_OUTPUT].setVoltage(vaa[c] * gaina, c);

		outputs[OUTA_OUTPUT].setChannels(channelsa_in);

	// VCA B
		float gainb = clamp((params[GAINB_PARAM].getValue()), 0.f, 1.f);

		int channelsb_in = inputs[INB_INPUT].getChannels();
		if (channelsb_in > 16)
			channelsb_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsb_in; c++)
			vab[c] = inputs[INB_INPUT].getVoltage(c);

		for (int c = 0; c < channelsb_in; c++)
			outputs[OUTB_OUTPUT].setVoltage(vab[c] * gainb, c);

		outputs[OUTB_OUTPUT].setChannels(channelsb_in);

	// VCA C
		float gainc = clamp((params[GAINC_PARAM].getValue()), 0.f, 1.f);

		int channelsc_in = inputs[INC_INPUT].getChannels();
		if (channelsc_in > 16)
			channelsc_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsc_in; c++)
			vac[c] = inputs[INC_INPUT].getVoltage(c);

		for (int c = 0; c < channelsc_in; c++)
			outputs[OUTC_OUTPUT].setVoltage(vac[c] * gainc, c);

		outputs[OUTC_OUTPUT].setChannels(channelsc_in);

	// VCA D
		float gaind = clamp((params[GAIND_PARAM].getValue()), 0.f, 1.f);

		int channelsd_in = inputs[IND_INPUT].getChannels();
		if (channelsd_in > 16)
			channelsd_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsc_in; c++)
			vad[c] = inputs[IND_INPUT].getVoltage(c);

		for (int c = 0; c < channelsc_in; c++)
			outputs[OUTD_OUTPUT].setVoltage(vad[c] * gaind, c);

		outputs[OUTD_OUTPUT].setChannels(channelsc_in);
	}
};

struct VCA4Widget : ModuleWidget {
	VCA4Widget(VCA4* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCA4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 15.75)), module, VCA4::INA_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 24.88)), module, VCA4::OUTA_OUTPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 33.00)), module, VCA4::GAINA_PARAM));


		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 43.50)), module, VCA4::INB_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 52.63)), module, VCA4::OUTB_OUTPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 60.75)), module, VCA4::GAINB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 71.25)), module, VCA4::INC_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 80.38)), module, VCA4::OUTC_OUTPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 88.50)), module, VCA4::GAINC_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 99.00)), module, VCA4::IND_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 108.13)), module, VCA4::OUTD_OUTPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 116.25)), module, VCA4::GAIND_PARAM));
	}
};


Model* modelVCA4 = createModel<VCA4, VCA4Widget>("VCA4");
