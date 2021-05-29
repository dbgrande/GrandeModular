#pragma once
#include <rack.hpp>
#include "components.hpp"


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelClip;
extern Model* modelMergeSplit4;
extern Model* modelMicrotonalChords;
extern Model* modelMicrotonalNotes;
extern Model* modelNoteMT;
extern Model* modelPolyMergeResplit;
extern Model* modelQuant;
extern Model* modelQuantIntervals;
extern Model* modelQuantMT;
extern Model* modelSampleDelays;
extern Model* modelScale;
extern Model* modelTails;
extern Model* modelVarSampleDelays;
