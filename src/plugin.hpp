#pragma once
#include <rack.hpp>
#include "components.hpp"


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelClip;
extern Model* modelLFO3;
extern Model* modelLogic;
extern Model* modelMerge8;
extern Model* modelMergeSplit4;
extern Model* modelMicrotonalChords;
extern Model* modelMicrotonalNotes;
extern Model* modelNoteMT;
extern Model* modelPeak;
extern Model* modelPolyMergeResplit;
extern Model* modelPolySplit;
extern Model* modelQuant;
extern Model* modelQuantIntervals;
extern Model* modelQuantMT;
extern Model* modelSampleDelays;
extern Model* modelScale;
extern Model* modelSplit8;
extern Model* modelTails;
extern Model* modelVarSampleDelays;
extern Model* modelVCA3;
