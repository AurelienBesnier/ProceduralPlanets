#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <QVector3D>

enum PlateType {
	OCEANIC, CONTINENTAL
};

struct Plate {
	PlateType type;
	std::vector<unsigned int> points;
    QVector3D mouvement;
	double e;
};

struct PlateParameters {
	double oceanicElevation, continentalElevation;
};

#endif /* PLATE_HPP_ */
