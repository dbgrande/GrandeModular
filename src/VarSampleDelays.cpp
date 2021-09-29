#include "plugin.hpp"


struct VarSampleDelays : Module {
	enum ParamIds {
		A_PARAM,
		B_PARAM,
		C_PARAM,
		D_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		A_INPUT,
		B_INPUT,
		C_INPUT,
		D_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		A_OUTPUT,
		B_OUTPUT,
		C_OUTPUT,
		D_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	VarSampleDelays() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(A_PARAM, 1, 9, 5, "Sample delays", "");
		configParam(B_PARAM, 1, 9, 5, "Sample delays", "");
		configParam(C_PARAM, 1, 9, 5, "Sample delays", "");
		configParam(D_PARAM, 1, 9, 5, "Sample delays", "");
		configInput(A_INPUT, "A");
		configOutput(A_OUTPUT, "A");
		configInput(B_INPUT, "B");
		configOutput(B_OUTPUT, "B");
		configInput(C_INPUT, "C");
		configOutput(C_OUTPUT, "C");
		configInput(D_INPUT, "D");
		configOutput(D_OUTPUT, "D");
		configBypass(A_INPUT, A_OUTPUT);
		configBypass(B_INPUT, B_OUTPUT);
		configBypass(C_INPUT, C_OUTPUT);
		configBypass(D_INPUT, D_OUTPUT);
	}

	int channels_a[9] = { 0 };
	int channels_b[9] = { 0 };
	int channels_c[9] = { 0 };
	int channels_d[9] = { 0 };

	float fifo_a[9][16] = { 0.f };
	float fifo_b[9][16] = { 0.f };
	float fifo_c[9][16] = { 0.f };
	float fifo_d[9][16] = { 0.f };

	int last_out_a = 0;
	int last_out_b = 0;
	int last_out_c = 0;
	int last_out_d = 0;

	void process(const ProcessArgs &args) override {

		// A //
		int out_a = clamp((int)(params[A_PARAM].getValue()), 1, 9) - 1;
		// only increase/decrease by one to reduce glitches
		if (out_a > last_out_a)
			out_a = last_out_a + 1;
		else if (out_a < last_out_a)
			out_a = last_out_a - 1;
		last_out_a = out_a;
		// shift upper fifo stages, if used
		for (int n = out_a; n > 0; n--) {
			channels_a[n] = channels_a[n-1];
			for (int c = 0; c < channels_a[n]; c++) {
				fifo_a[n][c] = fifo_a[n-1][c];
			}
		}
		// input -> fifo 0
		channels_a[0] = inputs[A_INPUT].getChannels();
		for (int c = 0; c < channels_a[0]; c++) {
			fifo_a[0][c] = inputs[A_INPUT].getVoltage(c);
		}
		// fifo top -> output
		for (int c = 0; c < channels_a[out_a]; c++) {
			outputs[A_OUTPUT].setVoltage(fifo_a[out_a][c], c);
		}
		outputs[A_OUTPUT].setChannels(channels_a[out_a]);

		// B //
		int out_b = clamp((int)(params[B_PARAM].getValue()), 1, 9) - 1;
		// only increase/decrease by one to reduce glitches
		if (out_b > last_out_b)
			out_b = last_out_b + 1;
		else if (out_b < last_out_b)
			out_b = last_out_b - 1;
		last_out_b = out_b;
		// shift upper fifo stages, if used
		for (int n = out_b; n > 0; n--) {
			channels_b[n] = channels_b[n-1];
			for (int c = 0; c < channels_b[n]; c++) {
				fifo_b[n][c] = fifo_b[n-1][c];
			}
		}
		// input -> fifo 0
		channels_b[0] = inputs[B_INPUT].getChannels();
		for (int c = 0; c < channels_b[0]; c++) {
			fifo_b[0][c] = inputs[B_INPUT].getVoltage(c);
		}
		// fifo top -> output
		for (int c = 0; c < channels_b[out_b]; c++) {
			outputs[B_OUTPUT].setVoltage(fifo_b[out_b][c], c);
		}
		outputs[B_OUTPUT].setChannels(channels_b[out_b]);

		// C //
		int out_c = clamp((int)(params[C_PARAM].getValue()), 1, 9) - 1;
		// only increase/decrease by one to reduce glitches
		if (out_c > last_out_c)
			out_c = last_out_c + 1;
		else if (out_c < last_out_c)
			out_c = last_out_c - 1;
		last_out_c = out_c;
		// shift upper fifo stages, if used
		for (int n = out_c; n > 0; n--) {
			channels_c[n] = channels_c[n-1];
			for (int c = 0; c < channels_c[n]; c++) {
				fifo_c[n][c] = fifo_c[n-1][c];
			}
		}
		// input -> fifo 0
		channels_c[0] = inputs[C_INPUT].getChannels();
		for (int c = 0; c < channels_c[0]; c++) {
			fifo_c[0][c] = inputs[C_INPUT].getVoltage(c);
		}
		// fifo top -> output
		for (int c = 0; c < channels_c[out_c]; c++) {
			outputs[C_OUTPUT].setVoltage(fifo_c[out_c][c], c);
		}
		outputs[C_OUTPUT].setChannels(channels_c[out_c]);

		// D //
		int out_d = clamp((int)(params[D_PARAM].getValue()), 1, 9) - 1;
		// only increase/decrease by one to reduce glitches
		if (out_d > last_out_d)
			out_d = last_out_d + 1;
		else if (out_d < last_out_d)
			out_d = last_out_d - 1;
		last_out_d = out_d;
		// shift upper fifo stages, if used
		for (int n = out_d; n > 0; n--) {
			channels_d[n] = channels_d[n-1];
			for (int c = 0; c < channels_d[n]; c++) {
				fifo_d[n][c] = fifo_d[n-1][c];
			}
		}
		// input -> fifo 0
		channels_d[0] = inputs[D_INPUT].getChannels();
		for (int c = 0; c < channels_d[0]; c++) {
			fifo_d[0][c] = inputs[D_INPUT].getVoltage(c);
		}
		// fifo top -> output
		for (int c = 0; c < channels_d[out_d]; c++) {
			outputs[D_OUTPUT].setVoltage(fifo_d[out_d][c], c);
		}
		outputs[D_OUTPUT].setChannels(channels_d[out_d]);

	}
};

struct VarSampleDelaysWidget : ModuleWidget {
	VarSampleDelaysWidget(VarSampleDelays* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VarSampleDelays.svg")));

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 15.75)), module, VarSampleDelays::A_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 24.875)), module, VarSampleDelays::A_OUTPUT));
		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(5.08, 33.0)), module, VarSampleDelays::A_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 43.5)), module, VarSampleDelays::B_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 52.625)), module, VarSampleDelays::B_OUTPUT));
		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(5.08, 60.75)), module, VarSampleDelays::B_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 71.25)), module, VarSampleDelays::C_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 80.375)), module, VarSampleDelays::C_OUTPUT));
		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(5.08, 88.5)), module, VarSampleDelays::C_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 99.0)), module, VarSampleDelays::D_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 108.125)), module, VarSampleDelays::D_OUTPUT));
		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(5.08, 116.25)), module, VarSampleDelays::D_PARAM));
	}
};


Model* modelVarSampleDelays = createModel<VarSampleDelays, VarSampleDelaysWidget>("VarSampleDelays");
