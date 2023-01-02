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
	double radius;
	int elems;

    std::vector<QVector3D> pos;
    std::vector<Plate> plates;
    std::vector<std::vector<unsigned int> >  one_ring;
    bool needInitBuffers = true;

    void triangulate();
    void drawPlanet(const qglviewer::Camera *camera);
    void drawOcean(const qglviewer::Camera *camera);

public:
    Mesh mesh, oceanMesh;
    bool shaderLighting = false;
    bool oceanDraw = true;
    PlateParameters plateParams;
    QOpenGLShaderProgram *program=nullptr, *oceanProgram = nullptr;
    GLuint programID, oceanProgramID;
    bool planetCreated=false;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;

    Planet (){}
    Planet (QOpenGLContext *context);
	~Planet ();

    void init ();
    void initGLSL();
    void makeSphere ();
    void makeOcean ();
    void makePlates ();
    void initElevations();
    void initPlanet ();
    void collect_one_ring (std::vector<QVector3D> const & i_vertices,
    std::vector< unsigned int > const & i_triangles,
    std::vector<std::vector<unsigned int> > & o_one_ring);

    void draw (const qglviewer::Camera *camera);

	void clear ();
	void save () const;
	void saveOFF () const;

	void setPlateNumber (int _plateNum);
	void setRadius (double _r);
    double getRadius () const;
    void setElems (int _elems);

	void setOceanicElevation (double _e);
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
