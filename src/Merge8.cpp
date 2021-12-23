#include "plugin.hpp"

struct Merge8 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(INPUTS_A, 8),
		NUM_INPUTS
	};
	enum OutputIds {
		POLY_OUT_A,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	int channels[2];

	Merge8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(INPUTS_A, "Channel 1");
		configInput(INPUTS_A + 1, "Channel 2");
		configInput(INPUTS_A + 2, "Channel 3");
		configInput(INPUTS_A + 3, "Channel 4");
		configInput(INPUTS_A + 4, "Channel 5");
		configInput(INPUTS_A + 5, "Channel 6");
		configInput(INPUTS_A + 6, "Channel 7");
		configInput(INPUTS_A + 7, "Channel 8");
		configOutput(POLY_OUT_A, "Polyphonic");
		onReset();
	}

	void onReset() override {
		channels[0] = -1;
		channels[1] = -1;
	}

	void process(const ProcessArgs& args) override {
		int lastChannelA = -1;
		for (int c = 0; c < 8; c++) {
			float v = 0.f;
			
			if (inputs[INPUTS_A + c].isConnected()) {
				lastChannelA = c;
				v = inputs[INPUTS_A + c].getVoltage();
			}
			outputs[POLY_OUT_A].setVoltage(v, c);
		}
		outputs[POLY_OUT_A].channels = (channels[0] >= 0) ? channels[0] : (lastChannelA + 1);
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

struct Merge8ChannelItem : MenuItem {
	Merge8* module;
	int channels;
	int index;
	void onAction(const event::Action& e) override {
		module->channels[index] = channels;
	}
};


struct Merge8ChannelsItem : MenuItem {
	Merge8* module;
	int index;

	Menu* createChildMenu() override {
		Menu* menu = new Menu;
		for (int channels = -1; channels <= 8; channels++) {
			if(channels == 0) continue;
			Merge8ChannelItem* item = new Merge8ChannelItem;
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

struct Merge8Widget : ModuleWidget {
	Merge8Widget(Merge8* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Merge8.svg")));

		{
			for(int i = 0; i < 8; i++) {
				addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 19.50 + 11.25 * i)), module, Merge8::INPUTS_A + i));
			}
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 114.50)) , module, Merge8::POLY_OUT_A));
		}
		
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}

	void appendContextMenu(Menu* menu) override {
		Merge8* module = dynamic_cast<Merge8*>(this->module);

		menu->addChild(new MenuEntry);

		{
			Merge8ChannelsItem* channelsItem = new Merge8ChannelsItem;
			channelsItem->index = 0;
			channelsItem->text = "Channels Merge";
			channelsItem->rightText = RIGHT_ARROW;
			channelsItem->module = module;
			menu->addChild(channelsItem);
		}
	}
};


Model* modelMerge8 = createModel<Merge8, Merge8Widget>("Merge8");
