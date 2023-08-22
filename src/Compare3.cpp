#include "plugin.hpp"


struct Compare3 : Module {
	enum ParamIds {
		UPPER_A_PARAM,
		LOWER_A_PARAM,
		UPPER_B_PARAM,
		LOWER_B_PARAM,
		UPPER_C_PARAM,
		LOWER_C_PARAM,
		MODE_A_PARAM,
		MODE_B_PARAM,
		MODE_C_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		COMMON_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTA_OUTPUT,
		OUTB_OUTPUT,
		OUTC_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(A_LIGHTS, 21*3),
		ENUMS(B_LIGHTS, 21*3),
		ENUMS(C_LIGHTS, 21*3),
		ENUMS(MODE_A_LIGHT, 3),
		ENUMS(MODE_B_LIGHT, 3),
		ENUMS(MODE_C_LIGHT, 3),
		NUM_LIGHTS
	};

	Compare3() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(UPPER_A_PARAM, -10.f, 10.f, 0.f, "A Upper threshold down", "V");
		configParam(LOWER_A_PARAM, -10.f, 10.f, 0.f, "A Lower threshold up", "V");
		configParam(UPPER_B_PARAM, -10.f, 10.f, 0.f, "B Upper threshold down", "V");
		configParam(LOWER_B_PARAM, -10.f, 10.f, 0.f, "B Lower threshold up", "V");
		configParam(UPPER_C_PARAM, -10.f, 10.f, 0.f, "C Upper threshold down", "V");
		configParam(LOWER_C_PARAM, -10.f, 10.f, 0.f, "C Lower threshold up", "V");
		configInput(COMMON_INPUT, "Common");
		configOutput(OUTA_OUTPUT, "A");
		configButton(MODE_A_PARAM, "A Window mode");
		configOutput(OUTB_OUTPUT, "B");
		configButton(MODE_B_PARAM, "B Window mode");
		configOutput(OUTC_OUTPUT, "C");
		configButton(MODE_C_PARAM, "C Window mode");
		onReset();
	}

	float vin[16] = { 0.f };
	float upperA, lowerA, upperB, lowerB, upperC, lowerC;
	int param_timer = 0;
	int last_mode_a = -1;
	int last_mode_b = -1;
	int last_mode_c = -1;
	int mode_a = -1;
	int mode_b = -1;
	int mode_c = -1;
	int modestate[3] = { 0 };  // 0=windowed--red/blue, 1=lower--red, 2=upper--blue
	bool toward_zero = false;

	void process(const ProcessArgs &args) override {
		if (param_timer == 0) {
			param_timer = 25;  // how often to update params (audio cycles)

			// A
			mode_a = clamp((int)(params[MODE_A_PARAM].getValue()), 0, 1);
			if (mode_a == 1) {
				if (last_mode_a == 0) {
					modestate[0] = (modestate[0] + 1) % 3;
					last_mode_a = 1;
				}
			}
			else
				last_mode_a = 0;
			lights[MODE_A_LIGHT + 0].setBrightness(modestate[0] == 1);  // red
			lights[MODE_A_LIGHT + 1].setBrightness(false);  // green
			lights[MODE_A_LIGHT + 2].setBrightness(modestate[0] == 2);  // blue

			upperA = clamp((params[UPPER_A_PARAM].getValue()), -10.f, 10.f);
			lowerA = clamp((params[LOWER_A_PARAM].getValue()), -10.f, 10.f);

			for (int i = -10; i <= 10; i++) {
				int iup = roundf(upperA);
				int ilo = roundf(lowerA);

				if (modestate[0] == 0) {  // windowed mode
					if (i == iup) {
						if (iup == ilo) {
							// c_both
							lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.75);  // red
							lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.75);  // blue
						}
						else {
							// c_up
							lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(1.0);  // blue
						}
					}
					else if (i == ilo) {
						// c_lo
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(1.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else if (upperA >= lowerA && i > ilo && i < iup) {
						// c_between
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.35);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.35);  // blue
					}
					else if (lowerA > upperA) {
						if (i > ilo) {
							// c_above_lo
							lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.5);  // red
							lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.05);  // green
							lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
						}
						else if (i < iup) {
							// c_below_up
							lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.3);  // green
							lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.3);  // blue
						}
						else {
							// c_off
							lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
						}
					}
					else {
						// c_off
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
				else if (modestate[0] == 1) {  // lower threshold mode (red - true for higher voltages)
					if (i == ilo) {
						// c_lo
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(1.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else if (i > ilo) {
						// c_above_lo
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.5);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.05);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else {
						// c_off
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
				else {  // upper threshold mode (blue - true for lower voltages)
					if (i == iup) {
						// c_up
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(1.0);  // blue
					}
					else if (i < iup) {
						// c_below_up
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.3);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.3);  // blue
					}
					else {
						// c_off
						lights[A_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[A_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[A_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
			}

			// B
			mode_b = clamp((int)(params[MODE_B_PARAM].getValue()), 0, 1);
			if (mode_b == 1) {
				if (last_mode_b == 0) {
					modestate[1] = (modestate[1] + 1) % 3;
					last_mode_b = 1;
				}
			}
			else
				last_mode_b = 0;
			lights[MODE_B_LIGHT + 0].setBrightness(modestate[1] == 1);  // red
			lights[MODE_B_LIGHT + 1].setBrightness(false);  // green
			lights[MODE_B_LIGHT + 2].setBrightness(modestate[1] == 2);  // blue

			upperB = clamp((params[UPPER_B_PARAM].getValue()), -10.f, 10.f);
			lowerB = clamp((params[LOWER_B_PARAM].getValue()), -10.f, 10.f);

			for (int i = -10; i <= 10; i++) {
				int iup = roundf(upperB);
				int ilo = roundf(lowerB);

				if (modestate[1] == 0) {  // windowed mode
					if (i == iup) {
						if (iup == ilo) {
							// c_both
							lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.75);  // red
							lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.75);  // blue
						}
						else {
							// c_up
							lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(1.0);  // blue
						}
					}
					else if (i == ilo) {
						// c_lo
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(1.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else if (upperB >= lowerB && i > ilo && i < iup) {
						// c_between
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.35);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.35);  // blue
					}
					else if (lowerB > upperB) {
						if (i > ilo) {
							// c_above_lo
							lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.5);  // red
							lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.05);  // green
							lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
						}
						else if (i < iup) {
							// c_below_up
							lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.3);  // green
							lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.3);  // blue
						}
						else {
							// c_off
							lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
						}
					}
					else {
						// c_off
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
				else if (modestate[1] == 1) {  // lower threshold mode (red - true for higher voltages)
					if (i == ilo) {
						// c_lo
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(1.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else if (i > ilo) {
						// c_above_lo
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.5);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.05);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else {
						// c_off
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
				else {  // upper threshold mode (blue - true for lower voltages)
					if (i == iup) {
						// c_up
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(1.0);  // blue
					}
					else if (i < iup) {
						// c_below_up
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.3);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.3);  // blue
					}
					else {
						// c_off
						lights[B_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[B_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[B_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
			}

			// C
			mode_c = clamp((int)(params[MODE_C_PARAM].getValue()), 0, 1);
			if (mode_c == 1) {
				if (last_mode_c == 0) {
					modestate[2] = (modestate[2] + 1) % 3;
					last_mode_c = 1;
				}
			}
			else
				last_mode_c = 0;
			lights[MODE_C_LIGHT + 0].setBrightness(modestate[2] == 1);  // red
			lights[MODE_C_LIGHT + 1].setBrightness(false);  // green
			lights[MODE_C_LIGHT + 2].setBrightness(modestate[2] == 2);  // blue

			upperC = clamp((params[UPPER_C_PARAM].getValue()), -10.f, 10.f);
			lowerC = clamp((params[LOWER_C_PARAM].getValue()), -10.f, 10.f);

			for (int i = -10; i <= 10; i++) {
				int iup = roundf(upperC);
				int ilo = roundf(lowerC);

				if (modestate[2] == 0) {  // windowed mode
					if (i == iup) {
						if (iup == ilo) {
							// c_both
							lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.75);  // red
							lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.75);  // blue
						}
						else {
							// c_up
							lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(1.0);  // blue
						}
					}
					else if (i == ilo) {
						// c_lo
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(1.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else if (upperC >= lowerC && i > ilo && i < iup) {
						// c_between
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.35);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.35);  // blue
					}
					else if (lowerC > upperC) {
						if (i > ilo) {
							// c_above_lo
							lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.5);  // red
							lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.05);  // green
							lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
						}
						else if (i < iup) {
							// c_below_up
							lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.3);  // green
							lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.3);  // blue
						}
						else {
							// c_off
							lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
							lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
							lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
						}
					}
					else {
						// c_off
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
				else if (modestate[2] == 1) {  // lower threshold mode (red - true for higher voltages)
					if (i == ilo) {
						// c_lo
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(1.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else if (i > ilo) {
						// c_above_lo
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.5);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.05);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
					else {
						// c_off
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
				else {  // upper threshold mode (blue - true for lower voltages)
					if (i == iup) {
						// c_up
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(1.0);  // blue
					}
					else if (i < iup) {
						// c_below_up
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.3);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.3);  // blue
					}
					else {
						// c_off
						lights[C_LIGHTS + (i+10)*3 + 0].setBrightness(0.0);  // red
						lights[C_LIGHTS + (i+10)*3 + 1].setBrightness(0.0);  // green
						lights[C_LIGHTS + (i+10)*3 + 2].setBrightness(0.0);  // blue
					}
				}
			}
		}
		else
			param_timer--;  // just decrement timer

		// Audio rate processing
		// Common Input
		int channels_in = inputs[COMMON_INPUT].getChannels();
		if (channels_in > 16)
			channels_in = 16;  // Deal with broken polymerge

		for (int c = 0 ; c < channels_in; c++)
			vin[c] = inputs[COMMON_INPUT].getVoltage(c);

		// For '≤' vs '<' and '≥' vs '>' conditions
		// Normal mode uses [lo..up) everywhere
		// Equal toward zero uses (lo..up] both when neg, [lo..up) when both pos, (lo..up) when straddle 0
		bool lower_cond = false;
		bool upper_cond = false;

		// A compare
		for (int c = 0; c < channels_in; c++) {
			lower_cond = (toward_zero && lowerA < 0.f) ? vin[c] > lowerA : vin[c] >= lowerA;
			upper_cond = (toward_zero && upperA <= 0.f) ? vin[c] <= upperA : vin[c] < upperA; 
			if (modestate[0] == 0 ) {  // windowed mode
				if (upperA == lowerA)
					outputs[OUTA_OUTPUT].setVoltage(10.f * (vin[c] == lowerA), c);
				else if (upperA > lowerA)
					outputs[OUTA_OUTPUT].setVoltage(10.f * (lower_cond && upper_cond), c);
				else  // upperA < lowerA
					outputs[OUTA_OUTPUT].setVoltage(10.f * (lower_cond || upper_cond), c);
			}
			else if (modestate[0] == 1) {  // red lower, voltages above
				outputs[OUTA_OUTPUT].setVoltage(10.f * lower_cond, c);
			}
			else {  // blue upper, voltages below
				outputs[OUTA_OUTPUT].setVoltage(10.f * upper_cond, c);
			}
		}
		outputs[OUTA_OUTPUT].setChannels(channels_in);

		// B compare
		for (int c = 0; c < channels_in; c++) {
			lower_cond = (toward_zero && lowerB < 0.f) ? vin[c] > lowerB : vin[c] >= lowerB;
			upper_cond = (toward_zero && upperB <= 0.f) ? vin[c] <= upperB : vin[c] < upperB; 
			if (modestate[1] == 0 ) {  // windowed mode
				if (upperB == lowerB)
					outputs[OUTB_OUTPUT].setVoltage(10.f * (vin[c] == lowerB), c);
				else if (upperB > lowerB)
					outputs[OUTB_OUTPUT].setVoltage(10.f * (lower_cond && upper_cond), c);
				else  // upperB < lowerB
					outputs[OUTB_OUTPUT].setVoltage(10.f * (lower_cond || upper_cond), c);
			}
			else if (modestate[1] == 1) {  // red lower, voltages above
				outputs[OUTB_OUTPUT].setVoltage(10.f * lower_cond, c);
			}
			else {  // blue upper, voltages below
				outputs[OUTB_OUTPUT].setVoltage(10.f * upper_cond, c);
			}
		}
		outputs[OUTB_OUTPUT].setChannels(channels_in);

		// C compare
		for (int c = 0; c < channels_in; c++) {
			lower_cond = (toward_zero && lowerC < 0.f) ? vin[c] > lowerC : vin[c] >= lowerC;
			upper_cond = (toward_zero && upperC <= 0.f) ? vin[c] <= upperC : vin[c] < upperC; 
			if (modestate[2] == 0 ) {  // windowed mode
				if (upperC == lowerC)
					outputs[OUTC_OUTPUT].setVoltage(10.f * (vin[c] == lowerC), c);
				else if (upperC > lowerC)
					outputs[OUTC_OUTPUT].setVoltage(10.f * (lower_cond && upper_cond), c);
				else  // upperC < lowerC
					outputs[OUTC_OUTPUT].setVoltage(10.f * (lower_cond || upper_cond), c);
			}
			else if (modestate[2] == 1) {  // red lower, voltages above
				outputs[OUTC_OUTPUT].setVoltage(10.f * lower_cond, c);
			}
			else {  // blue upper, voltages below
				outputs[OUTC_OUTPUT].setVoltage(10.f * upper_cond, c);
			}
		}
		outputs[OUTC_OUTPUT].setChannels(channels_in);
	}

	void onReset() override {
		for (int c = 0; c < 3; c++)
			modestate[c] = 0;
		toward_zero = false;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();

		// modestates
		json_t* modestatesJ = json_array();
		for (int i = 0; i < 3; i++) {
			json_t* modestateJ = json_integer(modestate[i]);
			json_array_append_new(modestatesJ, modestateJ);
		}
		json_object_set_new(rootJ, "modestates", modestatesJ);
		// toward_zero
		json_object_set_new(rootJ, "toward_zero", json_boolean(toward_zero));

		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// modestates
		json_t* modestatesJ = json_object_get(rootJ, "modestates");
		if (modestatesJ) {
			for (int i = 0; i < 3; i++) {
				json_t* modestateJ = json_array_get(modestatesJ, i);
				if (modestateJ)
					modestate[i] = json_integer_value(modestateJ);
			}
		}
		// toward_zero
		json_t* toward_zeroJ = json_object_get(rootJ, "toward_zero");
		if (toward_zeroJ)
			toward_zero = json_boolean_value(toward_zeroJ);
	}
};


struct Compare3Widget : ModuleWidget {
	Compare3Widget(Compare3* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/Compare3.svg"),
			asset::plugin(pluginInstance, "res/Compare3-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(0, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Common Input
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 18.25)), module, Compare3::COMMON_INPUT));

		// A
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.96, 32.00)), module, Compare3::UPPER_A_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.96, 40.80)), module, Compare3::LOWER_A_PARAM));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 51.25)), module, Compare3::OUTA_OUTPUT));
		for (int i = 0; i < 21; i++) {
			addChild(createLightCentered<TinyLightRect<RedGreenBlueLight>>(mm2px(Vec(1.58, 47.00 - 1.00*i)), module, Compare3::A_LIGHTS + i*3));
		}
		addChild(createLightCentered<SmallLightFlat<RedGreenBlueLight>>(mm2px(Vec(1.78, 54.8)), module, Compare3::MODE_A_LIGHT));
		addParam(createParam<SmallLEDButton>(mm2px(Vec(1.78-1.50, 54.8-1.5)), module, Compare3::MODE_A_PARAM));

		// B
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.96, 64.00)), module, Compare3::UPPER_B_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.96, 72.80)), module, Compare3::LOWER_B_PARAM));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 83.25)), module, Compare3::OUTB_OUTPUT));
		for (int i = 0; i < 21; i++) {
			addChild(createLightCentered<TinyLightRect<RedGreenBlueLight>>(mm2px(Vec(1.58, 79.00 - 1.00*i)), module, Compare3::B_LIGHTS + i*3));
		}
		addChild(createLightCentered<SmallLightFlat<RedGreenBlueLight>>(mm2px(Vec(1.78, 86.8)), module, Compare3::MODE_B_LIGHT));
		addParam(createParam<SmallLEDButton>(mm2px(Vec(1.78-1.50, 86.8-1.5)), module, Compare3::MODE_B_PARAM));

		// C
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.96, 96.00)), module, Compare3::UPPER_C_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(5.96, 104.80)), module, Compare3::LOWER_C_PARAM));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(5.08, 115.25)), module, Compare3::OUTC_OUTPUT));
		for (int i = 0; i < 21; i++) {
			addChild(createLightCentered<TinyLightRect<RedGreenBlueLight>>(mm2px(Vec(1.58, 111.00 - 1.00*i)), module, Compare3::C_LIGHTS + i*3));
		}
		addChild(createLightCentered<SmallLightFlat<RedGreenBlueLight>>(mm2px(Vec(1.78, 118.8)), module, Compare3::MODE_C_LIGHT));
		addParam(createParam<SmallLEDButton>(mm2px(Vec(1.78-1.50, 118.8-1.5)), module, Compare3::MODE_C_PARAM));
	}

	void appendContextMenu(Menu* menu) override {
		Compare3* module = dynamic_cast<Compare3*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createBoolPtrMenuItem("Close interval toward zero", "", &module->toward_zero));
	}
};


Model* modelCompare3 = createModel<Compare3, Compare3Widget>("Compare3");
