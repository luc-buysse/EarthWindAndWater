#pragma once

#ifndef TREE_GENERATOR
#define TREE_GENERATOR

#include "TPG/Utils/Utils.h"
#include "TPG/TreeModel/TreeModel.h"
#include "TPG/TreeGenerator/Base.h"
#include "TPG/TreeGenerator/Branch.h"
#include "custom_drawable.h"

class BranchSection {
public:
	// Input
	vec3 x,y;
	double rad_x, rad_y;
	double bottomTexV;
	Base prevBase, currentBase, nextBase;

	// Ouput
	double topTexV;
};

class Tree {
public:
	Tree() {}

	void make(TreeModel);

	GLuint tesselationShader;
	GLuint regularShader;

	template<class SCENE_ENVIRONMENT>
	void render(SCENE_ENVIRONMENT const& env) {
		branchDrawable.model_matrix = model_matrix;
		leafDrawable.model_matrix = model_matrix;

		draw(branchDrawable, env);
		draw(leafDrawable, env);
	}

	mat4 model_matrix;
	void saveAs(string file_name);
	void load(string file_name);

private:
	vector<Branch> branches;
	void initialize();

	mesh branchMesh;
	mesh leafMesh;

	custom_drawable branchDrawable;
	custom_drawable leafDrawable;

	void renderBranch(int);
	void renderLeaf(int);

	void addCylinder(BranchSection section);
};

#endif