#pragma once

#ifndef TREE_MODEL
#define TREE_MODEL

#include "TPG/Utils/Utils.h"

static struct SelectorData {
	vec3 pos;

	unsigned hopCount;
	
	bool top;
	bool middle;
};

enum LayerType {
	REGULAR = 0,
	TWIG = 1,
	LEAF = 2,
};

struct BranchLayer {

	int numEndpoints;
	double startWidth;
	double ratio;
	double dec;
	bool guided;
	int precision;

	double smoothedPrecision;

	// Specific to twigs
	int numOuterEndpoints;

	LayerType type;

	std::function<bool(SelectorData)> endpointCandidateSelector;
	std::function<double(unsigned)> alpha;

	BranchLayer() : type(REGULAR), precision(1), ratio(0), dec(0), startWidth(0) {};
};

enum TreeType {
	TREE_BASIC = 0,
	TREE_SEQUOIA = 1,
	TREE_MOTHER = 2
};

struct TreeModel {
	vector<BranchLayer> layers;
	double trunkHeight;
	double minDist;
	vec3 dimensions;

	SelectorData selectorData;

	TreeModel(TreeType type);

private:
	void makeBasic();
	void makeSequoia();
	void makeMother();
};

#endif