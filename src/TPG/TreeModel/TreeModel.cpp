#include "TreeModel.h"

TreeModel::TreeModel(TreeType type) {
	switch (type) {
	case TREE_BASIC:
		makeBasic();
		break;
	case TREE_SEQUOIA:
		makeSequoia();
		break;
	case TREE_MOTHER:
		makeMother();
		break;
	}
}

void TreeModel::makeBasic() {

	double height = 40;

	dimensions.x = height / 5;
	dimensions.y = height * 4 / 5;
	dimensions.z = height / 5;

	minDist = height / 20;
	trunkHeight = height / 5;

	layers.resize(5);

	// Trunk
	layers[0].numEndpoints = 1;
	layers[0].startWidth = dimensions.x / 8;
	layers[0].dec = 0.03;
	layers[0].precision = 3;

	layers[0].endpointCandidateSelector = [&](SelectorData d) -> bool {
		return d.top;
	};

	layers[0].alpha = [&](unsigned hc) -> double {
		return 0.;
	};

	// Branches
	layers[1].numEndpoints = 20;
	layers[1].startWidth = dimensions.x / 30;
	layers[1].ratio = 0.4;
	layers[1].dec = 0.3;
	layers[1].precision = 3;

	layers[1].endpointCandidateSelector = [&](SelectorData d) -> bool {
		float y_center = dimensions.y / 2. + trunkHeight;
		float y_radius = dimensions.y / 2.;

		double ellipseEval = (d.pos.x * d.pos.x) / (dimensions.x * dimensions.x)
			+ (d.pos.z * d.pos.z) / (dimensions.z * dimensions.z)
			+ (d.pos.y - y_center) * (d.pos.y - y_center) / (y_radius * y_radius);

		if (ellipseEval <= 1
			&& ellipseEval > 0.8
			&& d.pos.y > y_center) {
			return true;
		}

		return false;
	};

	layers[1].alpha = [&](unsigned hc) -> double {
		if (hc == 0) {
			return Pi / 2.2;
		}
		else {
			return 0;
		}
	};

	layers[2].numEndpoints = 100;
	layers[2].ratio = 0.8;
	layers[2].startWidth = dimensions.x / 40;
	layers[2].dec = 0.4;
	layers[2].precision = 2;

	layers[2].endpointCandidateSelector = layers[1].endpointCandidateSelector;
	layers[2].alpha = layers[1].alpha;

	//Twigs
	layers[3].numEndpoints = 100;
	layers[3].numOuterEndpoints = 1000;
	layers[3].startWidth = dimensions.x / 300;
	layers[3].ratio = 0.2;
	layers[3].dec = 0.7;
	layers[3].type = TWIG;
	layers[3].precision = 0;

	layers[3].endpointCandidateSelector = [&](SelectorData d) -> bool {
		float y_center = dimensions.y / 2. + trunkHeight;
		float y_radius = dimensions.y / 2.;

		if ((d.pos.x * d.pos.x) / (dimensions.x * dimensions.x)
			+ (d.pos.z * d.pos.z) / (dimensions.z * dimensions.z)
			+ (d.pos.y - y_center) * (d.pos.y - y_center) / (y_radius * y_radius) <= 1
			&& d.hopCount == 2
			&& d.pos.y > y_center) {
			return true;
		}

		return false;
	};

	layers[3].alpha = [&](unsigned hc) -> double {
		if (hc == 1) {
			return Pi / 3;
		}
		else {
			return 0;
		}
	};

	//Twigs
	layers[4].numEndpoints = 1000;
	layers[4].numOuterEndpoints = 1000;
	layers[4].type = LEAF;

	layers[4].endpointCandidateSelector = [&](SelectorData d) -> bool {
		return d.hopCount == 1;
	};

	layers[4].alpha = [&](unsigned hc) -> double {
		if (hc == 1) {
			return Pi / 3;
		}
		else {
			return 0;
		}
	};
}

