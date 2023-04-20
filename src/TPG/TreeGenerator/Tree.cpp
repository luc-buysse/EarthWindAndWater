#include "Tree.h"
#include "TPG/Net/Net.h"
#include <iostream>
#include <fstream>

using namespace cgp;

#define DEBUG false

void Tree::make(TreeModel model) {
	Net net(model);
	branches = net.getTree();

	initialize();
}

void Tree::renderBranch(int branchIndex) {
	Branch branch = branches[branchIndex];

	if (branch.nodes.size() < 2)
		return;

	Base prevBase, currentBase, nextBase;

	BranchSection currentSection;

	currentSection.x = branch.nodes[0].pos;
	currentSection.y = branch.nodes[1].pos;

	currentSection.bottomTexV = 0;
	currentSection.prevBase = Base(vec3(currentSection.y - currentSection.x));
	currentSection.currentBase = currentSection.prevBase;
	currentSection.nextBase = currentSection.currentBase;

	currentSection.rad_x = branch.nodes[0].width;
	currentSection.rad_y = branch.nodes[1].width;

	if (2 < branch.nodes.size()) {
		currentSection.nextBase.rotateYto(branch.nodes[2].pos - branch.nodes[1].pos);
	}

	for (int i = 0; i < branch.nodes.size()-1; i++) {
		addCylinder(currentSection);

		// Next section
		if (i < branch.nodes.size() - 2) {
			currentSection.x = branch.nodes[i + 1].pos;
			currentSection.y = branch.nodes[i + 2].pos;

			currentSection.bottomTexV = currentSection.topTexV;
			currentSection.prevBase = currentSection.currentBase;
			currentSection.currentBase = currentSection.nextBase;
			currentSection.nextBase = currentSection.currentBase;

			if (i < branch.nodes.size() - 3) {
				currentSection.nextBase.rotateYto(branch.nodes[i + 3].pos - branch.nodes[i + 2].pos);
			}

			currentSection.rad_x = branch.nodes[i + 1].width;
			currentSection.rad_y = branch.nodes[i + 2].width;
		}
	}
}

void Tree::addCylinder(BranchSection data) {

	// To be input data in the futur
	unsigned precision = 10;
	double texture_size = 0.7;

	// Tools
	unsigned numpoints = precision * 2;
	double height = norm(data.y - data.x);

	// Tex height and radial increment

	double tex_inc, tex_height;
	{
		double avg_rad = (data.rad_x + data.rad_y) / 2;
		double circum = avg_rad * 2 * Pi;

		int num_tex = circum / texture_size;

		tex_inc = (double)num_tex / precision;
		tex_height = texture_size / texture_size;
	}

	if (height > 0) {
		
		mat3 botRot = (data.currentBase + data.prevBase).normalize().matrix();
		mat3 topRot = (data.currentBase + data.nextBase).normalize().matrix();
		mat3 rot = data.currentBase.matrix();

		int pp = branchMesh.position.size();

		const double t = 2 * Pi / precision;
		for (int i = 0; i < precision + 1; i++) {

			vec3 t1 = topRot *  vec3(cos(i * t) * data.rad_y, 0, sin(i * t) * data.rad_y);
			vec3 b1 = botRot * vec3(cos(i * t) * data.rad_x, 0, sin(i * t) * data.rad_x);

			t1 += rot * vec3(0, height, 0);

			branchMesh.position.push_back(data.x + t1);
			branchMesh.position.push_back(data.x + b1);

			branchMesh.uv.push_back(vec2(i * tex_inc, data.bottomTexV + tex_height));
			branchMesh.uv.push_back(vec2(i * tex_inc, data.bottomTexV));

			branchMesh.normal.push_back(rot * vec3(cos(i * t), 0, sin(i * t)));
			branchMesh.normal.push_back(rot * vec3(cos(i * t), 0, sin(i * t)));

			if (i < precision) {
				int p1 = pp + i * 2;
				int p2 = pp + i * 2 + 1;
				int p3 = pp + (i * 2 + 2);
				int p4 = pp + (i * 2 + 3);

				branchMesh.connectivity.push_back(uint3(p1, p3, p4));
				branchMesh.connectivity.push_back(uint3(p1, p4, p2));
			}
		}

		data.topTexV = data.bottomTexV + tex_height;
	}
}

