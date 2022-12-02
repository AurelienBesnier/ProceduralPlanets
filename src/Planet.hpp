#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>
#include <Eigen/Dense>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/draw_triangulation_3.h>
#include "Plate.hpp"


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_3<K>      Triangulation;
typedef Triangulation::Cell_handle    Cell_handle;
typedef Triangulation::Vertex_handle  Vertex_handle;
typedef Triangulation::Locate_type    Locate_type;
typedef Triangulation::Point          Point;

using Eigen::Vector2d;

struct Vertex {
	Point pos;
	Point normal;
	Vector2d texCoord;
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
	std::vector<int> indices;

	bool wireframe = false;

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
	void makeSphere (float radius, int slices, int stacks);
	void makePlates ();
	void initPlanet ();
    void triangulate();
	void createBuffers ();

	void draw (const qglviewer::Camera *camera);
	void changeViewMode ();
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
