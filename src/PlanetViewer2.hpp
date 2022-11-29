#ifndef PLANETVIEWER2_H
#define PLANETVIEWER2_H

#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QVector3D>

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include "Planet.hpp"

class PlanetViewer2 : public QOpenGLWidget
{
    Q_OBJECT
public:
    PlanetViewer2 (QWidget *parent);

protected:

    Planet planet;

    virtual void draw ();
    virtual void changeViewMode ();
    virtual void initializeGL ();
    virtual QString helpString () const;
    virtual void keyPressEvent (QKeyEvent *e);

    void drawClippingPlane ();

    void clear ();
    void updateCamera (const qglviewer::Vec &center, float radius);

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

#endif // PLANETVIEWER2_H
