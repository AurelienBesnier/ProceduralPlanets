#include "PlanetViewer2.hpp"
#include <cfloat>
#include <QFileDialog>

using namespace std;

PlanetViewer2::PlanetViewer2(QWidget *parent) : QOpenGLWidget(parent)
{

}

void PlanetViewer2::initializeGL ()
{
    drawClippingPlane ();
    glEnable (GL_LIGHTING);

    glEnable (GL_DEPTH_TEST);

    //getCamere pos
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glDisable (GL_BLEND);

    //planet.draw();
    glDisable (GL_LIGHTING);

}

void PlanetViewer2::drawClippingPlane ()
{

    glEnable (GL_LIGHTING);

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glDisable (GL_LIGHTING);

    GLdouble equation[4];
    glGetClipPlane (GL_CLIP_PLANE0, equation);

    qreal p[] = { 0., -equation[3] / equation[1], 0. };
    qreal projP[3];
    //camera ()->getWorldCoordinatesOf (p, projP);

    qreal norm[] =
            { equation[0] + p[0], equation[1] + p[1], equation[2] + p[2] };
    qreal normResult[3];
    QVector3D normal (normResult[0] - projP[0], normResult[1] - projP[1],
                            normResult[2] - projP[2]);
    QVector3D point (projP[0], projP[1], projP[2]);

}

void PlanetViewer2::clear ()
{
    planet.clear ();
    update ();
}

void PlanetViewer2::setPlateNumber (int _plateNum)
{
    planet.setPlateNumber (_plateNum);
    update ();
}

void PlanetViewer2::setPlanetRadius (QString _r)
{
    planet.setRadius (_r.toDouble ());
    update ();
}

void PlanetViewer2::setPlanetElem (int _elems)
{
    planet.setElems (_elems);
    update ();
}

void PlanetViewer2::generatePlanet ()
{
    if (planet.planetCreated)
        planet.clear ();
    planet.initPlanet ();
    update ();
}

void PlanetViewer2::clearPlanet ()
{
    planet.clear ();
    update ();
}

void PlanetViewer2::setOceanicThickness (QString _t)
{

    planet.setOceanicThickness (_t.toDouble ());
    update ();
}

void PlanetViewer2::setOceanicElevation (QString _e)
{
    planet.setOceanicElevation (_e.toDouble ());
    update ();
}

void PlanetViewer2::setContinentThickness (QString _t)
{
    planet.setContinentalThickness (_t.toDouble ());
    update ();
}

void PlanetViewer2::setContinentElevation (QString _e)
{
    planet.setContinentalElevation (_e.toDouble ());
    update ();
}


void PlanetViewer2::savePlanetOff () const
{
    planet.saveOFF ();
}

void PlanetViewer2::savePlanetObj () const
{
    planet.save ();
}

void PlanetViewer2::changeViewMode ()
{
    this->planet.changeViewMode ();
    update ();
}
