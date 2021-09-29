#include "plugin.hpp"

#define MAX_SCALE 34

struct QuantIntervals : Module {
	enum ParamIds {
		ROUNDING_PARAM,
		EQUI_PARAM,
		SIZE_PARAM,
		ENUMS(INTERVAL_PARAMS, 67),
		TOLERANCE_PARAM,
		CLEAR_ALL_PARAM,
		SHOW_SMALL_PARAM,
		SEL_ENABLED_PARAM,
		SHOW_ALLOWED_PARAM,
		ENUMS(INTERVAL_LIGHT_PARAMS, 67),
		ENUMS(NOTE_LIGHT_PARAMS, 34),
		NUM_PARAMS
	};
	enum InputIds {
		ROOT_INPUT,
		CV_IN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUT_OUTPUT,
		TRIGGER_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(INTERVAL_TOP_LIGHTS, 67),
		ENUMS(INTERVAL_BOT_LIGHTS, 67),
		ENUMS(NOTE_LIGHTS, 34),
		NUM_LIGHTS
	};

	QuantIntervals() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configSwitch(ROUNDING_PARAM, 0.0, 2.0, 1.0, "Rounding", {"Down", "Nearest", "Up"});
		getParamQuantity(ROUNDING_PARAM)->randomizeEnabled = false;
		configSwitch(EQUI_PARAM, 0.0, 1.0, 0.0, "Equi-likely notes", {"Off", "On"});
		getParamQuantity(EQUI_PARAM)->randomizeEnabled = false;
		configParam(SIZE_PARAM, 1, 34, 12, "Notes per Octave", "");
		configParam(INTERVAL_PARAMS, 0.0, 1.0, 1.0, "0.0", "");  // Unison
		configParam(INTERVAL_PARAMS + 1, 0.0, 1.0, 0.0, "48.8", "");
		configParam(INTERVAL_PARAMS + 2, 0.0, 1.0, 0.0, "63.0", "");
		configParam(INTERVAL_PARAMS + 3, 0.0, 1.0, 0.0, "67.9", "");
		configParam(INTERVAL_PARAMS + 4, 0.0, 1.0, 0.0, "80.5", "");
		configParam(INTERVAL_PARAMS + 5, 0.0, 1.0, 0.0, "111.7", "");
		configParam(INTERVAL_PARAMS + 6, 0.0, 1.0, 0.0, "119.4", "");
		configParam(INTERVAL_PARAMS + 7, 0.0, 1.0, 0.0, "128.3", "");
		configParam(INTERVAL_PARAMS + 8, 0.0, 1.0, 0.0, "138.6", "");
		configParam(INTERVAL_PARAMS + 9, 0.0, 1.0, 0.0, "150.6", "");
		configParam(INTERVAL_PARAMS + 10, 0.0, 1.0, 0.0, "165.0", "");
		configParam(INTERVAL_PARAMS + 11, 0.0, 1.0, 0.0, "182.4", "");
		configParam(INTERVAL_PARAMS + 12, 0.0, 1.0, 0.0, "196.2", "");
		configParam(INTERVAL_PARAMS + 13, 0.0, 1.0, 1.0, "203.9", ""); // 9:8
		configParam(INTERVAL_PARAMS + 14, 0.0, 1.0, 0.0, "231.2", "");
		configParam(INTERVAL_PARAMS + 15, 0.0, 1.0, 0.0, "247.7", "");
		configParam(INTERVAL_PARAMS + 16, 0.0, 1.0, 0.0, "266.9", "");
		configParam(INTERVAL_PARAMS + 17, 0.0, 1.0, 0.0, "289.2", "");
		configParam(INTERVAL_PARAMS + 18, 0.0, 1.0, 0.0, "294.1", "");
		configParam(INTERVAL_PARAMS + 19, 0.0, 1.0, 0.0, "315.6", "");
		configParam(INTERVAL_PARAMS + 20, 0.0, 1.0, 0.0, "347.4", "");
		configParam(INTERVAL_PARAMS + 21, 0.0, 1.0, 0.0, "359.5", "");
		configParam(INTERVAL_PARAMS + 22, 0.0, 1.0, 0.0, "369.7", "");
		configParam(INTERVAL_PARAMS + 23, 0.0, 1.0, 1.0, "386.3", ""); // 5:4
		configParam(INTERVAL_PARAMS + 24, 0.0, 1.0, 0.0, "417.5", "");
		configParam(INTERVAL_PARAMS + 25, 0.0, 1.0, 0.0, "427.4", "");
		configParam(INTERVAL_PARAMS + 26, 0.0, 1.0, 0.0, "435.1", "");
		configParam(INTERVAL_PARAMS + 27, 0.0, 1.0, 0.0, "454.2", "");
		configParam(INTERVAL_PARAMS + 28, 0.0, 1.0, 0.0, "470.8", "");
		configParam(INTERVAL_PARAMS + 29, 0.0, 1.0, 1.0, "498.0", "");  // 4:3
		configParam(INTERVAL_PARAMS + 30, 0.0, 1.0, 0.0, "537.0", "");
		configParam(INTERVAL_PARAMS + 31, 0.0, 1.0, 0.0, "551.3", "");
		configParam(INTERVAL_PARAMS + 32, 0.0, 1.0, 0.0, "563.4", "");
		configParam(INTERVAL_PARAMS + 33, 0.0, 1.0, 0.0, "582.5", "");
		configParam(INTERVAL_PARAMS + 34, 0.0, 1.0, 0.0, "617.5", "");
		configParam(INTERVAL_PARAMS + 35, 0.0, 1.0, 0.0, "636.6", "");
		configParam(INTERVAL_PARAMS + 36, 0.0, 1.0, 0.0, "648.7", "");
		configParam(INTERVAL_PARAMS + 37, 0.0, 1.0, 0.0, "663.0", "");
		configParam(INTERVAL_PARAMS + 38, 0.0, 1.0, 1.0, "702.0", ""); // 3:2
		configParam(INTERVAL_PARAMS + 39, 0.0, 1.0, 0.0, "729.2", "");
		configParam(INTERVAL_PARAMS + 40, 0.0, 1.0, 0.0, "745.8", "");
		configParam(INTERVAL_PARAMS + 41, 0.0, 1.0, 0.0, "764.9", "");
		configParam(INTERVAL_PARAMS + 42, 0.0, 1.0, 0.0, "772.6", "");
		configParam(INTERVAL_PARAMS + 43, 0.0, 1.0, 0.0, "782.5", "");
		configParam(INTERVAL_PARAMS + 44, 0.0, 1.0, 0.0, "813.7", "");
		configParam(INTERVAL_PARAMS + 45, 0.0, 1.0, 0.0, "830.3", "");
		configParam(INTERVAL_PARAMS + 46, 0.0, 1.0, 0.0, "840.5", "");
		configParam(INTERVAL_PARAMS + 47, 0.0, 1.0, 0.0, "852.6", "");
		configParam(INTERVAL_PARAMS + 48, 0.0, 1.0, 1.0, "884.4", ""); // 5:3
		configParam(INTERVAL_PARAMS + 49, 0.0, 1.0, 0.0, "905.9", "");
		configParam(INTERVAL_PARAMS + 50, 0.0, 1.0, 0.0, "910.8", "");
		configParam(INTERVAL_PARAMS + 51, 0.0, 1.0, 0.0, "933.1", "");
		configParam(INTERVAL_PARAMS + 52, 0.0, 1.0, 0.0, "952.3", "");
		configParam(INTERVAL_PARAMS + 53, 0.0, 1.0, 0.0, "968.8", "");
		configParam(INTERVAL_PARAMS + 54, 0.0, 1.0, 0.0, "996.1", "");
		configParam(INTERVAL_PARAMS + 55, 0.0, 1.0, 0.0, "1003.8", "");
		configParam(INTERVAL_PARAMS + 56, 0.0, 1.0, 0.0, "1017.6", "");
		configParam(INTERVAL_PARAMS + 57, 0.0, 1.0, 0.0, "1035.0", "");
		configParam(INTERVAL_PARAMS + 58, 0.0, 1.0, 0.0, "1049.4", "");
		configParam(INTERVAL_PARAMS + 59, 0.0, 1.0, 0.0, "1061.4", "");
		configParam(INTERVAL_PARAMS + 60, 0.0, 1.0, 0.0, "1071.7", "");
		configParam(INTERVAL_PARAMS + 61, 0.0, 1.0, 0.0, "1080.6", "");
		configParam(INTERVAL_PARAMS + 62, 0.0, 1.0, 1.0, "1088.3", ""); // 15:8
		configParam(INTERVAL_PARAMS + 63, 0.0, 1.0, 0.0, "1119.5", "");
		configParam(INTERVAL_PARAMS + 64, 0.0, 1.0, 0.0, "1132.1", "");
		configParam(INTERVAL_PARAMS + 65, 0.0, 1.0, 0.0, "1137.0", "");
		configParam(INTERVAL_PARAMS + 66, 0.0, 1.0, 0.0, "1151.2", "");
		configParam(TOLERANCE_PARAM, 0.0, 50.0, 20.0, "Tolerance", "¢");
		getParamQuantity(TOLERANCE_PARAM)->randomizeEnabled = false;
		configButton(CLEAR_ALL_PARAM, "Clear All");
		configButton(SHOW_SMALL_PARAM, "Show Small Valid");
		configButton(SHOW_ALLOWED_PARAM, "Show All Valid");
		configButton(SEL_ENABLED_PARAM, "Clear Invalid");
		for (int i = 0; i < 67; i++)
			configButton(INTERVAL_LIGHT_PARAMS + i, "i" + std::to_string(i));
		for (int i = 0; i < 34; i++)
			configButton(NOTE_LIGHT_PARAMS + i, "n" + std::to_string(i));
		configInput(ROOT_INPUT, "Transpose");
		configInput(CV_IN_INPUT, "Pitch");
		configOutput(CV_OUT_OUTPUT, "Pitch");
		configOutput(TRIGGER_OUTPUT, "Trigger");
		configBypass(CV_IN_INPUT, CV_OUT_OUTPUT);
	}

	dsp::PulseGenerator pulseGenerators[16];

	int param_timer = 0;
	int rounding_mode;
	int equi_likely;
	int equal_temp;
	float step_size;
	float tolerance;
	int scale[MAX_SCALE+1];
	int note_per_oct;
	int lower[MAX_SCALE];
	int upper[MAX_SCALE];
	float transpose[16];
	float cv_out[16];
	float last_cv_out[16] = { 0.f };
	// 13-limit with all octave complements and all ≤ 36
	float interval_lu[68] = { 0.f, 0.0406420f, 0.0524674f, 0.0565835f, 0.0671142f, 0.0931094f, 0.0995357f, 0.1069152f,
		0.1154772f, 0.1255309f, 0.1375035f, 0.1520031f, 0.1634987f, 0.1699250f, 0.1926451f, 0.2064509f,
		0.2223924f, 0.2410081f, 0.2451125f, 0.2630344f, 0.2895066f, 0.2995603f, 0.3081223f, 0.3219281f,
		0.3479233f, 0.3561438f, 0.3625701f, 0.3785116f, 0.3923174f, 0.4150375f, 0.4474590f, 0.4594316f,
		0.4694853f, 0.4854268f, 0.5145732f, 0.5305147f, 0.5405684f, 0.5525410f, 0.5849625f, 0.6076826f,
		0.6214884f, 0.6374299f, 0.6438562f, 0.6520767f, 0.6780719f, 0.6918777f, 0.7004397f, 0.7104934f,
		0.7369656f, 0.7548875f, 0.7589919f, 0.7776076f, 0.7935491f, 0.8073549f, 0.8300750f, 0.8365013f,
		0.8479969f, 0.8624965f, 0.8744691f, 0.8845228f, 0.8930848f, 0.9004643f, 0.9068906f, 0.9328858f,
		0.9434165f, 0.9475326f, 0.9593580f, 1.f };
	int num_intervals = 67;  // not including octave
	int ratio_size[68] = { 1, 3, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2,
		1, 2, 3, 1, 1, 2, 2, 1, 2, 3, 1, 1, 2, 1, 2, 1,
		2, 1, 1, 1, 2, 2, 1, 2, 2, 1, 2, 1, 1, 2, 1, 2,
		1, 2, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
		2, 2, 3, 1 };


	void process(const ProcessArgs &args) override {
		if (param_timer == 0) {
			// read parameters
			param_timer = 50;  // how often to update params (audio cycles)

			// rounding mode (-1 = down, 0 = nearest, 1 = up)
			rounding_mode = std::round(params[ROUNDING_PARAM].getValue()) - 1;

			// equally-likely note mode (0 = off, 1 = on)
			// makes the input voltage range for each note equivalent
			equi_likely = std::round(params[EQUI_PARAM].getValue());

			// equal temperament size
			equal_temp = clamp((int)(params[SIZE_PARAM].getValue()), 1, 34);
			step_size = 1.f / equal_temp;

			// tolerance of note matching interval
			tolerance = params[TOLERANCE_PARAM].getValue();  // read in 0-50¢
			tolerance = tolerance / 1200;  // convert ¢ to V

			// read interval buttons
			// include extra entry for octave, for rounding
			int interval_inputs[68];
			// break out bit 0 to avoid uninitialized warning
			interval_inputs[0] = clamp((int)(params[INTERVAL_PARAMS + 0].getValue()), 0, 1);
			for (int i = 1; i < num_intervals; i++)
				interval_inputs[i] = clamp((int)(params[INTERVAL_PARAMS + i].getValue()), 0, 1);
			interval_inputs[num_intervals] = interval_inputs[0];  // map unison to octave

			// determine if any interval lights pressed
			int interval_light_pressed = -1;
			for (int i = 0; i < 67; i++) {
				if (clamp((int)(params[INTERVAL_LIGHT_PARAMS + i].getValue()), 0, 1) == 1)
					interval_light_pressed = i;
			}
			// determine if any note lights pressed
			int note_light_pressed = -1;
			for (int i = 0; i < equal_temp; i++) {
				if (clamp((int)(params[NOTE_LIGHT_PARAMS + i].getValue()), 0, 1) == 1)
					note_light_pressed = i;
			}

			int clear_all = clamp((int)(params[CLEAR_ALL_PARAM].getValue()), 0, 1);
			int show_small = clamp((int)(params[SHOW_SMALL_PARAM].getValue()), 0, 1);
			int sel_enabled = clamp((int)(params[SEL_ENABLED_PARAM].getValue()), 0, 1);
			int show_allowed = clamp((int)(params[SHOW_ALLOWED_PARAM].getValue()), 0, 1);

			// initialize
			int note_used[MAX_SCALE + 1];  // include octave
			for (int i = 0; i < MAX_SCALE + 1; i++)
				note_used[i] = -1;  // -1 == unused, 0 - 31, or 0 - 61 == pointer to interval

			float interval_used[68];
			float error_table[68];
			for (int i = 0; i < num_intervals + 1; i++)
				interval_used[i] = -1.f;  // < -0.5f == unused, 0.f - min(tolerance, step_size) == current error

			// sweep through intervals
			// find closest note and error (guaranteed error < step_size)
			// if also within tolerance,
			//     if note currently unused update note and error
			//     if used, only update if closer, and remove previous interval
			// feed notes into input_scale
			// light up used interval lights, with brightness based on error (greater error == dimmer, over tolerance == off)

			for (int i = 0; i < num_intervals + 1; i++) {
				if (interval_inputs[i] == 1) {
					int closest_note = floorf((interval_lu[i] / step_size) + 0.5f);
					float raw_error = interval_lu[i] - closest_note * step_size;
					float error = fabsf(raw_error);
					if (error <= tolerance) {
						int prev_int = note_used[closest_note];
						if (prev_int >= 0 && error < interval_used[prev_int]) {
							interval_used[prev_int] = -1.f;  // zero out previous interval
							note_used[closest_note] = i;
							interval_used[i] = error;
							error_table[i] = raw_error;
						}
						else if (prev_int < 0) {
							note_used[closest_note] = i;
							interval_used[i] = error;
							error_table[i] = raw_error;
						}
					}
				}
			}

			// these will take effect next time around
			if (clear_all == 1) {  // except for root value
				params[INTERVAL_PARAMS + 0].setValue(1);
				for (int i = 1; i < num_intervals; i++)
					params[INTERVAL_PARAMS + i].setValue(0);
			}
			else if (sel_enabled == 1) {
				for (int i = 0; i < num_intervals; i++)
					if (interval_used[i] > -0.5f)
						params[INTERVAL_PARAMS + i].setValue(1);
					else
						params[INTERVAL_PARAMS + i].setValue(0);
			}

			// scale is defined by used notes from above
			float input_scale[MAX_SCALE];
			for (int i = 0; i < MAX_SCALE; i++)
				input_scale[i] = (note_used[i] >= 0) ? 1 : 0;

			// show all allowed intervals by simulating all intervals selected
			// show notes allowed to be used, so can display later
			int d_note_used[MAX_SCALE + 1];  // include octave
			if (show_allowed == 1  || show_small == 1) {
				for (int i = 0; i < MAX_SCALE + 1; i++)
					d_note_used[i] = -1;  // -1 == unused, 0 - 31, or 0 - 61 == pointer to interval

				float d_interval_used[68];
				float d_error_table[68];
				for (int i = 0; i < num_intervals; i++)
					d_interval_used[i] = -1.f;  // < -0.5f == unused, 0.f - min(tolerance, step_size) == current error

				for (int i = 0; i < num_intervals + 1; i++) {
					if (show_small == 0 || (show_small == 1 && ratio_size[i] == 1)) {  // only check small if show small
						int closest_note = floorf((interval_lu[i] / step_size) + 0.5f);
						float raw_error = interval_lu[i] - closest_note * step_size;
						float error = fabsf(raw_error);
						if (error <= tolerance) {
							int prev_int = d_note_used[closest_note];
							if (prev_int >= 0 && error < d_interval_used[prev_int]) {
								d_interval_used[prev_int] = -1.f;  // zero out previous interval
								d_note_used[closest_note] = i;
								d_interval_used[i] = error;
								d_error_table[i] = raw_error;
							}
							else if (prev_int < 0) {
								d_note_used[closest_note] = i;
								d_interval_used[i] = error;
								d_error_table[i] = raw_error;
							}
						}
					}
				}
				// show all allowed lights, with error amount (test with positive and negative error)
				for (int i = 0; i < 67; i++) {
					if (d_interval_used[i] > -0.5) {  //interval used
						float error = d_error_table[i] * 1200.f;  // 1¢ = 1, 50¢ = 50
						float thresh = 2.5f;
						float top, bot;
						//
						// Interval lights now split into top and bottom halves, so I can indicate if actual note
						//   is above or below each interval. For example, superflat EDOs have closest note well below the
						//   3/2 interval, e.g. note 5 in 9 EDO (5\9). For these, the 3/2 light will be fairly dim, but
						//   the bottom half will be brighter.
						// Explanation of algorithm:
						// - Within thresh (2.5¢), both halves will be full brightness.
						// - Up to 5¢, light in direction of note full brightness.
						// -   Beyond that, it decays at (5¢/error)**0.7.
						// -   However, at 25¢, it decays at twice that rate.
						// - At 2.5¢, the light in the opposite direction decays at a much faster rate, (2.5¢/error)**1.5.
						//
						// top half
						if (error < 2.f * -thresh)
							top = std::pow(2.f * thresh / -error, 0.7f) * (error < -25.f ? (75.f + error)/50.f : 1.f);
						else if (error > thresh)
							top = std::pow(thresh / error, 1.5f);
						else
							top = 1.0;
						// bot half
						if (error > 2.f * thresh)
							bot = std::pow(2.f * thresh / error, 0.7f) * (error > 25.f ? (75.f - error)/50.f : 1.f);
						else if (error < -thresh)
							bot = std::pow(thresh / -error, 1.5f);
						else
							bot = 1.0;
						lights[INTERVAL_TOP_LIGHTS + i].setBrightness(top);
						lights[INTERVAL_BOT_LIGHTS + i].setBrightness(bot);
					}
					else {
						lights[INTERVAL_TOP_LIGHTS + i].setBrightness(0.f);
						lights[INTERVAL_BOT_LIGHTS + i].setBrightness(0.f);
					}
				}
			}
			else if (interval_light_pressed >= 0) {
				// first convert interval to note, could be none, if note exists repeat note_light_pressed code below
				// convert interval to note, depending on tolerance, could be none
				float a = interval_lu[interval_light_pressed] * equal_temp;
				int note = (int) roundf(a);
				float e = fabsf(a - note) / equal_temp;
				if (note == equal_temp)  // root can round to 35:18 if error big enough
					note = 0;
				if (e <= tolerance) {
					for (int i = 0; i < 67; i++) {
						float error = (interval_lu[i] - ((float) note / equal_temp)) * 1200.f;
						if (note == 0 && i == 66)  // root can match 35:18 if tolerance big enough.
							error = 1200.f * (interval_lu[i] - 1.f);
						if (std::fabs(error) <= 1200.f * fminf(tolerance, 0.5f / equal_temp)) {
							float thresh = 2.5f;
							float top, bot;
							// lights show direction to actual note
							// top half
							if (error < 2.f * -thresh)
								top = std::pow(2.f * thresh / -error, 0.7f) * (error < -25.f ? (75.f + error)/50.f : 1.f);
							else if (error > thresh)
								top = std::pow(thresh / error, 1.5f);
							else
								top = 1.0;
							// bot half
							if (error > 2.f * thresh)
								bot = std::pow(2.f * thresh / error, 0.7f) * (error > 25.f ? (75.f - error)/50.f : 1.f);
							else if (error < -thresh)
								bot = std::pow(thresh / -error, 1.5f);
							else
								bot = 1.0;
							lights[INTERVAL_TOP_LIGHTS + i].setBrightness(top);
							lights[INTERVAL_BOT_LIGHTS + i].setBrightness(bot);
						}
						else {
							lights[INTERVAL_TOP_LIGHTS + i].setBrightness(0.f);
							lights[INTERVAL_BOT_LIGHTS + i].setBrightness(0.f);
						}
					}
				}
			}
			else if (note_light_pressed >= 0) {
				// show all intervals valid for pressed note, based on set tolerance
				for (int i = 0; i < 67; i++) {
					float error = (interval_lu[i] - ((float) note_light_pressed / equal_temp)) * 1200.f;
					if (note_light_pressed == 0 && i == 66)  // root can match 35:18 if tolerance big enough.
						error = 1200.f * (interval_lu[i] - 1.f);
					if (std::fabs(error) <= 1200.f * fminf(tolerance, 0.5f / equal_temp)) {  // 50¢
						float thresh = 2.5f;
						float top, bot;
						// lights show direction to actual note
						// top half
						if (error < 2.f * -thresh)
							top = std::pow(2.f * thresh / -error, 0.7f) * (error < -25.f ? (75.f + error)/50.f : 1.f);
						else if (error > thresh)
							top = std::pow(thresh / error, 1.5f);
						else
							top = 1.0;
						// bot half
						if (error > 2.f * thresh)
							bot = std::pow(2.f * thresh / error, 0.7f) * (error > 25.f ? (75.f - error)/50.f : 1.f);
						else if (error < -thresh)
							bot = std::pow(thresh / -error, 1.5f);
						else
							bot = 1.0;
						lights[INTERVAL_TOP_LIGHTS + i].setBrightness(top);
						lights[INTERVAL_BOT_LIGHTS + i].setBrightness(bot);
					}
					else {
						lights[INTERVAL_TOP_LIGHTS + i].setBrightness(0.f);
						lights[INTERVAL_BOT_LIGHTS + i].setBrightness(0.f);
					}
				}
			}
			else {
				// show normal interval lights, which give error for selected intervals
				for (int i = 0; i < 67; i++) {
					if (interval_used[i] > -0.5) {  //interval used
						float error = error_table[i] * 1200.f;  // 1¢ = 1, 50¢ = 50
						float thresh = 2.5f;
						float top, bot;
						// lights show direction to actual note
						// top half
						if (error < 2.f * -thresh)
							top = std::pow(2.f * thresh / -error, 0.7f) * (error < -25.f ? (75.f + error)/50.f : 1.f);
						else if (error > thresh)
							top = std::pow(thresh / error, 1.5f);
						else
							top = 1.0;
						// bot half
						if (error > 2.f * thresh)
							bot = std::pow(2.f * thresh / error, 0.7f) * (error > 25.f ? (75.f - error)/50.f : 1.f);
						else if (error < -thresh)
							bot = std::pow(thresh / -error, 1.5f);
						else
							bot = 1.0;
						lights[INTERVAL_TOP_LIGHTS + i].setBrightness(top);
						lights[INTERVAL_BOT_LIGHTS + i].setBrightness(bot);
					}
					else {
						lights[INTERVAL_TOP_LIGHTS + i].setBrightness(0.f);
						lights[INTERVAL_BOT_LIGHTS + i].setBrightness(0.f);
					}
				}
			}

			// show note lights
			if (note_light_pressed >= 0) {
				for (int i = 0; i < 34; i++) {
					// just show note pressed
					if (i == note_light_pressed)
						lights[NOTE_LIGHTS + i].setBrightness(1);
					else
						lights[NOTE_LIGHTS + i].setBrightness(0);
				}
			}
			else if (interval_light_pressed >= 0) {
				// convert interval to note, depending on tolerance, could be none
				float a = interval_lu[interval_light_pressed] * equal_temp;
				int note = (int) roundf(a);
				float e = fabsf(a - note) / equal_temp;
				if (note == equal_temp)  // 35:18 can round to 1:1 if error big enough
					note = 0;
				if (e <= tolerance) {
					for (int i = 0; i < 34; i++) {
						// just show note pressed
						if (i == note)
							lights[NOTE_LIGHTS + i].setBrightness(1);
						else
							lights[NOTE_LIGHTS + i].setBrightness(0);
					}
				}
				else {
					for (int i = 0; i < 34; i++)
						lights[NOTE_LIGHTS + i].setBrightness(0);
				}
			}
			else if (show_allowed == 1  || show_small == 1) {
				int n;
				for (n = 0; n < equal_temp; n++) {
					lights[NOTE_LIGHTS + n].setBrightness((d_note_used[n] >= 0) ? 1 : 0);
				}
				for (n = n ; n < 34; n++) {
					lights[NOTE_LIGHTS + n].setBrightness(0);
				}
			}
			else {
				// show normal note lights
				int n;
				for (n = 0; n < equal_temp; n++) {
					lights[NOTE_LIGHTS + n].setBrightness((note_used[n] >= 0) ? 1 : 0);
				}
				for (n = n ; n < 34; n++) {
					lights[NOTE_LIGHTS + n].setBrightness(0);
				}
			}

			// generate scale[] with enabled notes up to equal_temp size
			note_per_oct = 0;
			for (int i = 0, j = 0; i < equal_temp; i++) {
				if (input_scale[i] > 0.5f) {
					scale[j++] = i;
					note_per_oct++;
				}
			}
			// zero notes enabled, equal to just root selected
			if (note_per_oct == 0) {
				note_per_oct = 1;
				scale[0] = 0;
			}
			scale[note_per_oct] = scale[0] + equal_temp;  // for rounding

			// define lookup tables for rounding modes
			int j = (scale[0] == 0) ? 0 : -1;  // adjustment if 1st note not root
			for (int i = 0; i < equal_temp; i++) {
				if (i >= scale[j + 1])
					j++;
				lower[i] = (j < 0) ? scale[note_per_oct - 1] - equal_temp : scale[j];
				upper[i] = scale[j + 1];
			}

			// transpose to specified root
			int channels = inputs[ROOT_INPUT].getChannels();
			if (channels == 0)  // nothing plugged in, reset all channels to 0
				for (int c = 0; c < 16; c++)
					transpose[c] = 0.f;
			else if (channels == 1) { // mono tranpose, apply to all channels
				float vin = inputs[ROOT_INPUT].getVoltage(0);
				if (std::isnan(vin) || std::isinf(vin))  // NaN or ∞ crashed Rack
					vin = 0.f;
				float fracT = fmodf(vin, 1.f);
				if (fracT < 0.f ) // round to -∞
					fracT = (abs(fracT) < 1e-7) ? 0.f : fracT + 1.f;
				float qT = floor(equal_temp * fracT + 0.5f) / equal_temp; // quantize to chromatic scale
				for (int c = 0; c < 16; c++)
					transpose[c] = qT;
			}
			else { // full poly, separate transpose per channel
				for (int c = 0; c < channels; c++) {
					float vin = inputs[ROOT_INPUT].getVoltage(c);
					if (std::isnan(vin) || std::isinf(vin))  // NaN or ∞ crashed Rack
						vin = 0.f;
					float fracT = fmodf(vin, 1.f);
					if (fracT < 0.f ) // round to -∞
						fracT = (abs(fracT) < 1e-7) ? 0.f : fracT + 1.f;
					transpose[c] = floor(equal_temp * fracT + 0.5f) / equal_temp; // quantize to chromatic scale
				}
				for (int c = channels; c < 16; c++)  // zero out remaining
					transpose[c] = 0.f;
			}
		}
		else
			param_timer--;  // just decrement timer

		// quantize cv input (polyphonic)
		int channels = inputs[CV_IN_INPUT].getChannels();
		for (int c = 0; c < channels; c++) {
			float vin = inputs[CV_IN_INPUT].getVoltage(c);
			if (std::isnan(vin))  // NaN crashed Rack
				vin = 0.f;
			float intPart;
			float fracPart = modff(vin - transpose[c], &intPart);
			if (intPart < 0.f || fracPart < 0.f) { // round to -∞
				if (abs(fracPart) < 1e-7)
					fracPart = 0.f;
				else {
					fracPart += 1.f;
					intPart -= 1.f;
				}
			}
			int note;
			if (equi_likely == 0) {  // normal mode
				// quantize by half step, to match to VCV QNT and ML Quantum
				fracPart = equal_temp * fracPart;
				if (rounding_mode == -1) { // round down
					int n = floor(fracPart);
					if (fracPart > upper[n] - 0.5)
						note = upper[n];
					else
						note = lower[n];
				}
				else if (rounding_mode == 1) { // round up
					int n = floor(fracPart);
					if (fracPart > lower[n] + 0.5)
						note = upper[n];
					else
						note = lower[n];
				}
				else {  // round nearest (down)
					int n = floor(fracPart);
					float temp = (lower[n] + upper[n]) / 2.f;
					float threshold;
					// if threshold between notes, good to go
					if (abs(temp - floor(temp)) > 0.45)
						threshold = temp;
					else // up half a step
						threshold = temp + 0.5;
					if (fracPart > threshold)
						note = upper[n];
					else
						note = lower[n];
				}
			} else {  // equi-likely mode
				// in this case, can't serialize quantizers, so don't need window
				if (rounding_mode == -1) { // round down
					note = scale[(int)(floor(note_per_oct * fracPart))];
				}
				else if (rounding_mode == 1) { // round up
					note = scale[(int)(ceil(note_per_oct * fracPart))];
				}
				else {  // round nearest (down)
					note = scale[(int)(floor(note_per_oct * fracPart + 0.5f))];
				}
			}
			if (note == equal_temp) {
				intPart++;
				note = 0;
			}
			cv_out[c] = intPart + ((float) note / equal_temp) + transpose[c];
			outputs[CV_OUT_OUTPUT].setVoltage(cv_out[c], c);
			// generate trigger pulse on note change
			if (cv_out[c] != last_cv_out[c]) {
				pulseGenerators[c].trigger(1e-3f);
				last_cv_out[c] = cv_out[c];
			}
			bool pulse = pulseGenerators[c].process(args.sampleTime);
			outputs[TRIGGER_OUTPUT].setVoltage((pulse ? 10.f : 0.f), c);
			}
		outputs[CV_OUT_OUTPUT].setChannels(channels);
		outputs[TRIGGER_OUTPUT].setChannels(channels);
	}
};