void Tree::renderLeaf(int leafIndex) {
	Branch leaf = branches[leafIndex];

	if (leaf.nodes.size() < 2) {
		cout << "Nombre de noeuds de la feuille invalide (< 2)" << endl;
		return;
	}
	
	vec3 dir = leaf.nodes[1].pos - leaf.nodes[0].pos;
	double height = norm(dir);
	normalize(dir);

	vec3 perp;
	
	while (norm(perp = cross(dir, randDir())) < 0.1);

	normalize(perp);

	int base_index = leafMesh.position.size();

	vec3 p1 = leaf.nodes[0].pos + perp * height / 2;
	vec3 p2 = leaf.nodes[0].pos - perp * height / 2;
	vec3 p3 = leaf.nodes[1].pos + perp * height / 2;
	vec3 p4 = leaf.nodes[1].pos - perp * height / 2;

	leafMesh.position.push_back(p1);
	leafMesh.position.push_back(p2);
	leafMesh.position.push_back(p3);
	leafMesh.position.push_back(p4);

	leafMesh.uv.push_back(vec2(0, 0));
	leafMesh.uv.push_back(vec2(0.5, 0));
	leafMesh.uv.push_back(vec2(0, 1));
	leafMesh.uv.push_back(vec2(0.5, 1));

	leafMesh.position.push_back(p1);
	leafMesh.position.push_back(p2);
	leafMesh.position.push_back(p3);
	leafMesh.position.push_back(p4);

	leafMesh.uv.push_back(vec2(0.5, 0));
	leafMesh.uv.push_back(vec2(1, 0));
	leafMesh.uv.push_back(vec2(0.5, 1));
	leafMesh.uv.push_back(vec2(1, 1));

	leafMesh.connectivity.push_back(uint3(base_index, base_index + 1, base_index + 2));
	leafMesh.connectivity.push_back(uint3(base_index + 1, base_index + 3, base_index + 2));

	base_index += 4;

	leafMesh.connectivity.push_back(uint3(base_index, base_index + 1, base_index + 2));
	leafMesh.connectivity.push_back(uint3(base_index + 1, base_index + 3, base_index + 2));
}

void Tree::initialize()
{
	for (int i = 0; i < branches.size(); i++) {
		if (!branches[i].type == BRANCH_LEAF)
			renderBranch(i);
		else;
			renderLeaf(i);
	}

	branchMesh.fill_empty_field();
	leafMesh.fill_empty_field();

	for (vec3& p : branchMesh.position) {
		float t = p.z;
		p.z = p.y;
		p.y = t;
	}

	for (vec3& p : leafMesh.position) {
		float t = p.z;
		p.z = p.y;
		p.y = t;
	}

	// Initialize branch drawable

	branchDrawable.shader_type = SHADER_TESSELATION;
	branchDrawable.initialize(branchMesh, "Tree");
	branchDrawable.shader = tesselationShader;

	// On charge les textures
	branchDrawable.texture = opengl_load_texture_image("assets/bark_2/Bark_007_BaseColor.jpg");
	branchDrawable.normal_texture = opengl_load_texture_image("assets/bark_2/Bark_007_Normal.jpg");
	branchDrawable.roughness_texture = opengl_load_texture_image("assets/bark_2/Bark_007_Roughness.jpg");
	branchDrawable.ambient_occlusion_texture = opengl_load_texture_image("assets/bark_2/Bark_007_AmbientOcclusion.jpg");
	branchDrawable.height_texture = opengl_load_texture_image("assets/bark_2/Bark_007_Height.png");
	branchDrawable.shading.phong.diffuse = 0.5;
	branchDrawable.shading.phong.ambient = 0.5;

	leafDrawable.shader_type = SHADER_BASIC;
	leafDrawable.initialize(leafMesh, "Leaf");
	leafDrawable.shader = regularShader;
	leafDrawable.texture = opengl_load_texture_image("assets/leaf.png");
}

void Tree::saveAs(string file_name) {
	ofstream output(file_name);

	for (Branch& b : branches) {
		output << "#begin_branch" << endl;

		output << b.str();

		output << "#end_branch" << endl;
	}

	output.close();
}

void Tree::load(string file_name) {
	ifstream input;

	input.open(file_name);
	string word;
	stringstream data;
	bool ongoing = false;

	// Lecture du fichier
	vector<string> input_str;
	while (true) {
		input >> word;
		if (input.eof())
			break;

		if (!word.compare("#begin_branch")) {
			ongoing = true;
		}
		else if (!word.compare("#end_branch")) {
			input_str.push_back(data.str());
			data.str(string());
			ongoing = false;
		}
		else if (ongoing) {
			data << word << "\n";
		}
	}

	// Initialisation des branches
	for (string& s : input_str) {
		Branch b;

		b.load(s);

		branches.push_back(b);
	}

	// Raccordement des branches
	for (int i = 0; i < branches.size(); i++) {
		if (i != branches[i].ID)
			cout << "Erreur problème d'indice" << endl;

		branches[branches[i].parentBranchID].childrenIDs.push_back(i);
	}

	initialize();
}