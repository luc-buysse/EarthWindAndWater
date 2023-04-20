#include "Net.h"
#include "Utils/PoissonDisk.h"

void Net::debug(mesh& res) {
	for (Node& cn : m_Nodes) {
		int ci = res.position.size();

		ci = res.position.size();

		res.position.push_back(cn.getPos() + vec3(0, 0.02, 0));
		res.position.push_back(cn.getPos() + vec3(0, -0.02, 0.01));
		res.position.push_back(cn.getPos() + vec3(0.01, -0.02, 0));

		res.color.push_back(vec3(0, 1, 0));
		res.color.push_back(vec3(0, 1, 0));
		res.color.push_back(vec3(0, 1, 0));

		res.connectivity.push_back(uint3(ci, ci + 1, ci + 2));
	}
}

void Net::makeUnlimitedNeigh() {
	KdTreeType kdTree(3, *this);

	for (Node& cn : m_Nodes) {
		const int numLvls = 4;

		vector<Node*> levels[numLvls];

		vec3 pos = cn.getPos();
		double cn_pos[3];
		cn_pos[0] = pos.x;
		cn_pos[1] = pos.y;
		cn_pos[2] = pos.z;

		vector<nanoflann::ResultItem<uint32_t, double>> nearestNodesIndices;
		kdTree.radiusSearch(cn_pos, minDist * 6, nearestNodesIndices);

		for (nanoflann::ResultItem<uint32_t, double> resPair : nearestNodesIndices) {
			uint32_t i = resPair.first;
			Node& no = m_Nodes[i];

			if (&cn != &no) {
				double d = cn.distTo(no);
				if (d < minDist * 2) {
					levels[0].push_back(&no);
				}
				else if (d < minDist * 3) {
					levels[1].push_back(&no);
				}
				else if (d < minDist * 4) {
					levels[2].push_back(&no);
				}
				else if (d < minDist * 6) {
					levels[3].push_back(&no);
				}
			}
		}

		for (int i = 0; i < numLvls; i++) {
			for (Node* no : levels[i])
				cn.neigh.push_back(no);
			cn.precisionLevels.push_back(cn.neigh.size());
		}
	}
}

void Net::makeYao() {
	for (Node& cn : m_Nodes) {
		double dist[8];
		Node* nei[8];
		for (int i = 0; i < 8; i++) {
			dist[i] = DBL_MAX;
			nei[i] = NULL;
		}

		for (Node& no : m_Nodes) {
			if (&cn != &no) {
				int i = getDir(cn, no);
				double d = cn.distTo(no);
				if (d < dist[i] && d < minDist * 3) {
					dist[i] = cn.distTo(no);
					nei[i] = &no;
				}
			}
		}

		for (int i = 0; i < 8; i++) {
			if (nei[i] != NULL)
				cn.neigh.push_back(nei[i]);
		}
	}
}

void Net::buildPoissonDisk() {
	int rootIndex = 0;
	int topIndex = 0;

	int trunkSize = trunkHeight / minDist;

	vector<vec3> points;
	points.push_back(vec3(dimensions.x, dimensions.y / 2, dimensions.z));

	PoissonDisk pd(minDist, vec3(dimensions.x * 2, dimensions.y, dimensions.z * 2), points);

	points = pd.generate();

	m_Nodes.resize(points.size() + trunkSize);

	double min_rdist = DBL_MAX;
	double min_tdist = DBL_MAX;

	for (int i = 0; i < points.size(); i++) {
		m_Nodes[i] = Node(points[i] + vec3(-dimensions.x, trunkHeight, -dimensions.z));

		double tdist = m_Nodes[i].distTo(vec3(0, dimensions.y + trunkHeight, 0));

		if (tdist < min_tdist) {
			min_tdist = tdist;
			top = &m_Nodes[i];
		}
	}

	int offset = points.size();

	for (int i = 0; i < trunkSize; i++) {
		int index = i + offset;

		m_Nodes[index] = Node(vec3(0, minDist * i, 0));

		if (i == 0)
			root = &m_Nodes[index];
		if (i == trunkSize - 1)
			middle = &m_Nodes[index];
	}

	cout << root->getPos() << endl;
}

