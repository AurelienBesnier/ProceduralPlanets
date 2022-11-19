#ifndef PLANETVIEWER_H
#define PLANETVIEWER_H

#include <QKeyEvent>
#include <QGLViewer/qglviewer.h>

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include "Planet.h"

class PlanetViewer : public QGLViewer
{
    Q_OBJECT

public :
    PlanetViewer(QWidget *parent);


protected :

    Planet planet;

    bool imageLoaded;

    virtual void draw();
    virtual void init();
    virtual QString helpString() const;
    virtual void keyPressEvent(QKeyEvent *e);

    void drawClippingPlane();

    void clear();
    void updateCamera(const qglviewer::Vec & center, float radius);


    Vec3Df cut;
    Vec3Df cutDirection;

    std::map<unsigned char, bool> iDisplayMap;
    std::map<unsigned char, QColor> iColorMap;
    std::vector<unsigned char> subdomain_indices;


public slots:
    void setPlateNumber(int _plateNum);
    void generatePlanet();

 signals:

};

#endif
