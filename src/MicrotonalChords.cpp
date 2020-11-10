#include "plugin.hpp"


struct MicrotonalChords : Module {
	enum ParamIds {
		ENUMS(PITCHA_PARAMS, 4),
		ENUMS(PITCHB_PARAMS, 4),
		ENUMS(PITCHC_PARAMS, 4),
		ENUMS(PITCHD_PARAMS, 4),
		ENUMS(OCTAVEA_PARAMS, 4),
		ENUMS(OCTAVEB_PARAMS, 4),
		ENUMS(OCTAVEC_PARAMS, 4),
		ENUMS(OCTAVED_PARAMS, 4),
		SIZE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		SELECT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		POLY_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(SELECT_LIGHTS, 4),
		NUM_LIGHTS
	};

	MicrotonalChords() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 4; i++) {
			configParam(PITCHA_PARAMS + i, 0.f, 33.f, 0.f, "NoteA" + std::to_string(i+1), "");
			configParam(PITCHB_PARAMS + i, 0.f, 33.f, 0.f, "NoteB" + std::to_string(i+1), "");
			configParam(PITCHC_PARAMS + i, 0.f, 33.f, 0.f, "NoteC" + std::to_string(i+1), "");
			configParam(PITCHD_PARAMS + i, 0.f, 33.f, 0.f, "NoteD" + std::to_string(i+1), "");
			configParam(OCTAVEA_PARAMS + i, -4, 4, 0, "OctaveA" + std::to_string(i+1), "");
			configParam(OCTAVEB_PARAMS + i, -4, 4, 0, "OctaveB" + std::to_string(i+1), "");
			configParam(OCTAVEC_PARAMS + i, -4, 4, 0, "OctaveC" + std::to_string(i+1), "");
			configParam(OCTAVED_PARAMS + i, -4, 4, 0, "OctaveD" + std::to_string(i+1), "");
		}
		configParam(SIZE_PARAM, 1, 34, 12, "Notes per Octave", "");
	}

	void process(const ProcessArgs &args) override {
		int pitch_a[4], pitch_b[4], pitch_c[4], pitch_d[4];
		int octave_a[4], octave_b[4], octave_c[4], octave_d[4];
		for (int i = 0; i < 4; i++) {
			pitch_a[i] = clamp((int)(params[PITCHA_PARAMS + i].getValue()), 0, 33);
			pitch_b[i] = clamp((int)(params[PITCHB_PARAMS + i].getValue()), 0, 33);
			pitch_c[i] = clamp((int)(params[PITCHC_PARAMS + i].getValue()), 0, 33);
			pitch_d[i] = clamp((int)(params[PITCHD_PARAMS + i].getValue()), 0, 33);
			octave_a[i] = clamp((int)(params[OCTAVEA_PARAMS + i].getValue()), -4, 4);
			octave_b[i] = clamp((int)(params[OCTAVEB_PARAMS + i].getValue()), -4, 4);
			octave_c[i] = clamp((int)(params[OCTAVEC_PARAMS + i].getValue()), -4, 4);
			octave_d[i] = clamp((int)(params[OCTAVED_PARAMS + i].getValue()), -4, 4);
		}
		int size = clamp((int)(params[SIZE_PARAM].getValue()), 1, 34);

		// 0V == A, 1V == B, 2V == C, 3V == D
		int select = 0;
		int channels = inputs[SELECT_INPUT].getChannels();
		if (channels > 0)
			select = clamp((int) roundf(inputs[SELECT_INPUT].getVoltage(0)), 0, 3);
		for (int i = 0; i < 4; i++)
			lights[SELECT_LIGHTS + i].setBrightness(i == select);

		for (int i = 0; i < 4; i++) {
			int pitch, octave;
			if (select == 3) {
				pitch = (pitch_d[i] < size) ? pitch_d[i] : size - 1;
				octave = octave_d[i];
			} else if (select == 2) {
				pitch = (pitch_c[i] < size) ? pitch_c[i] : size - 1;
				octave = octave_c[i];
			} else if (select == 1) {
				pitch = (pitch_b[i] < size) ? pitch_b[i] : size - 1;
				octave = octave_b[i];
			} else {  // select == 0
				pitch = (pitch_a[i] < size) ? pitch_a[i] : size - 1;
				octave = octave_a[i];
			}
			float vout = octave + ((float) pitch / size);
			outputs[POLY_OUTPUT].setVoltage(vout, i);
		}
		outputs[POLY_OUTPUT].setChannels(4);
	}
};

struct MicrotonalChordsWidget : ModuleWidget {
	MicrotonalChordsWidget(MicrotonalChords* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MicrotonalChords.svg")));

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		for (int i = 0; i < 4; i++) {
			addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(8.25, 51.00 - i*11.25)), module, MicrotonalChords::OCTAVEA_PARAMS + i));
			addParam(createParamCentered<RoundSmallRotarySwitch>(mm2px(Vec(19.00, 51.00 - i*11.25)), module, MicrotonalChords::PITCHA_PARAMS + i));

			addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(34.69, 51.00 - i*11.25)), module, MicrotonalChords::OCTAVEB_PARAMS + i));
			addParam(createParamCentered<RoundSmallRotarySwitch>(mm2px(Vec(45.44, 51.00 - i*11.25)), module, MicrotonalChords::PITCHB_PARAMS + i));

			addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(8.25, 98.75 - i*11.25)), module, MicrotonalChords::OCTAVEC_PARAMS + i));
			addParam(createParamCentered<RoundSmallRotarySwitch>(mm2px(Vec(19.00, 98.75 - i*11.25)), module, MicrotonalChords::PITCHC_PARAMS + i));

			addParam(createParamCentered<RoundTinyRotarySwitch>(mm2px(Vec(34.69, 98.75 - i*11.25)), module, MicrotonalChords::OCTAVED_PARAMS + i));
			addParam(createParamCentered<RoundSmallRotarySwitch>(mm2px(Vec(45.44, 98.75 - i*11.25)), module, MicrotonalChords::PITCHD_PARAMS + i));
		}
		addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(6.0, 13.25)), module, MicrotonalChords::SELECT_LIGHTS + 0));
		addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(32.44, 13.25)), module, MicrotonalChords::SELECT_LIGHTS + 1));
		addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(6.0, 61.00)), module, MicrotonalChords::SELECT_LIGHTS + 2));
		addChild(createLightCentered<TinyLight<BlueLight>>(mm2px(Vec(32.44, 61.00)), module, MicrotonalChords::SELECT_LIGHTS + 3));

		addParam(createParamCentered<RoundBlackRotarySwitch>(mm2px(Vec(11.00, 115.25)), module, MicrotonalChords::SIZE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.00, 115.25)), module, MicrotonalChords::SELECT_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.13, 115.25)), module, MicrotonalChords::POLY_OUTPUT));
	}
};


Model* modelMicrotonalChords = createModel<MicrotonalChords, MicrotonalChordsWidget>("MicrotonalChords");
