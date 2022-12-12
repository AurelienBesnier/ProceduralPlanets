#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>
#include <QVector3D>
#include <QVector2D>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include "Plate.hpp"

typedef CGAL::Exact_predicates_inexact_constructions_kernel     K;
typedef K::Point_3                                              Point;
typedef CGAL::Point_set_3<Point>                                Point_set;


struct Vertex {
    Point pos;
    QVector3D normal;
    QVector2D texCoord;
    QVector3D color;
    unsigned int plate_id;
};

class Planet {
private:
	unsigned int plateNum;
	PlateParameters plateParams;
	double radius;
	int elems;

	std::vector<Vertex> vertices;
    std::vector<Point> pos;
	std::vector<Plate> plates;
    std::vector<unsigned int> indices;

	bool wireframe = false;
    bool shaderLigth = false;
    void triangulate();

public:
	GLuint VBO, VAO, EBO;
	bool planetCreated;
	bool buffersCreated = false;

	Planet () // @suppress("Class members should be properly initialized")
	{
	}
	Planet (QOpenGLContext *context);
	~Planet ();

	void init ();
    void initGLSL ();
    void makeSphere (float radius, int elems);
	void makePlates ();
    void initPlanet ();
	void createBuffers ();

	void draw (const qglviewer::Camera *camera);
	void changeViewMode ();
    void shaderLighting ();
	void clear ();
	void save () const;
	void saveOFF () const;

	void setPlateNumber (int _plateNum);
	void setRadius (double _r);
	double getRadius ()const;
	void setElems (int _elems);

	void setOceanicThickness (double _t);
	void setOceanicElevation (double _e);
	void setContinentalThickness (double _t);
	void setContinentalElevation (double _e);

	bool printShaderErrors (GLuint shader);
	bool printProgramErrors (int program);
	bool checkOpenGLError ();
	std::string readShaderSource (std::string filename);

	GLuint vShader, gShader, fShader, programID;
	QOpenGLContext *glContext;
	QOpenGLExtraFunctions *glFunctions;

	static void /*GLAPIENTRY */MessageCallback (GLenum source, GLenum type,
												GLuint id, GLenum severity,
												GLsizei length,
												const GLchar *message,
												const void *userParam);
};

#endif // PLANET_H
