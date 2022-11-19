#include "PlanetViewer.h"
#include <cfloat>
#include <QFileDialog>
#include <QGLViewer/manipulatedCameraFrame.h>

using namespace std;
using namespace qglviewer;

PlanetViewer::PlanetViewer(QWidget *parent)
    : QGLViewer(parent){
}


void PlanetViewer::draw(){
    // initLights();
    
    drawClippingPlane();
    glEnable(GL_LIGHTING);
    
    qglviewer::Vec cam = camera()->worldCoordinatesOf( qglviewer::Vec(0.,0.,0.) );

    glEnable(GL_DEPTH_TEST);
    
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDisable(GL_BLEND);
    

    planet.draw( camera() );

    glDisable(GL_LIGHTING);
}

void PlanetViewer::drawClippingPlane(){
    
    
    glEnable(GL_LIGHTING);
    
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    

    glDisable(GL_LIGHTING);
    
    GLdouble equation[4];
    glGetClipPlane(GL_CLIP_PLANE0, equation);
    
    qreal p[] = {0.,-equation[3]/equation[1], 0.};
    qreal projP[3];
    camera()->getWorldCoordinatesOf(p, projP);
    
    
    qreal norm[] = {equation[0] + p[0], equation[1]+ p[1], equation[2]+ p[2]};
    qreal normResult[3];
    camera()->getWorldCoordinatesOf(norm, normResult);
    
    Vec3Df normal(normResult[0]-projP[0], normResult[1]-projP[1], normResult[2]-projP[2]);
    Vec3Df point(projP[0], projP[1],projP[2]);
    

}

void PlanetViewer::init()
{

    planet = Planet ( QOpenGLContext::currentContext() );
    
    // The ManipulatedFrame will be used as the clipping plane
    setManipulatedFrame(new ManipulatedFrame());
    
    // Enable plane clipping
    glEnable(GL_CLIP_PLANE0);
    
    //Set background color
    setBackgroundColor(QColor(255,255,255));
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    //Set blend parameters
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    imageLoaded = false;
    
    cut = Vec3Df(0.,0.,0.),
    cutDirection = Vec3Df(1.,1.,1.);
}

void PlanetViewer::clear(){
    planet.clear();
}



void PlanetViewer::setPlateNumber(int _plateNum)
{
    planet.setPlateNumber(_plateNum);
    update();
}

void PlanetViewer::generatePlanet()
{
    planet.initPlanet();
    update();
}

void PlanetViewer::updateCamera(const qglviewer::Vec & center, float radius){
    camera()->setSceneCenter(center);
    camera()->setSceneRadius(radius);
    
    camera()->showEntireScene();
}



std::istream & operator>>(std::istream & stream, qglviewer::Vec & v)
{
    stream >>
            v.x >>
            v.y >>
            v.z;
    
    return stream;
}


void PlanetViewer::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_R :update(); break;
    default : QGLViewer::keyPressEvent(e);
    }
}

QString PlanetViewer::helpString() const
{
    QString text("<h2>S i m p l e V i e w e r</h2>");
    text += "Use the mouse to move the camera around the object. ";
    text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
    text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
    text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
    text += "Simply press the function key again to restore it. Several keyFrames define a ";
    text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
    text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
    text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
    text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
    text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
    text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
    text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
    text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
    text += "Press <b>Escape</b> to exit the PlanetViewer.";
    return text;
}
