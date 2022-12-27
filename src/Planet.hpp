#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>
#include <QVector3D>
#include <QVector2D>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>

#include "Plate.hpp"
#include "Mesh.hpp"
#include "Shader.h"

typedef CGAL::Simple_cartesian<double>                  K;
typedef K::Point_3                                      Point;
typedef CGAL::Point_set_3<Point>                        Point_set;

class Planet {
private:
	unsigned int plateNum;
	PlateParameters plateParams;
	double radius;
	int elems;

    Mesh mesh;
    std::vector<QVector3D> pos;
    std::vector<Plate> plates;
    std::vector<std::vector<unsigned int> >  one_ring;

	bool wireframe = false;
    bool shaderLight = false;
    void triangulate();
    void drawPlanet(const qglviewer::Camera *camera);

public:
    Shader shader;
    bool planetCreated;

    Planet ()
	{
	}
	Planet (QOpenGLContext *context);
	~Planet ();

	void init ();
    void initGLSL ();
    void makeSphere (float radius, int elems);
    void makePlates ();
    void initElevations();
    void initPlanet ();

    void draw (const qglviewer::Camera *camera);

	void changeViewMode ();
    void shaderLighting ();
	void clear ();
	void save () const;
	void saveOFF () const;

	void setPlateNumber (int _plateNum);
	void setRadius (double _r);
    double getRadius () const;
	void setElems (int _elems);

	void setOceanicThickness (double _t);
	void setOceanicElevation (double _e);
	void setContinentalThickness (double _t);
	void setContinentalElevation (double _e);

    QOpenGLContext *glContext;
    QOpenGLExtraFunctions *glFunctions;
};

#endif // PLANET_H
