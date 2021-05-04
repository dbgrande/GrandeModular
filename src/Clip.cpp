#include "plugin.hpp"


struct Clip : Module {
	enum ParamIds {
		VCLIP_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VCLIP_INPUT,
		A_INPUT,
		B_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		A_OUTPUT,
		B_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(A_LIGHTS, 16*3),
		ENUMS(B_LIGHTS, 16*3),
		NUM_LIGHTS
	};

	Clip() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(VCLIP_PARAM, 0.f, 10.f, 5.f, "Vclip", "V");
	}

	const float max_voltage = 20.f;  // max allowed clipping voltage
	float vclip[16] = { 5.f };
	int check_lights = 0;

	bool aclip[16] = { false };
	int acount[16] = { 0 };
	int acountlow[16] = { 0 };

	bool bclip[16] = { false };
	int bcount[16] = { 0 };
	int bcountlow[16] = { 0 };

	void process(const ProcessArgs &args) override {
		int light_interval = 10;  // how many audio samples between light updates
		// minimum length of time to keep clip light on (150 ms)
		int clip_time = (int)(0.150f * args.sampleRate / light_interval);
		// minimum length of time to keep clip light off, immediately after clip turns off (75 ms)
		// if less than clip_time, then clips during low time will be displayed right afterwards
		int low_time = (int)(0.075f * args.sampleRate / light_interval);

		float vclip_knob = clamp((params[VCLIP_PARAM].getValue()), 0.f, 10.f);

		int achannels = inputs[A_INPUT].getChannels();
		int bchannels = inputs[B_INPUT].getChannels();
		int max_channels = std::max(achannels, bchannels);

		// vclip voltage
		// nan or infinities on input set max voltage allowed
		int vchannels = inputs[VCLIP_INPUT].getChannels();
		if (vchannels > 0) {  // connected
			int c = 0;
			for ( ; c < vchannels; c++) {
				vclip[c] = std::abs(inputs[VCLIP_INPUT].getVoltage(c));
				if (std::isnan(vclip[c]) || vclip[c] > max_voltage)
					vclip[c] = max_voltage;
			}
			for ( ; c < max_channels; c++)
				vclip[c] = vclip[vchannels - 1];
		}
		else {  // not connected, set to knob value (10V max)
			for (int c = 0; c < max_channels; c++)
				vclip[c] = vclip_knob;
		}

		// A input
		// nan converted to 0, infinities clipped
		for (int c = 0; c < achannels; c++) {
			float vin = inputs[A_INPUT].getVoltage(c);
			float vout;
			if (std::isnan(vin)) {
				vout = 0.f;
				acount[c] = clip_time;
			}
			else if (vin > vclip[c]) {
				vout = vclip[c];
				acount[c] = clip_time;
			}
			else if (vin < -vclip[c]) {
				vout = -vclip[c];
				acount[c] = clip_time;
			}
			else
				vout = vin;
			outputs[A_OUTPUT].setVoltage(vout, c);
		}
		outputs[A_OUTPUT].setChannels(achannels);

		// B input
		// nan converted to 0, infinities clipped
		for (int c = 0; c < bchannels; c++) {
			float vin = inputs[B_INPUT].getVoltage(c);
			float vout;
			if (std::isnan(vin)) {
				vout = 0.f;
				bcount[c] = clip_time;
			}
			else if (vin > vclip[c]) {
				vout = vclip[c];
				bcount[c] = clip_time;
			}
			else if (vin < -vclip[c]) {
				vout = -vclip[c];
				bcount[c] = clip_time;
			}
			else
				vout = vin;
			outputs[B_OUTPUT].setVoltage(vout, c);
		}
		outputs[B_OUTPUT].setChannels(bchannels);

		// process lights
		if (check_lights == 0) {
			check_lights = light_interval;

			// A lights
			for (int i = 0; i < 16; i++) {
				if (i < achannels) {  // active channel a
					if (acountlow[i] > 0) {  // force blue light
						lights[A_LIGHTS + 3*i].setBrightness(false);
						lights[A_LIGHTS + 3*i + 2].setBrightness(true);
						if (acountlow[i] == 1) {  // force low ending
							if (acount[i] > 0)  // clipped while low so set to full duration
								acount[i] = clip_time;
						}
						else {
							acount[i] -= 1;
						}
						acountlow[i] -= 1;
					}
					else {
						if (acount[i] > 0) {  // force red light
							lights[A_LIGHTS + 3*i].setBrightness(true);
							lights[A_LIGHTS + 3*i + 2].setBrightness(false);
							if (acount[i] == 1)  // force high ending, start force low instead
								acountlow[i] = low_time;  // set force low time
							acount[i] -= 1;
						}
						else {  // idle mode, neither counts active
							lights[A_LIGHTS + 3*i].setBrightness(false);
							lights[A_LIGHTS + 3*i + 2].setBrightness(true);
						}
					}
				}
				else {  // inactive channel a
					lights[A_LIGHTS + 3*i].setBrightness(false);
					lights[A_LIGHTS + 3*i + 2].setBrightness(false);
					acount[i] = 0;
					acountlow[i] = 0;
				}

				// B lights
				if (i < bchannels) {  // active channel b
					if (bcountlow[i] > 0) {  // force blue light
						lights[B_LIGHTS + 3*i].setBrightness(false);
						lights[B_LIGHTS + 3*i + 2].setBrightness(true);
						if (bcountlow[i] == 1) {  // force low ending
							if (bcount[i] > 0)  // clipped while low so set to full duration
								bcount[i] = clip_time;
						}
						else {
							bcount[i] -= 1;
						}
						bcountlow[i] -= 1;
					}
					else {
						if (bcount[i] > 0) {  // force red light
							lights[B_LIGHTS + 3*i].setBrightness(true);
							lights[B_LIGHTS + 3*i + 2].setBrightness(false);
							if (bcount[i] == 1)  // force high ending, start force low instead
								bcountlow[i] = low_time;  // set force low time
							bcount[i] -= 1;
						}
						else {  // idle mode, neither counts active
							lights[B_LIGHTS + 3*i].setBrightness(false);
							lights[B_LIGHTS + 3*i + 2].setBrightness(true);
						}
					}
				}
				else {  // inactive channel b
					lights[B_LIGHTS + 3*i].setBrightness(false);
					lights[B_LIGHTS + 3*i + 2].setBrightness(false);
					bcount[i] = 0;
					bcountlow[i] = 0;
				}
			}
		}
		else {
			check_lights -= 1;
		}
	}
};

struct ClipWidget : ModuleWidget {
	ClipWidget(Clip* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Clip.svg")));

		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 65.00)), module, Clip::A_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 74.50)), module, Clip::A_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 86.25)), module, Clip::B_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 95.75)), module, Clip::B_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 107.50)), module, Clip::VCLIP_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.08, 115.75)), module, Clip::VCLIP_PARAM));

		for (int i = 0; i < 16; i++) {
			addChild(createLightCentered<PetiteLight<RedGreenBlueLight>>(mm2px(Vec(3.40, 16.75 + 2.42*i)), module, Clip::A_LIGHTS + i*3));
			addChild(createLightCentered<PetiteLight<RedGreenBlueLight>>(mm2px(Vec(6.68, 16.75 + 2.42*i)), module, Clip::B_LIGHTS + i*3));
		}
	}
};

Model* modelClip = createModel<Clip, ClipWidget>("Clip");
