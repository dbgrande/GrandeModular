#include "plugin.hpp"

struct MergeSplit4 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(INPUTS_A,4),
		POLY_IN_B,
		NUM_INPUTS
	};
	enum OutputIds {
		POLY_OUT_A,
		ENUMS(OUTPUTS_B,4),
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	int channels[2];

	MergeSplit4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(INPUTS_A, "Channel 1");
		configInput(INPUTS_A + 1, "Channel 2");
		configInput(INPUTS_A + 2, "Channel 3");
		configInput(INPUTS_A + 3, "Channel 4");
		configOutput(POLY_OUT_A, "Poly merge");
		configInput(POLY_IN_B, "Poly split");
		configOutput(OUTPUTS_B, "Channel 1");
		configOutput(OUTPUTS_B + 1, "Channel 2");
		configOutput(OUTPUTS_B + 2, "Channel 3");
		configOutput(OUTPUTS_B + 3, "Channel 4");
		onReset();
	}

	void onReset() override {
		channels[0] = -1;
		channels[1] = -1;
	}

	void process(const ProcessArgs& args) override {
		int lastChannelA = -1;
		for (int c = 0; c < 4; c++) {
			float v = 0.f;
			
			if (inputs[INPUTS_A + c].isConnected()) {
				lastChannelA = c;
				v = inputs[INPUTS_A + c].getVoltage();
			}
			outputs[POLY_OUT_A].setVoltage(v, c);
		}
		outputs[POLY_OUT_A].channels = (channels[0] >= 0) ? channels[0] : (lastChannelA + 1);

		for (int c = 0; c < 4; c++) {
			float v = inputs[POLY_IN_B].getVoltage(c);
			outputs[OUTPUTS_B + c].setVoltage(v);
		}
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "channels_merge", json_integer(channels[0]));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* channelsAJ = json_object_get(rootJ, "channels_merge");
		if (channelsAJ)
			channels[0] = json_integer_value(channelsAJ);
	}
};

struct MergeSplit4ChannelItem : MenuItem {
	MergeSplit4* module;
	int channels;
	int index;
	void onAction(const event::Action& e) override {
		module->channels[index] = channels;
	}
};


struct MergeSplit4ChannelsItem : MenuItem {
	MergeSplit4* module;
	int index;

	Menu* createChildMenu() override {
		Menu* menu = new Menu;
		for (int channels = -1; channels <= 4; channels++) {
			if(channels == 0) continue;
			MergeSplit4ChannelItem* item = new MergeSplit4ChannelItem;
			if (channels < 0)
				item->text = "Automatic";
			else
				item->text = string::f("%d", channels);
			item->rightText = CHECKMARK(module->channels[index] == channels);
			item->module = module;
			item->channels = channels;
			item->index = index;
			menu->addChild(item);
		}
		return menu;
	}
};

struct MergeSplit4Widget : ModuleWidget {
	MergeSplit4Widget(MergeSplit4* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/MergeSplit4.svg"),
			asset::plugin(pluginInstance, "res/MergeSplit4-dark.svg")
		));

		{
			for(int i = 0; i < 4; i++) {
				addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 17.00 + 10.00 * i)), module, MergeSplit4::INPUTS_A + i));
			}
			addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 59.25)) , module, MergeSplit4::POLY_OUT_A));

			addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 74.25)), module, MergeSplit4::POLY_IN_B));
			for(int i = 0; i < 4; i++) {
				addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 85.00 + 10.00 * i)), module, MergeSplit4::OUTPUTS_B + i));
			}
		}
		
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}

	void appendContextMenu(Menu* menu) override {
		MergeSplit4* module = dynamic_cast<MergeSplit4*>(this->module);

		menu->addChild(new MenuEntry);

		{
			MergeSplit4ChannelsItem* channelsItem = new MergeSplit4ChannelsItem;
			channelsItem->index = 0;
			channelsItem->text = "Channels Merge";
			channelsItem->rightText = RIGHT_ARROW;
			channelsItem->module = module;
			menu->addChild(channelsItem);
		}
	}
};


Model* modelMergeSplit4 = createModel<MergeSplit4, MergeSplit4Widget>("MergeSplit4");
