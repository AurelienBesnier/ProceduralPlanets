#ifndef PLANETVIEWER_H
#define PLANETVIEWER_H

#include <QKeyEvent>
#include <QGLViewer/qglviewer.h>

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include "Planet.hpp"

enum DisplayMode{ WIRE=0, SOLID=1};

class PlanetViewer : public QGLViewer {
Q_OBJECT

public:
    PlanetViewer (QWidget *parent);
protected:

    Planet planet;
    qglviewer::Vec cam;

    DisplayMode displayMode;

    virtual void draw ();
    virtual void shaderLighting ();
	virtual void init ();
	virtual QString helpString () const;
	virtual void keyPressEvent (QKeyEvent *e);

	void drawClippingPlane ();

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
	void setPlanetElem (int _elems);
	void savePlanetOff () const;
	void savePlanetObj () const;
	void setOceanicThickness (QString _t);
	void setOceanicElevation (QString _e);
	void setContinentThickness (QString _t);
	void setContinentElevation (QString _e);
signals:

};

#endif
