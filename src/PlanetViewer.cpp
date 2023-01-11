#include "PlanetViewer.hpp"
#include <cfloat>
#include <QFileDialog>
#include <QGLViewer/manipulatedCameraFrame.h>
#include <filesystem>
#include <GL/glu.h> 


PlanetViewer::PlanetViewer (QWidget *parent) : QGLViewer (parent)
{
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

    skyboxShader->setUniformValue(skyboxShader->uniformLocation("skybox"), 0);

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
    glDepthFunc(GL_LEQUAL);
    skyboxShader->bind();
    float pMatrix[16];
    glm::mat4 view;
    glm::vec3 cameraPos   = glm::vec3(camera()->position().x, camera()->position().y,  camera()->position().z);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    view = glm::mat4(glm::mat3(view));

    glActiveTexture(GL_TEXTURE0+skyboxTextureID);

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
    glDepthFunc(GL_LESS);
}

void PlanetViewer::draw ()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(displayMode == WIRE){
       glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    }  else if(displayMode == SOLID ){
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    if(planet.planetCreated && !planetCreated)
    {
        planetCreated = true;
        emit this->planetFinished();
        displayMessage("Planet created !");
    }

    drawSkybox();
    planet.draw (camera ());

    update();
}

unsigned int PlanetViewer::loadCubemap()
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
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


void PlanetViewer::clear ()
{
	if(!generationFuture.isRunning()){
        planet.clear ();planetCreated =false;
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
    
    if (planet.planetCreated){
        planet.clear ();
    }
	if(!generationFuture.isRunning()){
        displayMessage	("Generating planet...",-1);
		generationFuture = QtConcurrent::run(
        [this]{
            planet.initPlanet(); update (); planetCreated =false;
        });
    }
}

void PlanetViewer::clearPlanet ()
{
	if(!generationFuture.isRunning()){
        planet.clear ();
        displayMessage	("Planet cleared");
		update ();
	}
}

void PlanetViewer::resegment()
{
    if(!generationFuture.isRunning() && planet.planetCreated){
        planet.resegment();
        displayMessage ("Resegmented");
    }
}


void PlanetViewer::reelevateOcean()
{
    if(!generationFuture.isRunning() && planet.planetCreated){

        planet.reelevateOcean();
        displayMessage ("Re-elevating Ocean");
    }

}
void PlanetViewer::reelevateContinent()
{
    if(!generationFuture.isRunning() && planet.planetCreated){
        planet.reelevateContinent();
        displayMessage ("Re-elevating Continent");
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

void PlanetViewer::setOceanicOctave(int _o)
{
	if(!generationFuture.isRunning()){
        this->planet.setOceanicOctave(_o);
    }
}

void PlanetViewer::setContinentalOctave(int _o)
{
	if(!generationFuture.isRunning()){
        this->planet.setContinentalOctave(_o);
    }
}


void PlanetViewer::selectPlate(QListWidgetItem* item)
{
    int id = item->text().toDouble ();

    planet.selectedPlateID = (float)id;
    update();
}

void PlanetViewer::deselect()
{
    planet.selectedPlateID = (float)-1.0f;
    update();
}

void PlanetViewer::updateCamera (const qglviewer::Vec &center)
{
    camera ()->setSceneCenter (center);
    camera ()->setSceneRadius (planet.getRadius()+planet.plateParams.continentalElevation);

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
            planet.shaderLighting = !planet.shaderLighting;
            break;
        case Qt::Key_C:
            updateCamera(qglviewer::Vec (0.,0.,0.));
            break;
        case Qt::Key_O:
            planet.oceanDraw = !planet.oceanDraw;
            break;
		default:
			QGLViewer::keyPressEvent (e);
	}
}

void PlanetViewer::mousePressEvent (QMouseEvent *e)
{
    switch (e->button())
    {
        case Qt::MiddleButton:
            double projection[16];
            double modelview[16];
            camera()->getProjectionMatrix (projection);
            camera()->getModelViewMatrix (modelview);

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            GLdouble xw,yw,zw;
            GLfloat winx, winy, winz;
            winx = e->localPos().x();
            winy = (viewport[3] - e->localPos().y());
            winz = 0;
            
            gluUnProject(winx, winy, winz, modelview, projection, viewport, &xw, &yw, &zw);

            planet.closestPoint(QVector3D(xw,yw,zw));
            break;
        default:
            QGLViewer::mousePressEvent (e);
            break;
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
