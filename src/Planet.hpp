#ifndef PLANET_H
#define PLANET_H

#include <QString>
#include <QColor>
#include <QVector>
#include <QGLViewer/vec.h>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QGLViewer/camera.h>

#include "Plate.hpp"

class Planet {
private:
	unsigned int plateNum;
	PlateParameters plateParams;
	double radius;
	int elems;

	std::vector<qglviewer::Vec> positions;
	std::vector<Plate> plates;
	std::vector<unsigned int> indices;
	std::vector<float> normals;
	std::vector<float> texCoods;

	bool wireframe = false;

public:
	GLuint VBO, VAO, EBO;
	bool planetCreated;
	bool buffersCreated = false;

	Planet ()
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
