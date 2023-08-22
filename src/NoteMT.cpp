#include "plugin.hpp"


struct NoteMT : Module {
	enum ParamIds {
		PITCH_PARAM,
		OCTAVE_PARAM,
		SIZE_PARAM,
		INT_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VOCT_INPUT,
		THRESH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		VOCT_OUTPUT,
		OCTAVE_OUTPUT,
		NOTE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		INT_LIGHT,
		NUM_LIGHTS
	};

	int last_int_button = 0;
	int last_size = 0;
	bool round_state;

	NoteMT() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PITCH_PARAM, 0.f, 33.f, 0.f, "Note", "");
		configParam(OCTAVE_PARAM, -5, 5, 0, "Octave", "");
		configParam(SIZE_PARAM, 1, 34, 12, "Notes per Octave", "");
		configButton(INT_PARAM, "Round notes");
		configInput(VOCT_INPUT, "Pitch");
		configOutput(VOCT_OUTPUT, "Pitch");
		configOutput(OCTAVE_OUTPUT, "Octave");
		configOutput(NOTE_OUTPUT, "Note");
	}

	void process(const ProcessArgs &args) override {
			int pitch = clamp((int)(params[PITCH_PARAM].getValue()), 0, 33);
			int octave = clamp((int)(params[OCTAVE_PARAM].getValue()), -5, 5);
			int size = clamp((int)(params[SIZE_PARAM].getValue()), 1, 34);
			int int_button = clamp((int)(params[INT_PARAM].getValue()), 0, 1);

			// Round notes to integers
			// State not maintained through power cycles
			// Disabled if Notes/Oct changed
			if (int_button == 1) {
				if (last_int_button == 0) { // int button just pressed
					round_state = !round_state;  // toggle
					last_int_button = 1;
				}
			}
			else
				last_int_button = 0;

			// Disable rounding state if size changes
			if (size != last_size) {
				round_state = false;
				last_size = size;
			}
			lights[INT_LIGHT].setBrightness(round_state);

		if (pitch >= size)
			pitch = size - 1;
		float vout = octave + ((float) pitch / size);
		outputs[VOCT_OUTPUT].setVoltage(vout);

		float vin[16];
		int channels = inputs[VOCT_INPUT].getChannels();
		if (channels > 16)  // deal with borked poly merge
			channels = 16;
		int ochannels;
		if (channels == 0) { // nothing plugged in, use internal source
			vin[0] = vout;
			ochannels = 1;
		}
		else {
			for (int c = 0; c < channels; c++)
				vin[c] = inputs[VOCT_INPUT].getVoltage(c);
			ochannels = channels;
		}

		for (int c = 0; c < ochannels; c++) {
			float intPart;
			float fracPart = modff(vin[c], &intPart);
			if (intPart < 0.f || fracPart < 0.f) { // round to -∞
				if (abs(fracPart) < 1e-7)
					fracPart = 0.f;
				else {
					fracPart += 1.f;
					intPart -= 1.f;
				}
			}
			float scaled_note = fracPart * size;
			if (round_state)  // need to round note before correcting for octave
				scaled_note = roundf(scaled_note);
			if (scaled_note > size - 0.5f) {
				scaled_note -= size;
				intPart += 1.f;
			}
			outputs[OCTAVE_OUTPUT].setVoltage(intPart, c);
			outputs[NOTE_OUTPUT].setVoltage(scaled_note, c);
		}
		outputs[OCTAVE_OUTPUT].setChannels(ochannels);
		outputs[NOTE_OUTPUT].setChannels(ochannels);
	}
};

struct NoteMTWidget : ModuleWidget {
	NoteMTWidget(NoteMT* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/NoteMT.svg"),
			asset::plugin(pluginInstance, "res/NoteMT-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(0, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackRotarySwitch>(mm2px(Vec(12.70, 23.00)), module, NoteMT::OCTAVE_PARAM));

		addParam(createParamCentered<RoundLargeRotarySwitch>(mm2px(Vec(12.70, 42.00)), module, NoteMT::PITCH_PARAM));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(12.70, 61.00)), module, NoteMT::VOCT_OUTPUT));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(6.35, 77.00)), module, NoteMT::VOCT_INPUT));
		addParam(createParam<TL1105>(mm2px(Vec(19.05-2.709, 77.90-2.709)), module, NoteMT::INT_PARAM));
		addChild(createLightCentered<MediumLightFlat<BlueLight>>(mm2px(Vec(18.95, 77.80)), module, NoteMT::INT_LIGHT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(6.35, 93.00)), module, NoteMT::OCTAVE_OUTPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(19.05, 93.00)), module, NoteMT::NOTE_OUTPUT));

		addParam(createParamCentered<RoundLargeRotarySwitch>(mm2px(Vec(12.70, 111.00)), module, NoteMT::SIZE_PARAM));
	}
};


Model* modelNoteMT = createModel<NoteMT, NoteMTWidget>("NoteMT");
