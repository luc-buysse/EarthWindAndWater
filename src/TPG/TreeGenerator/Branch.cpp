#include "Branch.h"

void Branch::smoothen() {
	if (!isCurved)
		makeCurve();

	vector<TreeNode> new_node_list;

	if (precision <= 1) {
		return;
	}
	
	int n = nodes.size();
	int num_points = precision * n;

	for (int i = 0; i <= num_points ; i++) {
		double pos = (double) (n-1) * i / num_points;
		int index = pos;
		double offset = pos - index;

		TreeNode new_node;
		if (index == num_points)
			new_node.width = (nodes[index + 1].width * offset + (1 - offset) * nodes[index].width);
		else
			new_node.width = nodes[index].width;

		new_node.pos = curve[index](offset);

		new_node_list.push_back(new_node);
	}

	nodes = new_node_list;
}

void Branch::makeCurve() {
	isCurved = true;

	if (nodes.size() < 2)
		return;

	std::function<int(int)> clamp = [&](int a) -> int {
		int res = a;

		if (a >= nodes.size())
			res = nodes.size() - 1;
		if (a < 0)
			res = 0;

		return res;
	};

	for (int i = 0; i < nodes.size() - 1; i++) {
		curve.push_back(Spline(nodes[clamp(i - 1)].pos, nodes[clamp(i)].pos, nodes[clamp(i + 1)].pos, nodes[clamp(i + 2)].pos));
	}
}

string Branch::str() {
	stringstream output;

	output << (type == BRANCH_LEAF ? "LEAF" : "BRANCH") << endl;
	output << ID << endl;
	output << parentBranchID << endl;
	output << parentBranchRootIndex << endl;
	output << precision << endl;

	for (int i = 0; i < nodes.size() ; i++) {
		TreeNode& n = nodes[i];
	}

	return output.str();
}

void Branch::load(string str_input) {
	stringstream input(str_input);

	vec3 pos;
	double width;

	string tmp;
	input >> tmp;

	if (!tmp.compare("LEAF")) {
		type = BRANCH_LEAF;
	}
	else {
		type = BRANCH_REGULAR;
	}

	input >> tmp;
	ID = stoi(tmp);

	input >> tmp;
	parentBranchID = stoi(tmp);

	input >> tmp;
	parentBranchRootIndex = stoi(tmp);

	input >> tmp;
	precision = stoi(tmp);

	while (true) {

		input >> tmp;
		if (input.eof()) {
			break;
		}

		pos.x = stof(tmp);

		input >> tmp;
		pos.y = stof(tmp);

		input >> tmp;
		pos.z = stof(tmp);

		input >> tmp;
		width = stof(tmp);

		TreeNode n;
		n.pos = pos;
		n.width = width;

		nodes.push_back(n);
	}
}