struct QuantIntervalsWidget : ModuleWidget {
	QuantIntervalsWidget(QuantIntervals* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/QuantIntervals.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<TL1105>(mm2px(Vec(73.50-2.709, 17.50-2.709)), module, QuantIntervals::CLEAR_ALL_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(62.50-2.709, 17.50-2.709)), module, QuantIntervals::SHOW_ALLOWED_PARAM));

		addParam(createParam<TL1105>(mm2px(Vec(62.50-2.709, 29.75-2.709)), module, QuantIntervals::SHOW_SMALL_PARAM));
		addParam(createParam<TL1105>(mm2px(Vec(73.50-2.709, 29.75-2.709)), module, QuantIntervals::SEL_ENABLED_PARAM));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.00, 43.75)), module, QuantIntervals::TOLERANCE_PARAM));
		addParam(createParamCentered<RoundLargeRotarySwitch>(mm2px(Vec(68.00, 63.00)), module, QuantIntervals::SIZE_PARAM));

		addParam(createParam<CKSSThree>(mm2px(Vec(62.5-2.25, 79.00)), module, QuantIntervals::ROUNDING_PARAM));
		addParam(createParam<CKSS>(mm2px(Vec(73.5-2.25, 80.00)), module, QuantIntervals::EQUI_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.50, 100.0)), module, QuantIntervals::CV_IN_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(73.50, 100.0)), module, QuantIntervals::CV_OUT_OUTPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.50, 115.0)), module, QuantIntervals::ROOT_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(73.50, 115.0)), module, QuantIntervals::TRIGGER_OUTPUT));

		for (int i = 0; i < 67; i += 2)
			addParam(createParam<RectButton>(mm2px(Vec(14.00-3.50+1.325, 122.50 - 1.6 - 1.75*i)), module, QuantIntervals::INTERVAL_PARAMS + i));
		for (int i = 1; i < 67; i += 2)
			addParam(createParam<RectButton>(mm2px(Vec(30.00-3.50+1.325, 122.50 - 1.6 - 1.75*i)), module, QuantIntervals::INTERVAL_PARAMS + i));

		for (int i = 0; i < 67; i += 2) {
			addChild(createLightCentered<SmallLightTop<BlueLight>>(mm2px(Vec(20.25+1.325, 122.50 - 1.75*i)), module, QuantIntervals::INTERVAL_TOP_LIGHTS + i));
			addChild(createLightCentered<SmallLightBot<BlueLight>>(mm2px(Vec(20.25+1.325, 122.50 - 1.75*i)), module, QuantIntervals::INTERVAL_BOT_LIGHTS + i));
		}
		for (int i = 0; i < 67; i += 2)
			addParam(createParam<SmallLEDButton>(mm2px(Vec(20.25+1.325-1.5, 122.50-1.5 - 1.75*i)), module, QuantIntervals::INTERVAL_LIGHT_PARAMS + i));

		for (int i = 1; i < 67; i += 2) {
			addChild(createLightCentered<SmallLightTop<BlueLight>>(mm2px(Vec(23.75+1.325, 122.50 - 1.75*i)), module, QuantIntervals::INTERVAL_TOP_LIGHTS + i));
			addChild(createLightCentered<SmallLightBot<BlueLight>>(mm2px(Vec(23.75+1.325, 122.50 - 1.75*i)), module, QuantIntervals::INTERVAL_BOT_LIGHTS + i));
		}
		for (int i = 1; i < 67; i += 2)
			addParam(createParam<SmallLEDButton>(mm2px(Vec(23.75+1.325-1.5, 122.50-1.5 - 1.75*i)), module, QuantIntervals::INTERVAL_LIGHT_PARAMS + i));


		for (int i = 0; i < 34; i++)
			addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(52.75, 122.50 - 3.5*i)), module, QuantIntervals::NOTE_LIGHTS + i));

		for (int i = 0; i < 34; i++)
			addParam(createParam<SmallLEDButton>(mm2px(Vec(52.75-1.5, 122.50-1.5 - 3.5*i)), module, QuantIntervals::NOTE_LIGHT_PARAMS + i));
	}
};


Model* modelQuantIntervals = createModel<QuantIntervals, QuantIntervalsWidget>("QuantIntervals");
