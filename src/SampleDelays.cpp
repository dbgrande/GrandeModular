#include "plugin.hpp"


struct SampleDelays : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		A_INPUT,
		B_INPUT,
		C_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		A1_OUTPUT,
		A2_OUTPUT,
		B1_OUTPUT,
		B2_OUTPUT,
		C1_OUTPUT,
		C2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	SampleDelays() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(A_INPUT, "A");
		configOutput(A1_OUTPUT, "A1");
		configOutput(A2_OUTPUT, "A2");
		configInput(B_INPUT, "B");
		configOutput(B1_OUTPUT, "B1");
		configOutput(B2_OUTPUT, "B2");
		configInput(C_INPUT, "C");
		configOutput(C1_OUTPUT, "C1");
		configOutput(C2_OUTPUT, "C2");
	}

	int channela1 = 1;
	int channela2 = 1;
	float a1[16] = { 0.f };
	float a2[16] = { 0.f };
	int channelb1 = 1;
	int channelb2 = 1;
	float b1[16] = { 0.f };
	float b2[16] = { 0.f };
	int channelc1 = 1;
	float c1[16] = { 0.f };

	void process(const ProcessArgs &args) override {

		int channela = inputs[A_INPUT].getChannels();
		int channelb = inputs[B_INPUT].getChannels();
		int channelc = inputs[C_INPUT].getChannels();

		// c2 out <- c1
		for (int c = 0; c < channelc1; c++) {
			outputs[C2_OUTPUT].setVoltage(c1[c], c);
		}
		outputs[C2_OUTPUT].setChannels(channelc1);

		// c1 out <- 
		if (channelc == 0) {  // c1 out <- b2
			for (int c = 0; c < channelb2; c++) {
				outputs[C1_OUTPUT].setVoltage(b2[c], c);
				c1[c] = b2[c];
			}
			channelc1 = channelb2;
		} else { // c1 out <- c in
			for (int c = 0; c < channelc; c++) {
				c1[c] = inputs[C_INPUT].getVoltage(c);
				outputs[C1_OUTPUT].setVoltage(c1[c], c);
			}
			channelc1 = channelc;
		}
		outputs[C1_OUTPUT].setChannels(channelc1);

		// b2 out <- b1
		for (int c = 0; c < channelb1; c++) {
			outputs[B2_OUTPUT].setVoltage(b1[c], c);
			b2[c] = b1[c];
		}
		channelb2 = channelb1;
		outputs[B2_OUTPUT].setChannels(channelb2);

		// b1 out <- 
		if (channelb == 0) {  // b1 out <- a2
			for (int c = 0; c < channela1; c++) {
				outputs[B1_OUTPUT].setVoltage(a2[c], c);
				b1[c] = a2[c];
			}
			channelb1 = channela2;
		} else {  // b1 out <- b in
			for (int c = 0; c < channelb; c++) {
				b1[c] = inputs[B_INPUT].getVoltage(c);
				outputs[B1_OUTPUT].setVoltage(b1[c], c);
			}
			channelb1 = channelb;
		}
		outputs[B1_OUTPUT].setChannels(channelb1);

		// a2 out <- a1
		for (int c = 0; c < channela1; c++) {
			outputs[A2_OUTPUT].setVoltage(a1[c], c);
			a2[c] = a1[c];
		}
		channela2 = channela1;
		outputs[A2_OUTPUT].setChannels(channela2);

		// a1 out <-
		if (channela == 0) {  // a1 out <- zero (1 channel)
			a1[0] = 0.f;
			outputs[A1_OUTPUT].setVoltage(a1[0], 0);
			channela1 = 1;
		} else {  //a1 out <- a in
			for (int c = 0; c < channela; c++) {
				a1[c] = inputs[A_INPUT].getVoltage(c);
				outputs[A1_OUTPUT].setVoltage(a1[c], c);
			}
			channela1 = channela;
		}
		outputs[A1_OUTPUT].setChannels(channela1);
	}
};

struct SampleDelaysWidget : ModuleWidget {
	SampleDelaysWidget(SampleDelays* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SampleDelays.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 17.25)), module, SampleDelays::A_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 29.75)), module, SampleDelays::A1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 40.25)), module, SampleDelays::A2_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 54.25)), module, SampleDelays::B_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 66.75)), module, SampleDelays::B1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 77.25)), module, SampleDelays::B2_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 91.25)), module, SampleDelays::C_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 103.75)), module, SampleDelays::C1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 114.25)), module, SampleDelays::C2_OUTPUT));
	}
};


Model* modelSampleDelays = createModel<SampleDelays, SampleDelaysWidget>("SampleDelays");
