#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>

#include "Plate.hpp"

struct Vertex {
	QVector3D pos;
	QVector3D normal;
	QVector2D texCoord;
};

class Planet {
private:
	unsigned int plateNum;
	PlateParameters plateParams;
	double radius;
	int elems;

	std::vector<Vertex> vertices;
	std::vector<Plate> plates;
	std::vector<unsigned int> indices;

	bool wireframe = false;

public:
	GLuint VBO, VAO, EBO;
	bool planetCreated;
	bool buffersCreated = false;

	Planet () // @suppress("Class members should be properly initialized")
	{
	}
	Planet (QOpenGLContext *context);
	~Planet ();

	void init ();
	void initGLSL ();
	void makeSphere (float radius, int slices, int stacks);
	void makePlates ();
	void initPlanet ();
	void createBuffers ();

	void draw (const qglviewer::Camera *camera);
	void changeViewMode ();
	void clear ();
	void save () const;
	void saveOFF () const;

	void setPlateNumber (int _plateNum);
	void setRadius (double _r);
	void setElems (int _elems);

	void setOceanicThickness (double _t);
	void setOceanicElevation (double _e);
	void setContinentalThickness (double _t);
	void setContinentalElevation (double _e);

	bool printShaderErrors (GLuint shader);
	bool printProgramErrors (int program);
	bool checkOpenGLError ();
	std::string readShaderSource (std::string filename);

	GLuint vShader, gShader, fShader, programID;
	QOpenGLContext *glContext;
	QOpenGLExtraFunctions *glFunctions;

	static void /*GLAPIENTRY */MessageCallback (GLenum source, GLenum type,
												GLuint id, GLenum severity,
												GLsizei length,
												const GLchar *message,
												const void *userParam);
};

#endif // PLANET_H
