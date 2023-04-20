#pragma once

#include "TPG/Utils/Utils.h"
#include "TPG/Utils/Spline.h"

struct TreeNode {
	vec3 pos;
	double width = DBL_MIN;

	TreeNode() {};
};

enum BRANCH_TYPE {
	BRANCH_REGULAR,
	BRANCH_LEAF
};

struct Branch {
	int ID;
	int parentBranchID;
	int parentBranchRootIndex;

	BRANCH_TYPE type;

	vector<TreeNode> nodes;
	double precision;

	vector<int> childrenIDs;

	Branch() : precision(1), parentBranchRootIndex(-1), isCurved(false), type(BRANCH_REGULAR) {}

	void smoothen();

	string str();
	void load(string);

private:
	vector<Spline> curve;
	void makeCurve();
	bool isCurved;
};