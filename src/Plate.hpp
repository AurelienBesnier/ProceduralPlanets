#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <Eigen/Dense>

using Eigen::Vector3d;

enum PlateType {
	OCEANIC, CONTINENTAL
};

struct Plate {
	PlateType type;
	std::vector<unsigned int> points;
	Vector3d mouvement;
	double e;
	double z;
};

struct PlateParameters {
	double oceanicThickness, continentalThickness, oceanicEleavation,
			continentalElevation;
};

#endif /* PLATE_HPP_ */
