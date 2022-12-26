#include "PlanetViewer.hpp"
#include <cfloat>
#include <QFileDialog>
#include <QGLViewer/manipulatedCameraFrame.h>

PlanetViewer::PlanetViewer (QWidget *parent) : QGLViewer (parent)
{
}

void PlanetViewer::init ()
{
    glContext = QOpenGLContext::currentContext();
    glFunctions = glContext->extraFunctions();

    planet = Planet(glContext);
    // The ManipulatedFrame will be used as the clipping plane
    setManipulatedFrame (new qglviewer::ManipulatedFrame ());

    // Enable plane clipping
    glEnable (GL_CLIP_PLANE0);

    //Set background color
    setBackgroundColor (QColor (20, 20, 20));
    displayMode = SOLID;

    //Set blend parameters
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    updateCamera(qglviewer::Vec (0.,0.,0.));

}

void PlanetViewer::draw ()
{
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    this->cam = camera ()->worldCoordinatesOf (qglviewer::Vec (0., 0., 0.));

    if(displayMode == WIRE){
       glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    }  else if(displayMode == SOLID ){
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    planet.draw (camera ());
    update();
}


void PlanetViewer::clear ()
{
    planet.clear ();
	update ();
}

void PlanetViewer::setPlateNumber (int _plateNum)
{
    planet.clear ();
    planet.setPlateNumber (_plateNum);
	update ();
}

void PlanetViewer::setPlanetRadius (QString _r)
{
    planet.clear ();
    planet.setRadius (_r.toDouble ());
	update ();
}

void PlanetViewer::setPlanetElem (QString _elems)
{
    planet.clear ();
    planet.setElems (_elems.toInt());
	update ();
}

void PlanetViewer::generatePlanet ()
{
    if (planet.planetCreated)
        planet.clear ();
    planet.initPlanet ();
	update ();
}

void PlanetViewer::clearPlanet ()
{
    planet.clear ();
	update ();
}

void PlanetViewer::setOceanicThickness (QString _t)
{
    planet.clear ();
    planet.setOceanicThickness (_t.toDouble ());
    planet.initPlanet();
	update ();
}

void PlanetViewer::setOceanicElevation (QString _e)
{
    planet.setOceanicElevation (_e.toDouble ());
    planet.initPlanet();
	update ();
}

void PlanetViewer::setContinentThickness (QString _t)
{
    planet.clear ();
    planet.setContinentalThickness (_t.toDouble ());
    planet.initPlanet();
	update ();
}

void PlanetViewer::setContinentElevation (QString _e)
{
    planet.clear ();
    planet.setContinentalElevation (_e.toDouble ());
    planet.initPlanet();
	update ();
}

void PlanetViewer::updateCamera (const qglviewer::Vec &center)
{
    camera ()->setSceneCenter (center);
    camera ()->setSceneRadius (planet.getRadius());

    camera ()->showEntireScene ();
    update();
}

void PlanetViewer::savePlanetOff () const
{
    planet.saveOFF ();
}

void PlanetViewer::savePlanetObj () const
{
    planet.save ();
}

std::istream& operator>> (std::istream &stream, qglviewer::Vec &v)
{
	stream >> v.x >> v.y >> v.z;

	return stream;
}

void PlanetViewer::shaderLighting()
{
    this->planet.shaderLighting();
    update();
}

void PlanetViewer::keyPressEvent (QKeyEvent *e)
{
	switch (e->key ())
	{
		case Qt::Key_R:
			update ();
			break;
		case Qt::Key_W:
            changeDisplayMode();
            break;
        case Qt::Key_S:
            shaderLighting();
            break;
        case Qt::Key_C:
            updateCamera(qglviewer::Vec (0.,0.,0.));
            break;
		default:
			QGLViewer::keyPressEvent (e);
	}
}

QString PlanetViewer::helpString () const
{
	QString text ("<h2>S i m p l e V i e w e r</h2>");
	text += "Use the mouse to move the camera around the object. ";
	text +=
			"You can respectively revolve around, zoom and translate with the three mouse buttons. ";
	text +=
			"Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
	text +=
			"Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
	text +=
			"Simply press the function key again to restore it. Several keyFrames define a ";
	text +=
			"camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
	text +=
			"Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
	text +=
			"<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
	text +=
			"See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
	text +=
			"Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
	text +=
			"A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
	text +=
			"A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
	text +=
			"See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
	text += "Press <b>Escape</b> to exit the PlanetViewer.";
	return text;
}