void TreeModel::makeSequoia() {

	double height = 40;

	dimensions.x = height / 5;
	dimensions.y = height * 4 / 5;
	dimensions.z = height / 5;

	minDist = height / 200;
	trunkHeight = height / 5;

	layers.resize(5);

	// Trunk
	layers[0].numEndpoints = 1;
	layers[0].startWidth = dimensions.x / 8;
	layers[0].dec = 0.03;
	layers[0].precision = 3;

	layers[0].endpointCandidateSelector = [&](SelectorData d) -> bool {
		return d.top;
	};

	layers[0].alpha = [&](unsigned hc) -> double {
		return 0.;
	};

	// Branches
	layers[1].numEndpoints = 20;
	layers[1].startWidth = dimensions.x / 30;
	layers[1].ratio = 0.4;
	layers[1].dec = 0.3;
	layers[1].precision = 3;

	layers[1].endpointCandidateSelector = [&](SelectorData d) -> bool {
		float y_center = dimensions.y / 2. + trunkHeight;
		float y_radius = dimensions.y / 2.;

		double ellipseEval = (d.pos.x * d.pos.x) / (dimensions.x * dimensions.x)
			+ (d.pos.z * d.pos.z) / (dimensions.z * dimensions.z)
			+ (d.pos.y - y_center) * (d.pos.y - y_center) / (y_radius * y_radius);

		if (ellipseEval <= 1
			&& ellipseEval > 0.8
			&& d.pos.y > y_center) {
			return true;
		}

		return false;
	};

	layers[1].alpha = [&](unsigned hc) -> double {
		if (hc == 0) {
			return Pi / 2.2;
		}
		else {
			return 0;
		}
	};

	layers[2].numEndpoints = 100;
	layers[2].ratio = 0.8;
	layers[2].startWidth = dimensions.x / 40;
	layers[2].dec = 0.4;
	layers[2].precision = 2;

	layers[2].endpointCandidateSelector = layers[1].endpointCandidateSelector;
	layers[2].alpha = layers[1].alpha;

	//Twigs
	layers[3].numEndpoints = 100;
	layers[3].numOuterEndpoints = 1000;
	layers[3].startWidth = dimensions.x / 300;
	layers[3].ratio = 0.2;
	layers[3].dec = 0.7;
	layers[3].type = TWIG;
	layers[3].precision = 0;

	layers[3].endpointCandidateSelector = [&](SelectorData d) -> bool {
		float y_center = dimensions.y / 2. + trunkHeight;
		float y_radius = dimensions.y / 2.;

		if ((d.pos.x * d.pos.x) / (dimensions.x * dimensions.x)
			+ (d.pos.z * d.pos.z) / (dimensions.z * dimensions.z)
			+ (d.pos.y - y_center) * (d.pos.y - y_center) / (y_radius * y_radius) <= 1
			&& d.hopCount == 2
			&& d.pos.y > y_center) {
			return true;
		}

		return false;
	};

	layers[3].alpha = [&](unsigned hc) -> double {
		if (hc == 1) {
			return Pi / 3;
		}
		else {
			return 0;
		}
	};

	//Twigs
	layers[4].numEndpoints = 1000;
	layers[4].numOuterEndpoints = 1000;
	layers[4].type = LEAF;

	layers[4].endpointCandidateSelector = [&](SelectorData d) -> bool {
		return d.hopCount == 1;
	};

	layers[4].alpha = [&](unsigned hc) -> double {
		if (hc == 1) {
			return Pi / 3;
		}
		else {
			return 0;
		}
	};
}

void TreeModel::makeMother() {

	double height = 40;

	dimensions.x = height;
	dimensions.y = height * 4 / 5;
	dimensions.z = height;

	minDist = height / 80;
	trunkHeight = height / 5;

	layers.resize(5);

	// Trunk
	layers[0].numEndpoints = 1;
	layers[0].startWidth = dimensions.x / 16;
	layers[0].dec = 0.03;
	layers[0].precision = 3;

	layers[0].endpointCandidateSelector = [&](SelectorData d) -> bool {
		return d.top;
	};

	layers[0].alpha = [&](unsigned hc) -> double {
		return 0.;
	};

	// Branches
	layers[1].numEndpoints = 20;
	layers[1].startWidth = dimensions.x / 60;
	layers[1].ratio = 0.4;
	layers[1].dec = 0.3;
	layers[1].precision = 3;

	layers[1].endpointCandidateSelector = [&](SelectorData d) -> bool {
		float y_center = dimensions.y / 2. + trunkHeight;
		float y_radius = dimensions.y / 2.;

		double ellipseEval = (d.pos.x * d.pos.x) / (dimensions.x * dimensions.x)
			+ (d.pos.z * d.pos.z) / (dimensions.z * dimensions.z)
			+ (d.pos.y - y_center) * (d.pos.y - y_center) / (y_radius * y_radius);

		if (ellipseEval <= 1
			&& ellipseEval > 0.8
			&& d.pos.y > y_center) {
			return true;
		}

		return false;
	};

	layers[1].alpha = [&](unsigned hc) -> double {
		if (hc == 0) {
			return Pi / 2.2;
		}
		else {
			return 0;
		}
	};

	layers[2].numEndpoints = 100;
	layers[2].ratio = 0.8;
	layers[2].startWidth = dimensions.x / 40;
	layers[2].dec = 0.4;
	layers[2].precision = 2;

	layers[2].endpointCandidateSelector = layers[1].endpointCandidateSelector;
	layers[2].alpha = layers[1].alpha;

	//Twigs
	layers[3].numEndpoints = 100;
	layers[3].numOuterEndpoints = 1000;
	layers[3].startWidth = dimensions.x / 300;
	layers[3].ratio = 0.2;
	layers[3].dec = 0.7;
	layers[3].type = TWIG;
	layers[3].precision = 0;

	layers[3].endpointCandidateSelector = [&](SelectorData d) -> bool {
		float y_center = dimensions.y / 2. + trunkHeight;
		float y_radius = dimensions.y / 2.;

		if ((d.pos.x * d.pos.x) / (dimensions.x * dimensions.x)
			+ (d.pos.z * d.pos.z) / (dimensions.z * dimensions.z)
			+ (d.pos.y - y_center) * (d.pos.y - y_center) / (y_radius * y_radius) <= 1
			&& d.hopCount == 2
			&& d.pos.y > y_center) {
			return true;
		}

		return false;
	};

	layers[3].alpha = [&](unsigned hc) -> double {
		if (hc == 1) {
			return Pi / 3;
		}
		else {
			return 0;
		}
	};

	//Twigs
	layers[4].numEndpoints = 1000;
	layers[4].numOuterEndpoints = 1000;
	layers[4].type = LEAF;

	layers[4].endpointCandidateSelector = [&](SelectorData d) -> bool {
		return d.hopCount == 1;
	};

	layers[4].alpha = [&](unsigned hc) -> double {
		if (hc == 1) {
			return Pi / 3;
		}
		else {
			return 0;
		}
	};
}