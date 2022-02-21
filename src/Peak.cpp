#include "plugin.hpp"


struct Peak : Module {
	enum ParamIds {
		RESETA_PARAM,
		RESETB_PARAM,
		THRESHOLD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		A_INPUT,
		B_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		MAXA_OUTPUT,
		MINA_OUTPUT,
		MAXB_OUTPUT,
		MINB_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		MAXA_LIGHT,
		MINA_LIGHT,
		MAXB_LIGHT,
		MINB_LIGHT,
		NUM_LIGHTS
	};

	Peak() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RESETA_PARAM, 0.0, 1.0, 0.0, "Reset A", "");
		configParam(RESETB_PARAM, 0.0, 1.0, 0.0, "Reset B", "");
		configParam(THRESHOLD_PARAM, 0.0, 12.0, 12.0, "Threshold for lights", "V");
		configInput(A_INPUT, "A");
		configOutput(MAXA_OUTPUT, "Max A");
		configOutput(MINA_OUTPUT, "Min A");
		configInput(B_INPUT, "B");
		configOutput(MAXB_OUTPUT, "Max B");
		configOutput(MINB_OUTPUT, "Min B");
	}

int last_reseta = 0;
int last_resetb = 0;
float highest_a[16] = { -INFINITY };
float lowest_a[16] = { INFINITY };
float highest_b[16] = { -INFINITY };
float lowest_b[16] = { INFINITY };

	void process(const ProcessArgs &args) override {
		// Threshold for lights
		float threshold = clamp((params[THRESHOLD_PARAM].getValue()), 0.0, 12.0);

		// A Section
		int reseta = clamp((int)(params[RESETA_PARAM].getValue()), 0, 1);
		if (last_reseta == 0 && reseta == 1) {
			for (int c = 0; c < 16; c++) {
				highest_a[c] = -INFINITY;
				lowest_a[c] = INFINITY;
			}
		}
		last_reseta = reseta;

		int channels_a = inputs[A_INPUT].getChannels();
		float vin;
		for (int c = 0; c < channels_a; c++) {
			vin = inputs[A_INPUT].getVoltage(c);
			//if (std::isnan(vin))  // NaNs have highest priority
			//	highest_a[c] = lowest_a[c] = vin;
			//else {
				if (vin > highest_a[c])
					highest_a[c] = vin;
				if (vin < lowest_a[c])
					lowest_a[c] = vin;
			//}
			outputs[MAXA_OUTPUT].setVoltage(highest_a[c], c);
			outputs[MINA_OUTPUT].setVoltage(lowest_a[c], c);
		}
		// Always output values, without needing connection.
		outputs[MAXA_OUTPUT].channels = channels_a;
		outputs[MINA_OUTPUT].channels = channels_a;

		// lights
		bool over = false;
		bool under = false;
		for (int c = 0; c < channels_a; c++) {
			if (highest_a[c] > threshold)
				over = true;
			if (lowest_a[c] < -threshold)
				under = true;
		}
		lights[MAXA_LIGHT].setBrightness(over);
		lights[MINA_LIGHT].setBrightness(under);

		// B Section
		int resetb = clamp((int)(params[RESETB_PARAM].getValue()), 0, 1);
		if (last_resetb == 0 && resetb == 1) {
			for (int c = 0; c < 16; c++) {
				highest_b[c] = -INFINITY;
				lowest_b[c] = INFINITY;
			}
		}
		last_resetb = resetb;

		int channels_b = inputs[B_INPUT].getChannels();
		for (int c = 0; c < channels_b; c++) {
			vin = inputs[B_INPUT].getVoltage(c);
			//if (std::isnan(vin))  // NaNs have highest priority
			//	highest_b[c] = lowest_b[c] = vin;
			//else {
				if (vin > highest_b[c])
					highest_b[c] = vin;
				if (vin < lowest_b[c])
					lowest_b[c] = vin;
			//}
			outputs[MAXB_OUTPUT].setVoltage(highest_b[c], c);
			outputs[MINB_OUTPUT].setVoltage(lowest_b[c], c);
		}
		// Always output values, without needing connection.
		outputs[MAXB_OUTPUT].channels = channels_b;
		outputs[MINB_OUTPUT].channels = channels_b;

		// lights
		over = false;
		under = false;
		for (int c = 0; c < channels_b; c++) {
			if (highest_b[c] > threshold)
				over = true;
			if (lowest_b[c] < -threshold)
				under = true;
		}
		lights[MAXB_LIGHT].setBrightness(over);
		lights[MINB_LIGHT].setBrightness(under);

	}
};

struct PeakWidget : ModuleWidget {
	PeakWidget(Peak* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Peak.svg")));

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 18.50)), module, Peak::A_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 30.50)), module, Peak::MAXA_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 41.50)), module, Peak::MINA_OUTPUT));

		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.00, 25.25)), module, Peak::MAXA_LIGHT));
		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.00, 36.25)), module, Peak::MINA_LIGHT));

		addParam(createParam<TL1105>(mm2px(Vec(5.08-2.709, 53.00-2.709)), module, Peak::RESETA_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 67.00)), module, Peak::B_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 79.00)), module, Peak::MAXB_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 90.00)), module, Peak::MINB_OUTPUT));

		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.00, 73.75)), module, Peak::MAXB_LIGHT));
		addChild(createLightCentered<PetiteLight<RedLight>>(mm2px(Vec(8.00, 84.75)), module, Peak::MINB_LIGHT));

		addParam(createParam<TL1105>(mm2px(Vec(5.08-2.709, 101.50-2.709)), module, Peak::RESETB_PARAM));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 115.50)), module, Peak::THRESHOLD_PARAM));
	}
};


Model* modelPeak = createModel<Peak, PeakWidget>("Peak");
