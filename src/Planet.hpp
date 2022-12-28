#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>
#include <QVector3D>
#include <QVector2D>
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

    QVector<QVector3D> pos;
    std::vector<Plate> plates;
    QVector<QVector<unsigned int> >  one_ring;
    bool needInitBuffers = true;

    void triangulate();
    void drawPlanet(const qglviewer::Camera *camera);

public:
    Mesh mesh;
    QOpenGLShaderProgram *program=nullptr;
    GLuint programID;
    bool planetCreated=false;

    Planet (){}
    Planet (QOpenGLContext *context);
	~Planet ();

    void init ();
    void initGLSL();
    void makeSphere (float radius);
    void makePlates ();
    void initElevations();
    void initPlanet ();

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
    static void /*GLAPIENTRY */ MessageCallback (GLenum source, GLenum type,
                          GLuint id, GLenum severity,
                          GLsizei length,
                          const GLchar *message,
                          const void *userParam)
    {
        if (severity == GL_DEBUG_SEVERITY_HIGH
              || severity == GL_DEBUG_SEVERITY_MEDIUM
              || severity == GL_DEBUG_SEVERITY_LOW)
        {
            std::string s_severity = (
                  severity == GL_DEBUG_SEVERITY_HIGH ? "High" :
                  severity == GL_DEBUG_SEVERITY_MEDIUM ? "Medium" : "Low");
            std::cerr << "Error " << id <<", Source: "<<source<<",  Type: "<< type << ", Length: "<<length<<" [severity=" << s_severity << "]: "
              << message << std::endl;
        }
    }
};

#endif // PLANET_H
