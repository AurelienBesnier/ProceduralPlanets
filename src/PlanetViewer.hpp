#ifndef PLANETVIEWER_H
#define PLANETVIEWER_H

#include <QKeyEvent>
#include <QGLViewer/qglviewer.h>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include "Planet.hpp"


enum DisplayMode{WIRE=0, SOLID=1};

class PlanetViewer : public QGLViewer {
Q_OBJECT

public:
    PlanetViewer (QWidget *parent);

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
protected:
    Planet planet;
    QOpenGLContext *glContext;
    QOpenGLExtraFunctions *glFunctions;
    QOpenGLShaderProgram shader;
    qglviewer::Vec cam;

    DisplayMode displayMode;

    virtual void draw ();
    virtual void shaderLighting ();
    virtual void init ();
    virtual QString helpString () const;
    virtual void keyPressEvent (QKeyEvent *e);

    void changeDisplayMode(){
        if(displayMode == WIRE)
            displayMode = SOLID;
        else if(displayMode == SOLID)
            displayMode = WIRE;
    }

    void clear ();
    void updateCamera (const qglviewer::Vec &center);

public slots:
	void setPlateNumber (int _plateNum);
	void generatePlanet ();
	void clearPlanet ();
	void setPlanetRadius (QString _r);
	void setPlanetElem (QString _elems);
	void savePlanetOff () const;
	void savePlanetObj () const;
	void setOceanicThickness (QString _t);
	void setOceanicElevation (QString _e);
	void setContinentThickness (QString _t);
	void setContinentElevation (QString _e);
signals:

};

#endif
