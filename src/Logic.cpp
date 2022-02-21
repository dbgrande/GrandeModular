#include "plugin.hpp"
#include <cfloat>

struct Logic : Module {
	enum ParamIds {
		XOR_MODE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		AND_A_INPUT,
		AND_B_INPUT,
		OR_A_INPUT,
		OR_B_INPUT,
		XOR_A_INPUT,
		XOR_B_INPUT,
		NOT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		AND_OUTPUT,
		OR_OUTPUT,
		XOR_OUTPUT,
		NOT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		XOR_MODE_LIGHT,
		NUM_LIGHTS
	};

	Logic() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(AND_A_INPUT, "And A/Across");
		configInput(AND_B_INPUT, "And B/Not");
		configOutput(AND_OUTPUT, "And");
		configInput(OR_A_INPUT, "Or A/Across");
		configInput(OR_B_INPUT, "Or B/Not");
		configOutput(OR_OUTPUT, "Or");
		configButton(XOR_MODE_PARAM, "Xor one-hot mode");
		configInput(XOR_A_INPUT, "Xor A/Across");
		configInput(XOR_B_INPUT, "Xor B/Not");
		configOutput(XOR_OUTPUT, "Xor");
		configInput(NOT_INPUT, "Not");
		configOutput(NOT_OUTPUT, "Not");
		onReset();
	}

	int last_mode = -1;
	int mode = -1;
	bool modestate[1];
	const float threshold = 1.f;

	void process(const ProcessArgs &args) override {
		// Read button
		mode = clamp((int)(params[XOR_MODE_PARAM].getValue()), 0, 1);
		if (mode == 1) {
			if (last_mode == 0) {
				modestate[0] = !modestate[0];
				last_mode = 1;
			}
		}
		else
			last_mode = 0;
		lights[XOR_MODE_LIGHT].setBrightness(modestate[0]);

		// AND Gate
		bool acon1 = inputs[AND_A_INPUT].isConnected();
		bool acon2 = inputs[AND_B_INPUT].isConnected();
		if (acon1 && acon2) {  // 2-input AND
			int achan1 = inputs[AND_A_INPUT].getChannels();
			int achan2 = inputs[AND_B_INPUT].getChannels();
			if (achan1 == 1) {  // Single A input channel ANDed with all B input channels
				bool v1 = inputs[AND_A_INPUT].getVoltage() > threshold;
				for (int c = 0; c < achan2; c++) {
					bool out = v1 && (inputs[AND_B_INPUT].getVoltage(c) > threshold);
					outputs[AND_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[AND_OUTPUT].setChannels(achan2);
			}
			else if (achan2 == 1) {  // Single B input channel ANDed with all A input channels
				bool v2 = inputs[AND_B_INPUT].getVoltage() > threshold;
				for (int c = 0; c < achan1; c++) {
					bool out = (inputs[AND_A_INPUT].getVoltage(c) > threshold) && v2;
					outputs[AND_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[AND_OUTPUT].setChannels(achan1);
			}
			else {  // Both poly -- AND corresponding channels up to minimum of both inputs
				int achans = std::min(achan1, achan2);
				for (int c = 0; c < achans; c++) {
					bool out = (inputs[AND_A_INPUT].getVoltage(c) > threshold) &&
						(inputs[AND_B_INPUT].getVoltage(c) > threshold);
					outputs[AND_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[AND_OUTPUT].setChannels(achans);
			}
		}
		else if (acon1) {  // Only A -- AND across all the A input channels, mono output
			int achans = inputs[AND_A_INPUT].getChannels();
			bool results = true;
			for (int c = 0; c < achans; c++) {
				results = results && inputs[AND_A_INPUT].getVoltage(c) > threshold;
			}
			outputs[AND_OUTPUT].setVoltage((results ? 10.f : 0.f));
			outputs[AND_OUTPUT].setChannels(1);
		}
		else if (acon2) {  // Only B -- invert all the B input channels
			int achans = inputs[AND_B_INPUT].getChannels();
			for (int c = 0; c < achans; c++) {
				bool out = inputs[AND_B_INPUT].getVoltage(c) > threshold;
				outputs[AND_OUTPUT].setVoltage((out ? 0.f : 10.f), c);
			}
			outputs[AND_OUTPUT].setChannels(achans);
		}
		else {  // no inputs connected
			outputs[AND_OUTPUT].setVoltage(0);
			outputs[AND_OUTPUT].setChannels(0);
		}

		// OR Gate
		bool ocon1 = inputs[OR_A_INPUT].isConnected();
		bool ocon2 = inputs[OR_B_INPUT].isConnected();
		if (ocon1 && ocon2) {  // 2-input OR
			int ochan1 = inputs[OR_A_INPUT].getChannels();
			int ochan2 = inputs[OR_B_INPUT].getChannels();
			if (ochan1 == 1) {  // Single A input channel ORed with all B input channels
				bool v1 = inputs[OR_A_INPUT].getVoltage() > threshold;
				for (int c = 0; c < ochan2; c++) {
					bool out = v1 || (inputs[OR_B_INPUT].getVoltage(c) > threshold);
					outputs[OR_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[OR_OUTPUT].setChannels(ochan2);
			}
			else if (ochan2 == 1) {  // Single B input channel ORed with all A input channels
				bool v2 = inputs[OR_B_INPUT].getVoltage() > threshold;
				for (int c = 0; c < ochan1; c++) {
					bool out = (inputs[OR_A_INPUT].getVoltage(c) > threshold) || v2;
					outputs[OR_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[OR_OUTPUT].setChannels(ochan1);
			}
			else {  // Both poly -- OR corresponding channels up to minimum of both inputs
				int ochans = std::min(ochan1, ochan2);
				for (int c = 0; c < ochans; c++) {
					bool out = (inputs[OR_A_INPUT].getVoltage(c) > threshold) ||
						(inputs[OR_B_INPUT].getVoltage(c) > threshold);
					outputs[OR_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[OR_OUTPUT].setChannels(ochans);
			}
		}
		else if (ocon1) {  // Only A -- OR across all the A input channels, mono output
			int ochans = inputs[OR_A_INPUT].getChannels();
			bool results = false;
			for (int c = 0; c < ochans; c++) {
				results = results || inputs[OR_A_INPUT].getVoltage(c) > threshold;
			}
			outputs[OR_OUTPUT].setVoltage((results ? 10.f : 0.f));
			outputs[OR_OUTPUT].setChannels(1);
		}
		else if (ocon2) {  // Only B -- invert all the B input channels
			int ochans = inputs[OR_B_INPUT].getChannels();
			for (int c = 0; c < ochans; c++) {
				bool out = inputs[OR_B_INPUT].getVoltage(c) > threshold;
				outputs[OR_OUTPUT].setVoltage((out ? 0.f : 10.f), c);
			}
			outputs[OR_OUTPUT].setChannels(ochans);
		}
		else {  // no inputs connected
			outputs[OR_OUTPUT].setVoltage(0);
			outputs[OR_OUTPUT].setChannels(0);
		}

		// XOR Gate
		bool xcon1 = inputs[XOR_A_INPUT].isConnected();
		bool xcon2 = inputs[XOR_B_INPUT].isConnected();
		if (xcon1 && xcon2) {  // 2-input XOR
			int xchan1 = inputs[XOR_A_INPUT].getChannels();
			int xchan2 = inputs[XOR_B_INPUT].getChannels();
			if (xchan1 == 1) {  // Single A input channel XORed with all B input channels
				bool v1 = inputs[XOR_A_INPUT].getVoltage() > threshold;
				for (int c = 0; c < xchan2; c++) {
					bool out = v1 ^ (inputs[XOR_B_INPUT].getVoltage(c) > threshold);
					outputs[XOR_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[XOR_OUTPUT].setChannels(xchan2);
			}
			else if (xchan2 == 1) {  // Single B input channel XORed with all A input channels
				bool v2 = inputs[XOR_B_INPUT].getVoltage() > threshold;
				for (int c = 0; c < xchan1; c++) {
					bool out = (inputs[XOR_A_INPUT].getVoltage(c) > threshold) ^ v2;
					outputs[XOR_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[XOR_OUTPUT].setChannels(xchan1);
			}
			else {  // Both poly -- XOR corresponding channels up to minimum of both inputs
				int xchans = std::min(xchan1, xchan2);
				for (int c = 0; c < xchans; c++) {
					bool out = (inputs[XOR_A_INPUT].getVoltage(c) > threshold) ^
						(inputs[XOR_B_INPUT].getVoltage(c) > threshold);
					outputs[XOR_OUTPUT].setVoltage((out ? 10.f : 0.f), c);
				}
				outputs[XOR_OUTPUT].setChannels(xchans);
			}
		}
		else if (xcon1) {  // Only A -- XOR across all the A input channels, mono output
			int xchans = inputs[XOR_A_INPUT].getChannels();
			int count = 0;
			for (int c = 0; c < xchans; c++) {
				count += inputs[XOR_A_INPUT].getVoltage(c) > threshold;
			}
			// test for Xor mode
			if (modestate[0] == 0)  // normal mode, odd number of 1s
				outputs[XOR_OUTPUT].setVoltage((count % 2 == 1) ? 10.f : 0.f);
			else  // one-hot mode, only one 1
				outputs[XOR_OUTPUT].setVoltage((count == 1) ? 10.f : 0.f);
			outputs[XOR_OUTPUT].setChannels(1);
		}
		else if (xcon2) {  // Only B -- invert all the B input channels
			int xchans = inputs[XOR_B_INPUT].getChannels();
			for (int c = 0; c < xchans; c++) {
				bool out = inputs[XOR_B_INPUT].getVoltage(c) > threshold;
				outputs[XOR_OUTPUT].setVoltage((out ? 0.f : 10.f), c);
			}
			outputs[XOR_OUTPUT].setChannels(xchans);
		}
		else {  // no inputs connected
			outputs[XOR_OUTPUT].setVoltage(0);
			outputs[XOR_OUTPUT].setChannels(0);
		}

		// NOT Gate
		int nchans = inputs[NOT_INPUT].getChannels();
		for (int c = 0; c < nchans; c++) {
			bool out = inputs[NOT_INPUT].getVoltage(c) > threshold;
			outputs[NOT_OUTPUT].setVoltage((out ? 0.f : 10.f), c);
		}
		outputs[NOT_OUTPUT].setChannels(nchans);
	}

	void onReset() override {
		for (int c = 0; c < 1; c++)
			modestate[c] = false;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();

		// modestates
		json_t* modestatesJ = json_array();
		for (int i = 0; i < 1; i++) {
			json_t* modestateJ = json_boolean(modestate[i]);
			json_array_append_new(modestatesJ, modestateJ);
		}
		json_object_set_new(rootJ, "modestates", modestatesJ);

		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// modestates
		json_t* modestatesJ = json_object_get(rootJ, "modestates");
		if (modestatesJ) {
			for (int i = 0; i < 1; i++) {
				json_t* modestateJ = json_array_get(modestatesJ, i);
				if (modestateJ)
					modestate[i] = json_boolean_value(modestateJ);
			}
		}
	}
};


struct LogicWidget : ModuleWidget {
	LogicWidget(Logic* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Logic.svg")));

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 17.9)), module, Logic::AND_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 26.1)), module, Logic::AND_B_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 34.5)), module, Logic::AND_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 47.7)), module, Logic::OR_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 55.9)), module, Logic::OR_B_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 64.3)), module, Logic::OR_OUTPUT));

		addChild(createLightCentered<SmallLightFlat<BlueLight>>(mm2px(Vec(7.68, 72.50)), module, Logic::XOR_MODE_LIGHT));
		addParam(createParam<SmallLEDButton>(mm2px(Vec(7.68-1.5, 72.50-1.5)), module, Logic::XOR_MODE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 77.5)), module, Logic::XOR_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 85.7)), module, Logic::XOR_B_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 94.1)), module, Logic::XOR_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 107.3)), module, Logic::NOT_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 115.7)), module, Logic::NOT_OUTPUT));
	}
};


Model* modelLogic = createModel<Logic, LogicWidget>("Logic");
