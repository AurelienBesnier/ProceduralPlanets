#ifndef PLATE_HPP_
#define PLATE_HPP_

enum PlateType {
	OCEANIC, CONTINENTAL
};

struct Plate {
	PlateType type;
	std::vector<unsigned int> points;
	qglviewer::Vec mouvement;
	double e;
	double z;
};

struct PlateParameters {
	double oceanicThickness, continentalThickness, oceanicEleavation,
			continentalElevation;
};

#endif /* PLATE_HPP_ */
