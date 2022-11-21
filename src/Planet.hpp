#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QColor>
#include <QVector>
#include <QGLViewer/vec.h>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>

enum PlateType {
    OCEANIC,
    CONTINENTAL
};

struct Plate
{
    PlateType type;
    std::vector<unsigned int> points;
    qglviewer::Vec mouvement;
    double e;
    double z;
};

class Planet
{
private :
    unsigned int plateNum;
    double radius;
    int elems;

    GLuint VBO,VAO,EBO;

    std::vector<qglviewer::Vec> positions;
    std::vector<Plate> plates;
    std::vector<unsigned int> indices;

public:
    bool planetCreated;

    Planet(){}
    Planet( QOpenGLContext* context );
    ~Planet();

    void init();
    void initGLSL();
    void makeSphere(float radius,int slices,int stacks);
    void makePlates();
    void initPlanet();

    void draw( const qglviewer::Camera * camera );
    void clear();

    void setPlateNumber(int _plateNum);
    void setRadius(double _r);
    void setElems(int _elems);

    bool printShaderErrors(GLuint shader);
    bool printProgramErrors(int program);
    bool checkOpenGLError();
    std::string readShaderSource(std::string filename);

    GLuint vShader, gShader, fShader, programID;
    QOpenGLContext* glContext;
    QOpenGLExtraFunctions* glFunctions;

    static void /*GLAPIENTRY */MessageCallback( GLenum source, GLenum type,
                                                GLuint id, GLenum severity,
                                                GLsizei length, const GLchar* message,
                                                const void* userParam );
};

#endif // PLANET_H
