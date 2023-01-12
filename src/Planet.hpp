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
#include "Noise.hpp"

typedef CGAL::Simple_cartesian<double>                  K;
typedef K::Point_3                                      Point;
typedef CGAL::Point_set_3<Point>                        Point_set;

/**
 * @brief Planet Class.
 * Class reprensenting a planet.
 */
class Planet{
private:
	unsigned int plateNum;
	double radius;
	int elems;
    SimplexNoise noise;
    unsigned int octaveOcean, octaveContinent;

    std::vector<QVector3D> pos;
    std::vector<std::vector<unsigned int> >  one_ring;

    /**
     * @brief triangulation method.
     * 
     */
    void triangulate();
    /**
     * @brief method to draw the planet surface.
     * 
     * @param camera  
     */
    void drawPlanet(const qglviewer::Camera *camera);

    /**
     * @brief method to draw the ocean
     * 
     * @param camera 
     */
    void drawOcean(const qglviewer::Camera *camera);

public:
    std::vector<Plate> plates;
    float selectedPlateID = -1;
    Mesh mesh, oceanMesh;
    bool shaderLighting = false, needInitBuffers = true, oceanDraw = true, textures = false, needBuffersUpdate = false;
    PlateParameters plateParams;
    QOpenGLShaderProgram *program=nullptr, *oceanProgram = nullptr;
    GLuint programID, oceanProgramID;
    bool planetCreated=false;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;

    Planet (){}
    Planet (QOpenGLContext *context);
	~Planet ();

    /**
     * @brief Initialisation method
     * This method setup the variables of the class to a default values.
     */
    void init ();

    /**
     * @brief Initilisation method of the OpenGL context.
     * This method initialise the OpenGL context with default values.
     */
    void initGLSL();

    /**
     * @brief Method to initialize points of the sphere. 
     * This method samples the points of a fibonacci sphere to construct the mesh.
     */
    void makeSphere ();

    /**
     * @brief Method to build the mesh of the ocean.
     * 
     */
    void makeOcean ();

    /**
     * @brief Method initilizing the plates
     * This method segments the mesh in different regions.
     */
    void makePlates ();

    /**
     * @brief Method to initialize the elevation of the points of the mesh
     * 
     */
    void initElevations();

    /**
     * @brief Method that reset the elevations of the mesh.
     * 
     */
    void resetHeights();

    /**
     * @brief Method to resegement the mesh.
     * 
     */
    void resegment();
    
    /**
     * @brief Method to initialize the planet. 
     * 
     */
    void initPlanet ();

    /**
     * @brief Method to get the one_ring of each vertices of the mesh.
     * 
     * @param i_vertices 
     * @param i_triangles 
     * @param o_one_ring 
     */
    void collect_one_ring (std::vector<QVector3D> const & i_vertices,
    std::vector< unsigned int > const & i_triangles,
    std::vector<std::vector<unsigned int> > & o_one_ring);

    /**
    * @brief Method to draw the whole planet.
    * 
    * @param camera 
    */
    void draw (const qglviewer::Camera *camera);

    /**
    * @brief Method to clear the planet.
    * 
    */
	void clear ();

    /**
     * @brief Method to save the mesh as an .obj file.
     * 
     */
	void save () const;

    /**
     * @brief Method to save the mesh as an .off file.
     * 
     */
	void saveOFF () const;

    /**
     * @brief Set the Plate Number object
     * 
     * @param _plateNum 
     */
	void setPlateNumber (int _plateNum);

    /**
     * @brief Set the Radius object
     * 
     * @param _r 
     */
	void setRadius (double _r);

    /**
     * @brief Get the Radius object
     * 
     * @return double 
     */
    double getRadius () const;

    /**
     * @brief Set the Elems object
     * 
     * @param _elems 
     */
    void setElems (int _elems);

    /**
     * @brief Set the Oceanic Elevation object
     * 
     * @param _e 
     */
	void setOceanicElevation (double _e);

    /**
     * @brief Set the Continental Elevation object
     * 
     * @param _e 
     */
	void setContinentalElevation (double _e);

    /**
     * @brief Set the Oceanic Octave object
     * 
     * @param _o 
     */
    void setOceanicOctave(int _o);

    /**
     * @brief Set the Continental Octave object
     * 
     * @param _o 
     */
    void setContinentalOctave(int _o);

    /**
     * @brief 
     * 
     */
    void reelevateOcean();

    /**
     * @brief 
     * 
     */
    void reelevateContinent();

    /**
    * @brief 
    * 
    */
    void move();

    /**
     * @brief 
     * 
     * @param point 
     * @param mesh 
     */
    void closestPoint(QVector3D point);

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
