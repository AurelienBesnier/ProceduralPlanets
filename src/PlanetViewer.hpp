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


/**
 * @brief Viewer class.
 * 
 */
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
    unsigned long int years = 0;

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

    /**
     * @brief Method to draw the scene
     * 
     */
    virtual void draw ();

    /**
     * @brief Method to draw the scene(not functional yet).
     * 
     */
    virtual void drawSkybox();

    /**
     * @brief Method to initialize the viewer.
     * This method setup the OpenGL context of the viewer and initialize the objects
     * of the scene.
     */
	virtual void init ();

    /**
     * @brief help method of QGLViewer.
     * 
     * @return QString 
     */
	virtual QString helpString () const;

    /**
     * @brief Method to handle the keyboard events.
     * 
     * @param e the key event. 
     */
	virtual void keyPressEvent (QKeyEvent *e);

    /**
     * @brief Method to handle the mouse events.
     * 
     * @param e the mouse event.
     */
    virtual void mousePressEvent (QMouseEvent *e);

    /**
     * @brief Change the display mode of the viewer.
     * Change the viewer display mode between Wireframe and Solid. 
     */
    void changeDisplayMode(){
        if(displayMode == WIRE)
            displayMode = SOLID;
        else if(displayMode == SOLID)
            displayMode = WIRE;
    }

    unsigned int loadCubemap();

    /**
     * @brief Method to clear the objects in the viewer.
     * 
     */
    void clear ();

    /**
     * @brief Method to center the camera on the <b>center</b> vector.
     * 
     * @param center 
     */
    void updateCamera (const qglviewer::Vec &center);

public slots:

    /**
     * @brief Set the Plate Number object of the planet.
     * 
     * @param _plateNum 
     */
	void setPlateNumber (int _plateNum);

    /**
     * @brief Slot method triggered by the <b>Generate</b> button.
     * 
     */
	void generatePlanet ();

    /**
     * @brief Slot method triggered by the <b>Clear</b> button.
     * 
     */
	void clearPlanet ();

    /**
     * @brief Set the Planet Radius object
     * 
     * @param _r 
     */
	void setPlanetRadius (QString _r);

    /**
     * @brief Set the Planet Elem object
     * 
     * @param _elems 
     */
	void setPlanetElem (QString _elems);

    /**
     * @brief Saves the planet in an .off file
     * 
     */
    void savePlanetOff ();

    /**
     * @brief Saves the planet in an .obj file(not functional).
     * 
     */
    void savePlanetObj ();

    /**
     * @brief Set the Oceanic Elevation object
     * 
     * @param _e 
     */
	void setOceanicElevation (QString _e);

    /**
     * @brief Set the Continent Elevation object
     * 
     * @param _e 
     */
	void setContinentElevation (QString _e);

    /**
     * @brief Set the Time Step object
     * 
     * @param _t 
     */
    void setTimeStep(int _t);

    /**
     * @brief Slot method used to resegment the plates.
     * Calls the <b>resegment</b> method of the Planet class.
     */
    void resegment();

    /**
     * @brief Slot method used deselect the plate.
     * 
     */
    void deselect();

    /**
     * @brief Slot method used to move the plates.
     * Calls the <b>move</b> method of the Planet class the number of times of the timeStep slider. 
     */
    void movement();

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
     * @brief Slot method triggered by the list widget selection.
     * 
     * @param item 
     */
    void selectPlate(QListWidgetItem* item);

    /**
     * @brief Slot method triggered by the <b>Re-init Elevation</b> button.
     * 
     */
    void reelevateOcean();

    /**
     * @brief Slot method triggered by the <b>Re-init Elevation</b> button.
     * 
     */
    void reelevateContinent();

signals:

    /**
     * @brief Signal sent once the planet has been created.
     * 
     */
    void planetFinished();
};

#endif
