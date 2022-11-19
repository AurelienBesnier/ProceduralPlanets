#ifndef PLANET_H
#define PLANET_H

#include "Vec3D.hpp"

#include <QString>
#include <QColor>
#include <QVector>
#include <QGLViewer/vec.h>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>

class Planet
{
private :
    unsigned int plateNum;

    GLuint bufferId;

    bool planetCreated;
public:


    Planet(){}
    Planet( QOpenGLContext* context );
    ~Planet();

    void init();
    void initGLSL();

    void initPlanet();

    void draw( const qglviewer::Camera * camera );
    void clear();

    void setPlateNumber(int _plateNum);

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
