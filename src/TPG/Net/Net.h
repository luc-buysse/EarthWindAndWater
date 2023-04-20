#pragma once

#ifndef NET
#define NET

#include <vector>
#include "TPG/Utils/Utils.h"
#include "TPG/Net/Node.h"
#include "TPG/TreeModel/TreeModel.h"

using namespace std;

static class DatasetAdaptor {
public:
	vector<Node>& m_Nodes;

	DatasetAdaptor(vector<Node>& Nodes) : m_Nodes(Nodes) {};

	size_t kdtree_get_point_count() const {
		return m_Nodes.size();
	}

	double kdtree_get_pt(const uint32_t idx, const size_t dim) const {
		return m_Nodes[idx].getPos()[dim];
	}

	template <class BBOX>
	bool kdtree_get_bbox(BBOX&) const
	{
		return false;
	}
};

struct Net {
private:
	vector<Node> m_Nodes;
	vec3 dimensions;
	double trunkHeight;

	double minDist;

	Node* root;
	Node* top;
	Node* middle;

	void buildNet();
	void buildPoissonDisk();
	void makeYao();
	void makeUnlimitedNeigh();

	vector<Node*> endpoints;
	vector<vector<Node*>> finalEndpoints;
	vector<BranchLayer> branchLayers;

	void computeLayer(BranchLayer const& layer);

	void makeBranches(std::function<bool(SelectorData)>, std::function<double(unsigned)>, int precision);

	void resetNonGuide();
	void selectEndpoints(BranchLayer);
	void selectTwigEndpoints(BranchLayer);
	void selectLeafEndpoints(BranchLayer);

	using KdTreeType = nanoflann::KDTreeSingleIndexAdaptor<
		nanoflann::L2_Simple_Adaptor<double, Net>,
		Net,
		3>;

public:

	size_t kdtree_get_point_count() const {
		return m_Nodes.size();
	}

	double kdtree_get_pt(const uint32_t idx, const size_t dim) const {
		return m_Nodes[idx].getPos()[dim];
	}

	template <class BBOX>
	bool kdtree_get_bbox(BBOX&) const
	{
		return false;
	}

	Net(TreeModel);

	vector<Branch> getTree();

	void debug(mesh& res);
};

#endif