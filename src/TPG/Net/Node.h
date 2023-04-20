#pragma once

#ifndef NODE
#define NODE

#include "TPG/Utils/Utils.h"
#include "TPG/TreeGenerator/Branch.h"

struct Node {

	static struct NodePtr {
		int pos;
		NodePtr() : pos(0) {}
		Node* ptr;
		Node& operator*() {
			return (*ptr);
		}
		NodePtr& operator=(NodePtr& n) {
			ptr = n.ptr;
			ptr->nodePtr = this;

			return *this;
		}
	};

	// Attributes 
	double x, y, z;
	double dist;
	cgp::vec3 guide;
	Node* parent;
	vector<Node*> neigh;
	vector<double> weigh;

	vector<int> precisionLevels;

	Node() : x(0), y(0), z(0), neigh(0), weigh(0), dist(DBL_MAX), branchLevel(-1) {
		clear();
	}
	Node(const Node& n) : x(n.x), y(n.y), z(n.z), dist(DBL_MAX), branchLevel(-1) {
		clear();
	}
	Node(const vec3& p) : x(p.x), y(p.y), z(p.z), neigh(0), weigh(0), dist(DBL_MAX), branchLevel(-1) {
		clear();
	}

	// Supp
	unsigned lvl;
	unsigned hopCount;
	bool inTree;
	NodePtr* nodePtr;
	
	// Branch attributes
	int branchLevel;
	int branchIndex;
	int branchID;

	// For the heap
	bool operator<(Node& n) {
		return dist > n.dist;
	}

	// Convert pos to vec3
	vec3 getPos() const {
		return vec3(x, y, z);
	}

	// Update weigh from new guide
	void updateWeigh();

	// Clear
	void clear() {
		dist = DBL_MAX;
		parent = NULL;
		inTree = false;
		hopCount = 0;
	}

	double distTo(Node& n) {
		return sqrt((x - n.x) * (x - n.x) + (y - n.y) * (y - n.y) + (z - n.z) * (z - n.z));
	}

	double distTo(vec3 n) {
		return sqrt((x - n.x) * (x - n.x) + (y - n.y) * (y - n.y) + (z - n.z) * (z - n.z));
	}
};

int getDir(Node&, Node&);

bool nodecmp(Node::NodePtr n1, Node::NodePtr n2);

#endif