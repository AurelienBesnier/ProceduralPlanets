#include "PlanetViewer.hpp"
#include <cfloat>
#include <QFileDialog>
#include <QGLViewer/manipulatedCameraFrame.h>
#include <filesystem>


PlanetViewer::PlanetViewer (QWidget *parent) : QGLViewer (parent)
{
}

unsigned int PlanetViewer::loadCubemap()
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < skyboxFaces.size(); i++)
	{
        QImage texture = QImage(skyboxFaces[i].c_str());
        texture = texture.convertToFormat(QImage::Format_RGB888);
		unsigned char *data = texture.bits();
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                        0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << skyboxFaces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}  

void PlanetViewer::init ()
{
    planet = Planet (QOpenGLContext::currentContext ());
    skyboxShader = new QOpenGLShaderProgram();
    std::filesystem::path fs = std::filesystem::current_path ();
    std::string path = fs.string () + "/GLSL/shaders/";
    QString  vShaderPath = QString::fromStdString(path + "skybox.vert");
    QString  fShaderPath = QString::fromStdString(path + "skybox.frag");
    if(!skyboxShader->addShaderFromSourceFile(QOpenGLShader::Vertex,vShaderPath))
    {
        std::cerr<<skyboxShader->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add VERTEX shader"<<std::endl;
    }
    if(!skyboxShader->addShaderFromSourceFile(QOpenGLShader::Fragment,fShaderPath))
    {
        std::cerr<<skyboxShader->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add FRAGMENT shader"<<std::endl;
    }

    if(!skyboxShader->link())
    {
        std::cerr<<skyboxShader->log().toStdString()<<std::endl;
        std::cerr<<"Error linking program"<<std::endl;
    }
    skyboxShader->bind();

    skyboxTextureID = loadCubemap();

    skyboxVAO = new QOpenGLVertexArrayObject();
    skyboxVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    skyboxVAO->create();
    skyboxVBO->create();
    skyboxVBO->bind();
    skyboxVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
    skyboxVBO->allocate(skyboxVertices.data(),sizeof(float)*skyboxVertices.size());
    skyboxShader->enableAttributeArray(0);
    skyboxShader->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(float));

    skyboxVAO->release();
    skyboxVBO->release();

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

    setFPSIsDisplayed(true);
    displayMessage	("Viewer Initiliazed");

    skyboxShader->release();

}

void PlanetViewer::drawSkybox()
{
    glDepthMask(GL_FALSE);
    skyboxShader->bind();
    float pMatrix[16];
    float mvMatrix[16];
    glm::mat4 view;
    glm::vec3 cameraPos   = glm::vec3(camera()->position().x, camera()->position().y,  camera()->position().z);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    camera()->getProjectionMatrix (pMatrix);
    QOpenGLContext::currentContext ()->extraFunctions()->glUniformMatrix4fv (
             QOpenGLContext::currentContext ()->extraFunctions()->glGetUniformLocation (skyboxShader->programId(), "proj_matrix"), 1,
             GL_FALSE,
             pMatrix);
    QOpenGLContext::currentContext ()->extraFunctions()->glUniformMatrix4fv (
             QOpenGLContext::currentContext ()->extraFunctions()->glGetUniformLocation (skyboxShader->programId(), "view"), 1,
             GL_FALSE,
             &view[0][0]);
    skyboxVAO->bind();
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    skyboxShader->release();
    skyboxVAO->release();
    glDepthMask(GL_TRUE);
}

void PlanetViewer::draw ()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawClippingPlane ();

    this->cam = camera ()->worldCoordinatesOf (qglviewer::Vec (0., 0., 0.));

    glEnable(GL_LIGHTING);
    glEnable (GL_DEPTH_TEST);
    glDisable (GL_BLEND);

    if(displayMode == WIRE){
       glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    }  else if(displayMode == SOLID ){
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    drawSkybox();
    planet.draw (camera ());

    glDisable(GL_LIGHTING);
    update();
}

void PlanetViewer::drawClippingPlane ()
{
	glEnable (GL_LIGHTING);

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);


	GLdouble equation[4];
	glGetClipPlane (GL_CLIP_PLANE0, equation);

	qreal p[] = { 0., -equation[3] / equation[1], 0. };
	qreal projP[3];
    camera ()->getWorldCoordinatesOf (p, projP);

	qreal norm[] =
			{ equation[0] + p[0], equation[1] + p[1], equation[2] + p[2] };
	qreal normResult[3];
	qglviewer::Vec normal (normResult[0] - projP[0], normResult[1] - projP[1],
							normResult[2] - projP[2]);
	qglviewer::Vec point (projP[0], projP[1], projP[2]);

}

void PlanetViewer::clear ()
{
	if(!generationFuture.isRunning()){
		planet.clear ();
		update ();
	}
}

void PlanetViewer::setPlateNumber (int _plateNum)
{
	if(!generationFuture.isRunning()){
		planet.clear ();
    	planet.setPlateNumber (_plateNum);
		update ();
	}
}

void PlanetViewer::setPlanetRadius (QString _r)
{
	if(!generationFuture.isRunning()){
		planet.clear ();
    	planet.setRadius (_r.toDouble ());
		update ();
	}
}

void PlanetViewer::setPlanetElem (QString _elems)
{
	if(!generationFuture.isRunning()){
		planet.clear ();
    	planet.setElems (_elems.toInt());
		update ();
	}
}

void PlanetViewer::generatePlanet ()
{
    displayMessage	("Generating planet");
	if (planet.planetCreated)
		planet.clear ();
	if(!generationFuture.isRunning())
		generationFuture = QtConcurrent::run( [this]{planet.initPlanet(); update ();});
}

void PlanetViewer::clearPlanet ()
{
    displayMessage	("Planet cleared");
	if(!generationFuture.isRunning()){
		planet.clear ();
		update ();
	}
}

void PlanetViewer::setOceanicElevation (QString _e)
{
	if(!generationFuture.isRunning()){
		planet.setOceanicElevation (_e.toDouble ());
		update ();
	}
}

void PlanetViewer::setContinentElevation (QString _e)
{
	if(!generationFuture.isRunning()){
		planet.clear ();
		planet.setContinentalElevation (_e.toDouble ());
		update ();
	}
}

void PlanetViewer::updateCamera (const qglviewer::Vec &center)
{
    camera ()->setSceneCenter (center);
    camera ()->setSceneRadius (planet.getRadius());

    camera ()->showEntireScene ();
    update();
}

void PlanetViewer::savePlanetOff ()
{
    displayMessage	("Planet saved as planet.off");
	planet.saveOFF ();
}

void PlanetViewer::savePlanetObj ()
{
    displayMessage	("Planet saved as planet.obj");
	planet.save ();
}

std::istream& operator>> (std::istream &stream, qglviewer::Vec &v)
{
	stream >> v.x >> v.y >> v.z;

	return stream;
}

void PlanetViewer::shaderLighting()
{
    //this->planet.shaderLighting();
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
	QString text ("<h2>Procedural Planets</h2>");
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
