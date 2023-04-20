#include "Node.h"

void Node::updateWeigh() {
	weigh.resize(neigh.size());

	for (int i = 0; i < weigh.size(); i++) {
		vec3 v = normalize(neigh[i]->getPos() - getPos());
		if (norm(guide) > 0)
			guide = normalize(guide);
		weigh[i] = 1 - dot(v, guide);
	}
}

int getDir(Node& n1, Node& n2) {
	int res = 0;
	res += 4 * (n1.x >= n2.x);
	res += 2 * (n1.y >= n2.y);
	res += 1 * (n1.z >= n2.z);

	return res;
}

bool nodecmp(Node::NodePtr n1, Node::NodePtr n2) {
	return (*n1).dist > (*n2).dist;
}