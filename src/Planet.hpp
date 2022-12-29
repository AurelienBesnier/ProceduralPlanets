#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>
#include <QVector3D>
#include <QVector2D>
#include <chrono>
#include <set>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>

#include "Plate.hpp"
#include "Mesh.hpp"

typedef CGAL::Simple_cartesian<double>                  K;
typedef K::Point_3                                      Point;
typedef CGAL::Point_set_3<Point>                        Point_set;



class Planet{
private:
	unsigned int plateNum;
	PlateParameters plateParams;
	double radius;
	int elems;

    QVector<Vertex> vertices;
    QVector<unsigned int> indices;
    QVector<QVector3D> pos;
    std::vector<Plate> plates;
    QVector<QVector<unsigned int> >  one_ring;
    bool buffersCreated = false;

    void triangulate();
    void drawPlanet(const qglviewer::Camera *camera);

public:
    GLuint programID, VAO, VBO, EBO;
    GLuint vShader, fShader;
    bool planetCreated=false;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;

    Planet (){}
    Planet (QOpenGLContext *context);
	~Planet ();

    void init ();
    void initGLSL();
    void makeSphere (float radius);
    void makePlates ();
    void initElevations();
    void initPlanet ();
    void createBuffers();

    void draw (const qglviewer::Camera *camera);

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

    bool printShaderErrors (GLuint shader);
    bool printProgramErrors (int program);
    bool checkOpenGLError ();
    std::string readShaderSource (std::string filename);

    static void /*GLAPIENTRY */MessageCallback (GLenum source, GLenum type,
                                                GLuint id, GLenum severity,
                                                GLsizei length,
                                                const GLchar *message,
                                                const void *userParam);
};

#endif // PLANET_H
