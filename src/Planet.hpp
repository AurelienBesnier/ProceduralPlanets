#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>
#include <QVector3D>
#include <QVector2D>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include "Plate.hpp"

typedef CGAL::Exact_predicates_inexact_constructions_kernel          K;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned int, K> Vb;
typedef CGAL::Delaunay_triangulation_cell_base_3<K>                  Cb;
typedef CGAL::Triangulation_data_structure_3<Vb, Cb>                 Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location>  Delaunay;
typedef Delaunay::Vertex_handle                                      Vertex_handle;
typedef Delaunay::Point                                              Point;
typedef Delaunay::Facet                                              Facet;
typedef Delaunay::Cell                                               Cell;
typedef Delaunay::Finite_facets_iterator                             Finite_facets_iterator;
typedef Delaunay::Finite_cells_iterator                              Finite_cells_iterator;


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
    std::vector<std::pair<Point,unsigned int>> pos;
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