void Net::buildNet() {

	buildPoissonDisk();

	makeUnlimitedNeigh();

	root->dist = 0;
	root->guide = vec3(0, 1, 0);

	root->updateWeigh();
}

void Net::makeBranches(std::function<bool(SelectorData)> endpointCandidateSelector, std::function<double(unsigned)> alpha, int precision) {
	vector<Node::NodePtr> heap(m_Nodes.size());

	for (int i = 0; i < heap.size(); i++) {
		heap[i].ptr = &m_Nodes[i];
		heap[i].pos = i;
		m_Nodes[i].nodePtr = &heap[i];
	}

	make_heap(heap.begin(), heap.end(), nodecmp);
	vector<Node::NodePtr>::iterator end = heap.end();
	while (end != heap.begin()) {
		pop_heap(heap.begin(), end, nodecmp);
		Node& cn = **(--end);

		if (cn.parent != NULL && !cn.inTree) {
			cn.hopCount = cn.parent->hopCount + 1;

			vec3 axis = cross(vec3(0, 1, 0), cn.parent->guide);
			if (norm(axis) < 0.01) {
				vec3 tmpPos = cn.parent->getPos() - cn.getPos();
				axis = cross(cn.parent->guide, tmpPos);
			}
			if (norm(axis) > 0.01) {
				double angle = alpha(cn.hopCount);
				cn.guide = rotation_transform::from_axis_angle(axis, angle) * cn.parent->guide;
				cn.guide = normalize(cn.guide);
			}
			else {
				cn.guide = cn.parent->guide;
			}

			// Filling  SelectorData
			SelectorData selData;
			selData.hopCount = cn.hopCount;
			selData.middle = (&cn == middle);
			selData.top = ( &cn == top);
			selData.pos = cn.getPos();

			if (endpointCandidateSelector(selData)) {
				endpoints.push_back(&cn);
			}
			
			cn.updateWeigh();
		}

		if (heap.end() - end < 10 && DEBUG)
			cout << "Pos : " << end - heap.begin() << endl
			<< "Current point : " << endl
			<< "x : " << cn.x << endl
			<< "y : " << cn.y << endl
			<< "z : " << cn.z << endl
			<< "dist : " << cn.dist << endl
			<< "guide : " << cn.guide << endl
			<< "adress : " << &cn << endl
			<< "parent : " << &cn.parent << endl;

		for (int i = 0; i < cn.precisionLevels[precision]; i++) {
			Node* c = cn.neigh[i];

			if (heap.end() - end < 10 && DEBUG)
				cout << "Neigh : " << endl
				<< "x : " << c->x << endl
				<< "y : " << c->y << endl
				<< "z : " << c->z << endl
				<< "dist : " << c->dist << endl;

			if (cn.dist != DBL_MAX && (c->dist == DBL_MAX || c->dist > cn.dist + cn.weigh[i])) {
				c->dist = cn.dist + cn.weigh[i];

				if (heap.end() - end < 10 && DEBUG)
					cout << "Updated : " << c->dist << endl;

				c->parent = &cn;

				push_heap(heap.begin(), heap.begin() + c->nodePtr->pos + 1, nodecmp);
			}
		}
	}
}

void Net::selectEndpoints(BranchLayer b) {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(endpoints.begin(), endpoints.end(), std::default_random_engine(seed));

	finalEndpoints.resize(finalEndpoints.size() + 1);
	int index = finalEndpoints.size() - 1;

	int nb = b.numEndpoints;

	if (endpoints.size() < nb) {
		cout << "Trop peu d'endpoints pour en sélectionner " << nb << " (il n'y en a que " << endpoints.size() << ")" << endl;
		endpoints.clear();
		return;
	}

	for (int i = 0; i < nb; i++) {
		finalEndpoints[index].push_back(endpoints[i]);
	}

	endpoints.clear();
}

