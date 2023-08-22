#include "plugin.hpp"


struct MicrotonalNotes : Module {
	enum ParamIds {
		ENUMS(PITCH_PARAMS, 8),
		ENUMS(OCTAVE_PARAMS, 8),
		SIZE_PARAM,
		CHANNEL_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(NOTE_OUTPUTS, 8),
		POLY_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(CHANNEL_LIGHTS, 8),
		NUM_LIGHTS
	};

	MicrotonalNotes() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 8; i++) {
			configParam(PITCH_PARAMS + i, 0.f, 33.f, 0.f, "Note" + std::to_string(i+1), "");
			configParam(OCTAVE_PARAMS + i, -4, 4, 0, "Octave" + std::to_string(i+1), "");
			configOutput(NOTE_OUTPUTS + i, "Note " + std::to_string(i+1));
		}
		configParam(SIZE_PARAM, 1, 34, 12, "Notes per Octave", "");
		configParam(CHANNEL_PARAM, 1, 8, 8, "Channels out", "");
		getParamQuantity(CHANNEL_PARAM)->randomizeEnabled = false;
		configOutput(POLY_OUTPUT, "Poly");
	}

	void process(const ProcessArgs &args) override {
		int pitch[8];
		int octave[8];
		for (int i = 0; i < 8; i++) {
			pitch[i] = clamp((int)(params[PITCH_PARAMS + i].getValue()), 0, 33);
			octave[i] = clamp((int)(params[OCTAVE_PARAMS + i].getValue()), -4, 4);
		}
		int size = clamp((int)(params[SIZE_PARAM].getValue()), 1, 34);
		int channels = clamp((int)(params[CHANNEL_PARAM].getValue()), 1, 8);

		for (int i = 0; i < 8; i++) {
			if (pitch[i] >= size)
				pitch[i] = size - 1;
			float vout = octave[i] + ((float) pitch[i] / size);
			outputs[NOTE_OUTPUTS + i].setVoltage(vout);
			if (i < channels) {
				outputs[POLY_OUTPUT].setVoltage(vout, i);
				lights[CHANNEL_LIGHTS + i].setBrightness(1);
			}
			else
				lights[CHANNEL_LIGHTS +i].setBrightness(0);
		}
		outputs[POLY_OUTPUT].setChannels(channels);
	}
};

struct MicrotonalNotesWidget : ModuleWidget {
	MicrotonalNotesWidget(MicrotonalNotes* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/MicrotonalNotes.svg"),
			asset::plugin(pluginInstance, "res/MicrotonalNotes-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(0, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		for (int i = 0; i < 8; i++) {
			addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(7.62, 19.50 + i*11.25)), module, MicrotonalNotes::OCTAVE_PARAMS + i));
			addParam(createParamCentered<RoundSmallRotarySwitch>(mm2px(Vec(18.32, 19.50 + i*11.25)), module, MicrotonalNotes::PITCH_PARAMS + i));
			addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(29.87, 19.50 + i*11.25)), module, MicrotonalNotes::NOTE_OUTPUTS + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(35.85, 19.50 + i*11.25)), module, MicrotonalNotes::CHANNEL_LIGHTS + i));
		}
		addParam(createParamCentered<RoundBlackRotarySwitch>(mm2px(Vec(8.15, 114.50)), module, MicrotonalNotes::SIZE_PARAM));
		addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(21.50, 114.50)), module, MicrotonalNotes::CHANNEL_PARAM));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(32.02, 114.50)), module, MicrotonalNotes::POLY_OUTPUT));
	}
};


Model* modelMicrotonalNotes = createModel<MicrotonalNotes, MicrotonalNotesWidget>("MicrotonalNotes");
