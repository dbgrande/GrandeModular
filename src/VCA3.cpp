#include "plugin.hpp"


struct VCA3 : Module {
	enum ParamIds {
		GAINA_PARAM,
		GAINB_PARAM,
		GAINC_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INA_INPUT,
		CVA_INPUT,
		INB_INPUT,
		CVB_INPUT,
		INC_INPUT,
		CVC_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTA_OUTPUT,
		OUTB_OUTPUT,
		OUTC_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	VCA3() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GAINA_PARAM, 0.f, 1.f, 1.f, "Gain A", "");
		configParam(GAINB_PARAM, 0.f, 1.f, 1.f, "Gain B", "");
		configParam(GAINC_PARAM, 0.f, 1.f, 1.f, "Gain C", "");
		configInput(INA_INPUT, "A");
		configInput(CVA_INPUT, "CV A");
		configOutput(OUTA_OUTPUT, "A");
		configInput(INB_INPUT, "B");
		configInput(CVB_INPUT, "CV B");
		configOutput(OUTB_OUTPUT, "B");
		configInput(INC_INPUT, "C");
		configInput(CVC_INPUT, "CV C");
		configOutput(OUTC_OUTPUT, "C");
	}

	dsp::SchmittTrigger trigger;
	float vaa[16] = { 0.f };
	float cva[16] = { 0.f };
	float vab[16] = { 0.f };
	float cvb[16] = { 0.f };
	float vac[16] = { 0.f };
	float cvc[16] = { 0.f };

	void process(const ProcessArgs &args) override {

	// VCA A
		float gaina = clamp((params[GAINA_PARAM].getValue()), 0.f, 1.f);

		int channelsa_in = inputs[INA_INPUT].getChannels();
		if (channelsa_in > 16)
			channelsa_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsa_in; c++)
			vaa[c] = inputs[INA_INPUT].getVoltage(c);

		int channelsa_cv = inputs[CVA_INPUT].getChannels();
		// clamp VCA cv input voltages between 0-10V
		if (channelsa_cv > 0) {  // connected
			int c = 0;
			for ( ; c < channelsa_cv; c++)
				cva[c] = clamp(inputs[CVA_INPUT].getVoltage(c), 0.f, 10.f) / 10.f;
			for ( ; c < channelsa_in; c++)
				cva[c] = cva[channelsa_cv - 1];
		}
		else {  // not connected, set all to one
			for (int c = 0; c < channelsa_in; c++)
				cva[c] = 1.f;
		}

		for (int c = 0; c < channelsa_in; c++)
			outputs[OUTA_OUTPUT].setVoltage(vaa[c] * cva[c] * gaina, c);

		outputs[OUTA_OUTPUT].setChannels(channelsa_in);

	// VCA B
		float gainb = clamp((params[GAINB_PARAM].getValue()), 0.f, 1.f);

		int channelsb_in = inputs[INB_INPUT].getChannels();
		if (channelsb_in > 16)
			channelsb_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsb_in; c++)
			vab[c] = inputs[INB_INPUT].getVoltage(c);

		int channelsb_cv = inputs[CVB_INPUT].getChannels();
		// clamp VCA cv input voltages between 0-10V
		if (channelsb_cv > 0) {  // connected
			int c = 0;
			for ( ; c < channelsb_cv; c++)
				cvb[c] = clamp(inputs[CVB_INPUT].getVoltage(c), 0.f, 10.f) / 10.f;
			for ( ; c < channelsb_in; c++)
				cvb[c] = cvb[channelsb_cv - 1];
		}
		else {  // not connected, set all to one
			for (int c = 0; c < channelsb_in; c++)
				cvb[c] = 1.f;
		}

		for (int c = 0; c < channelsb_in; c++)
			outputs[OUTB_OUTPUT].setVoltage(vab[c] * cvb[c] * gainb, c);

		outputs[OUTB_OUTPUT].setChannels(channelsb_in);

	// VCA C
		float gainc = clamp((params[GAINC_PARAM].getValue()), 0.f, 1.f);

		int channelsc_in = inputs[INC_INPUT].getChannels();
		if (channelsc_in > 16)
			channelsc_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channelsc_in; c++)
			vac[c] = inputs[INC_INPUT].getVoltage(c);

		int channelsc_cv = inputs[CVC_INPUT].getChannels();
		// clamp VCA cv input voltages between 0-10V
		if (channelsc_cv > 0) {  // connected
			int c = 0;
			for ( ; c < channelsc_cv; c++)
				cvc[c] = clamp(inputs[CVC_INPUT].getVoltage(c), 0.f, 10.f) / 10.f;
			for ( ; c < channelsc_in; c++)
				cvc[c] = cvc[channelsc_cv - 1];
		}
		else {  // not connected, set all to one
			for (int c = 0; c < channelsc_in; c++)
				cvc[c] = 1.f;
		}

		for (int c = 0; c < channelsc_in; c++)
			outputs[OUTC_OUTPUT].setVoltage(vac[c] * cvc[c] * gainc, c);

		outputs[OUTC_OUTPUT].setChannels(channelsc_in);
	}
};

struct VCA3Widget : ModuleWidget {
	VCA3Widget(VCA3* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCA3.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 17.50)), module, VCA3::INA_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 26.25)), module, VCA3::OUTA_OUTPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 35.00)), module, VCA3::CVA_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 42.50)), module, VCA3::GAINA_PARAM));


		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 54.50)), module, VCA3::INB_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 63.25)), module, VCA3::OUTB_OUTPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 72.00)), module, VCA3::CVB_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 79.50)), module, VCA3::GAINB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 91.50)), module, VCA3::INC_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 100.25)), module, VCA3::OUTC_OUTPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 109.00)), module, VCA3::CVC_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 116.50)), module, VCA3::GAINC_PARAM));
	}
};


Model* modelVCA3 = createModel<VCA3, VCA3Widget>("VCA3");