void Net::resetNonGuide() {
	for (vector<Node*> fe : finalEndpoints)
		for (Node* n : fe) {
			Node* c = n;
			do {
				c->inTree = true;
				c->dist = 0;
				c->hopCount = 0;
				c = c->parent;
			} while (c != NULL);
		}

	for (Node& n : m_Nodes) {
		if (!n.inTree) {
			n.clear();
		}
	}
}

void Net::selectTwigEndpoints(BranchLayer layer) {
	vec3 center = vec3(0, trunkHeight + dimensions.y / 2, 0);
	double liminf = 0.1;

	vector<bool> valid(endpoints.size());
	fill(valid.begin(), valid.end(), true);

	for (int i = 0; i < endpoints.size(); i++) {
		Node* n1 = endpoints[i];

		for (Node* n2 : endpoints) {
			if (n1 != n2) {
				vec3 p1 = n1->getPos();
				vec3 p2 = n2->getPos();

				if (distance(center, p1) < minDist) continue;

				vec3 cp1 = normalize(p1 - center);
				vec3 p1p2 = normalize(p2 - p1);

				if (dot(cp1, p1p2) > 0.8) {
					valid[i] = false;
				}
			}
		}
	}

	vector<Node*> outerEndpoints;
	vector<Node*> innerEndpoints;

	for (int i = 0; i < endpoints.size(); i++) {
		if (valid[i]) {
			outerEndpoints.push_back(endpoints[i]);
		}
		else {
			innerEndpoints.push_back(endpoints[i]);
		}
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(innerEndpoints.begin(), innerEndpoints.end(), std::default_random_engine(seed));
	shuffle(outerEndpoints.begin(), outerEndpoints.end(), std::default_random_engine(seed));

	int index = finalEndpoints.size();

	finalEndpoints.push_back(vector<Node*>());

	finalEndpoints[index].resize(0);

	for (int i = 0; i < ((innerEndpoints.size() > layer.numEndpoints) ? layer.numEndpoints : innerEndpoints.size()); i++) {
		finalEndpoints[index].push_back(innerEndpoints[i]);
	}

	for (int i = 0; i < ((outerEndpoints.size() > layer.numOuterEndpoints) ? layer.numOuterEndpoints : outerEndpoints.size()); i++) {
		finalEndpoints[index].push_back(outerEndpoints[i]);
	}

	endpoints.clear();
}

void Net::selectLeafEndpoints(BranchLayer layer) {
	vec3 center = vec3(0, trunkHeight + dimensions.y / 2, 0);
	double liminf = 0.1;

	vector<bool> valid(endpoints.size());
	fill(valid.begin(), valid.end(), true);

	for (int i = 0; i < endpoints.size(); i++) {
		Node* n1 = endpoints[i];

		for (Node* n2 : endpoints) {
			if (n1 != n2) {
				vec3 p1 = n1->getPos();
				vec3 p2 = n2->getPos();

				if (distance(center, p1) < minDist) continue;

				vec3 cp1 = normalize(p1 - center);
				vec3 p1p2 = normalize(p2 - p1);

				if (dot(cp1, p1p2) > 0.8) {
					valid[i] = false;
				}
			}
		}
	}

	vector<Node*> outerEndpoints;
	vector<Node*> innerEndpoints;

	for (int i = 0; i < endpoints.size(); i++) {
		if (valid[i]) {
			outerEndpoints.push_back(endpoints[i]);
		}
		else {
			innerEndpoints.push_back(endpoints[i]);
		}
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(innerEndpoints.begin(), innerEndpoints.end(), std::default_random_engine(seed));
	shuffle(outerEndpoints.begin(), outerEndpoints.end(), std::default_random_engine(seed));

	int index = finalEndpoints.size();

	finalEndpoints.push_back(vector<Node*>());

	finalEndpoints[index].resize(0);

	for (int i = 0; i < ((innerEndpoints.size() > layer.numEndpoints) ? layer.numEndpoints : innerEndpoints.size()); i++) {
		finalEndpoints[index].push_back(innerEndpoints[i]);
	}

	for (int i = 0; i < ((outerEndpoints.size() > layer.numOuterEndpoints) ? layer.numOuterEndpoints : outerEndpoints.size()); i++) {
		finalEndpoints[index].push_back(outerEndpoints[i]);
	}

	endpoints.clear();
}

Net::Net(TreeModel model) :
	dimensions(model.dimensions),
	minDist(model.minDist),
	trunkHeight(model.trunkHeight)
{
	srand(time(NULL));

	// Placing points
	buildNet();

	branchLayers = model.layers;

	// Building trunk, branches and twigs according to the model's layers
	for (BranchLayer& layer : model.layers) {
		computeLayer(layer);
	}
}

void Net::computeLayer(BranchLayer const& layer) {
	makeBranches(layer.endpointCandidateSelector, layer.alpha, layer.precision);

	switch (layer.type) {
	case REGULAR:
		selectEndpoints(layer);
		break;
	case TWIG:
		selectTwigEndpoints(layer);
		break;
	case LEAF:
		selectLeafEndpoints(layer);
		break;
	default:
		cout << "Erreur : type de layer non supporté";
		endpoints.clear();
		break;
	}

	resetNonGuide();
}

vector<Branch> Net::getTree() {
	vector<Branch> branchList;

	for (int i = 0; i < finalEndpoints.size(); i++) {
		cout << "Nombre de branches : " << finalEndpoints[i].size() << endl;

		vector<Node*> fe = finalEndpoints[i];

		BranchLayer& bl = branchLayers[i];

		double maxStartWidth = bl.startWidth;
		double dec = bl.dec;
		double ratio = bl.ratio;
		double smoothedPrecision = bl.smoothedPrecision;

		double startWidth = maxStartWidth;
		for (Node* n : fe) {
			stack<Node*> branch_stack;
			int num_nodes = 0;
			Branch branch;
			branch.ID = branchList.size();
			branch.precision = smoothedPrecision;

			if (bl.type == LEAF)
				branch.type = BRANCH_LEAF;

			Node* c = n;
			while (c->parent != NULL && c->parent->branchLevel == -1) {
				c->branchID = branch.ID;
				c->branchLevel = i;

				branch_stack.push(c);

				c = c->parent;
			}
			if (c->parent == NULL) {
				root = c;
			}
			else {
				branch.parentBranchID = c->parent->branchIndex;
				branch.parentBranchRootIndex = c->parent->branchIndex;

				branchList[c->parent->branchID].childrenIDs.push_back(branch.ID);

				if (c->parent->branchLevel == i) {
					startWidth = branchList[c->parent->branchID].nodes[c->parent->branchIndex].width;
				}
				else {
					startWidth = branchList[c->parent->branchID].nodes[c->parent->branchIndex].width * ratio;
				}

				if (startWidth > maxStartWidth)
					startWidth = maxStartWidth;

				TreeNode root_node;
				root_node.pos = c->parent->getPos();
				root_node.width = startWidth;

				branch.nodes.push_back(root_node);
			}
			// Compute last node
			{
				c->branchID = branch.ID;
				c->branchLevel = i;

				branch_stack.push(c);
			}

			double incWidth = (dec - 1) * startWidth / branch_stack.size();
			double width = startWidth;

			while (!branch_stack.empty()) {
				Node* n = branch_stack.top();
				branch_stack.pop();

				TreeNode new_node;

				new_node.pos = n->getPos();
				new_node.width = width;

				n->branchIndex = branch.nodes.size();

				branch.nodes.push_back(new_node);

				width += incWidth;
			}

			branchList.push_back(branch);
		}
	}

	return branchList;
}