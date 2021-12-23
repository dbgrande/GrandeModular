#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelClip);
	p->addModel(modelMerge8);
	p->addModel(modelMergeSplit4);
	p->addModel(modelMicrotonalChords);
	p->addModel(modelMicrotonalNotes);
	p->addModel(modelNoteMT);
	p->addModel(modelPolyMergeResplit);
	p->addModel(modelQuant);
	p->addModel(modelQuantIntervals);
	p->addModel(modelQuantMT);
	p->addModel(modelSampleDelays);
	p->addModel(modelScale);
	p->addModel(modelSplit8);
	p->addModel(modelTails);
	p->addModel(modelVarSampleDelays);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
