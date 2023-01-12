#ifndef PLANETVIEWER_H
#define PLANETVIEWER_H

#include <QKeyEvent>
#include <QGLViewer/qglviewer.h>
#include <QtConcurrent>
#include <QFuture>
#include <QThread>
#include <QListWidgetItem>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Planet.hpp"
#include "Mesh.hpp"

enum DisplayMode{WIRE=0, SOLID=1};

class PlanetViewer : public QGLViewer {
Q_OBJECT

friend class PlanetDockWidget;

public:
    PlanetViewer (QWidget *parent);
protected:
    Planet planet;
    bool planetCreated = false;
	QFuture<void> generationFuture;

    unsigned int timeStep = 1;

    QOpenGLVertexArrayObject* skyboxVAO;
    QOpenGLBuffer *skyboxVBO;
    GLuint skyboxTextureID;
    QOpenGLShaderProgram *skyboxShader=nullptr;
    std::vector<std::string> skyboxFaces{
        "./res/skybox/right.jpg",
        "./res/skybox/left.jpg",
        "./res/skybox/top.jpg",
        "./res/skybox/bottom.jpg",
        "./res/skybox/front.jpg",
        "./res/skybox/back.jpg"
	};

    std::vector<float> skyboxVertices{
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    DisplayMode displayMode;

    virtual void draw ();
    virtual void drawSkybox();
	virtual void init ();
	virtual QString helpString () const;
	virtual void keyPressEvent (QKeyEvent *e);
    virtual void mousePressEvent (QMouseEvent *e);

    void changeDisplayMode(){
        if(displayMode == WIRE)
            displayMode = SOLID;
        else if(displayMode == SOLID)
            displayMode = WIRE;
    }

    unsigned int loadCubemap();

    void clear ();
    void updateCamera (const qglviewer::Vec &center);

public slots:
	void setPlateNumber (int _plateNum);
	void generatePlanet ();
	void clearPlanet ();
	void setPlanetRadius (QString _r);
	void setPlanetElem (QString _elems);
    void savePlanetOff ();
    void savePlanetObj ();
	void setOceanicElevation (QString _e);
	void setContinentElevation (QString _e);
    void setTimeStep(int _t);

    void resegment();
    void deselect();
    void movement();

    void setOceanicOctave(int _o);
    void setContinentalOctave(int _o);

    void selectPlate(QListWidgetItem* item);
    void reelevateOcean();
    void reelevateContinent();

signals:
    void planetFinished();
};

#endif
