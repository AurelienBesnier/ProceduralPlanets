#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

#include "Planet.hpp"

#define PI 3.14159265

Planet::Planet (QOpenGLContext *context)
{
	glContext = context;
	init ();
	initGLSL ();
}

Planet::~Planet ()
{
	vertices.clear ();
	indices.clear ();
    pos.clear();
	glFunctions->glDeleteVertexArrays (1, &VAO);
	glFunctions->glDeleteBuffers (1, &VBO);
	glFunctions->glDeleteBuffers (1, &EBO);
	glFunctions->glDeleteProgram (this->programID);
}

void Planet::init ()
{
	planetCreated = false;
    this->plateNum = 4;
    this->radius = 6370;
    this->elems = 1000;
}

void Planet::initGLSL ()
{
	std::filesystem::path fs = std::filesystem::current_path ();
	std::string path = fs.string () + "/GLSL/shaders/";
	std::string vShaderPath = path + "planet.vert";
	std::string fShaderPath = path + "planet.frag";

	glFunctions = glContext->extraFunctions ();
	glEnable ( GL_DEBUG_OUTPUT);
	glFunctions->glDebugMessageCallback (&Planet::MessageCallback, 0);

	glFunctions->glDisable (GL_LIGHT0);
	glFunctions->glEnable (GL_LIGHT1);
	glFunctions->glEnable (GL_LIGHTING);
	glFunctions->glEnable (GL_COLOR_MATERIAL);
	glFunctions->glEnable (GL_BLEND);
	glFunctions->glEnable (GL_TEXTURE_2D);
	glFunctions->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create programs and link shaders
	this->programID = glFunctions->glCreateProgram ();
	std::string content = readShaderSource (vShaderPath);
	if (!content.empty ())
	{
		this->vShader = glFunctions->glCreateShader (GL_VERTEX_SHADER);
		const char *src = content.c_str ();
		glFunctions->glShaderSource (this->vShader, 1, &src, NULL);
		glFunctions->glCompileShader (this->vShader);
		glFunctions->glAttachShader (this->programID, this->vShader);
		printShaderErrors (this->vShader);
	}
	content = readShaderSource (fShaderPath);
	if (!content.empty ())
	{
		this->fShader = glFunctions->glCreateShader (GL_FRAGMENT_SHADER);
		const char *src = content.c_str ();
		glFunctions->glShaderSource (this->fShader, 1, &src, NULL);
		glFunctions->glCompileShader (this->fShader);
		glFunctions->glAttachShader (this->programID, this->fShader);
		printShaderErrors (this->fShader);
	}

	glFunctions->glLinkProgram (this->programID);
	glFunctions->glUseProgram (programID);
	printProgramErrors (programID);
	checkOpenGLError ();
}


void Planet::initPlanet ()
{
    makeSphere (this->radius, elems);
    makePlates ();
    triangulate();

	planetCreated = true;
}

void Planet::makeSphere (float radius, int slices, int stacks)
{
    // Calc The Vertices
	std::vector<Point> normals;
    std::vector<QVector2D> texCoords;

	for (int i = 0; i <= stacks; ++i)
	{
		float V = i / (float) stacks;
        float phi = V * PI;

		// Loop Through Slices
		for (int j = 0; j <= slices; ++j)
		{
			float U = j / (float) slices;
            float theta = U * (PI * 2);

			// Calc The Vertex Positions
			float x = cosf (theta) * sinf (phi);
			float y = cosf (phi);
			float z = sinf (theta) * sinf (phi);

			// Push Back Vertex Data
            Point position = Point (x * radius, y * radius, z * radius) ;
            float squareLength = position.x()*position.x() + position.y()*position.y() + position.z()*position.z(); ;
            float length = sqrt(squareLength);
            Point normal = Point(position.x()/length,position.y()/length,position.z()/length);
            QVector2D texCoord = QVector2D ((float )j / slices, (float)i / stacks);
            pos.push_back(position);
		 	normals.push_back(normal);
		 	texCoords.push_back(texCoord);
		}
	}

	// Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i)
	{
		indices.push_back (i);
		indices.push_back (i + slices + 1);
		indices.push_back (i + slices);

		indices.push_back (i + slices + 1);
		indices.push_back (i);
		indices.push_back (i + 1);
    }

    for(size_t i = 0; i < pos.size(); i++)
	{
            Vertex newVertex = { .pos = pos[i], .normal = normals[i], .texCoord =
					texCoords[i] };
			vertices.push_back (newVertex);
    }
    std::cout<<pos.size()<<std::endl;
}


void Planet::makeSphere (float radius, int elems)
{
    // Calc The Vertices
    std::vector<Point> normals;
    std::vector<QVector2D> texCoords;

    float phi = PI * (3.0 - sqrt(5.0));

    for (int i = 0; i < elems; ++i)
    {
        float y = 1 - (i / float(elems - 1)) * 2;
        float rad = sqrt(1 - y*y);
        float theta = phi * i;

        float x = cosf(theta) * rad;
        float z = sinf(theta) * rad;

        Point position = Point (x * radius, y * radius, z * radius) ;
        float squareLength = position.x()*position.x() + position.y()*position.y() + position.z()*position.z(); ;
        float length = sqrt(squareLength);
        Point normal = Point(position.x()/length,position.y()/length,position.z()/length);
        QVector2D texCoord = QVector2D ((float )i / elems, (float)i / elems);
        pos.push_back(position);
        normals.push_back(normal);
        texCoords.push_back(texCoord);
    }

    for (int i = 0; i < elems; ++i)
    {
        indices.push_back (i);
    }

    for(size_t i = 0; i < pos.size(); i++)
    {
            Vertex newVertex = { .pos = pos[i], .normal = normals[i], .texCoord =
                    texCoords[i]};
            vertices.push_back (newVertex);
    }
    std::cout<<pos.size()<<std::endl;
}

void Planet::makePlates ()
{
    if (plates.size () > 1)
	{
		plates.clear ();
        std::vector<unsigned int> tmp_init(plateNum);
        std::vector<Point> colors(plateNum);
        QRandomGenerator prng;
        prng.seed(time(NULL));

        for(Point c : colors)
            c = Point(prng.generateDouble()*1,prng.generateDouble()*1,prng.generateDouble()*1);

	}
    QRandomGenerator prng;
    prng.seed(time(NULL));
    for(size_t i = 0; i<vertices.size(); i++)
    {
        vertices[i].color=Point(prng.generateDouble()*1,prng.generateDouble()*1,prng.generateDouble()*1);
    }
}

void Planet::triangulate()
{
    // construction from a list of points :
    Triangulation T(this->pos.begin(), this->pos.end());
    Triangulation::size_type n = T.number_of_vertices();
    assert(T.is_valid());
    std::cout<<n<<" "<<pos.size()<<std::endl;
    assert(n==pos.size());
}

void Planet::setOceanicThickness (double _t)
{
	plateParams.oceanicThickness = _t;
	std::cout << "Oceanic thickness: " << this->plateParams.oceanicThickness
			<< std::endl;
}

void Planet::setOceanicElevation (double _e)
{
	plateParams.oceanicEleavation = _e;
	std::cout << "Oceanic elevation: " << this->plateParams.oceanicEleavation
			<< std::endl;
}

void Planet::setContinentalThickness (double _t)
{
	plateParams.continentalThickness = _t;
	std::cout << "Continental thickness: "
			<< this->plateParams.continentalThickness << std::endl;
}

void Planet::setContinentalElevation (double _e)
{
	plateParams.continentalElevation = _e;
	std::cout << "Continental elevation: "
			<< this->plateParams.continentalElevation << std::endl;
}

void Planet::setPlateNumber (int _plateNum)
{
	this->plateNum = _plateNum;

	std::cout << "planet plate number: " << this->plateNum << std::endl;
}

void Planet::setRadius (double _r)
{
	this->radius = _r;

	std::cout << "planet radius set to " << this->radius << std::endl;
}

double Planet::getRadius() const
{
	return this->radius;
}

void Planet::setElems (int _elems)
{
	this->elems = _elems;

	std::cout << "planet elems set to " << this->elems << std::endl;
}


void /*GLAPIENTRY */Planet::MessageCallback (GLenum source, GLenum type,
												GLuint id, GLenum severity,
												GLsizei length,
												const GLchar *message,
												const void *userParam)
{
	if (severity == GL_DEBUG_SEVERITY_HIGH
			|| severity == GL_DEBUG_SEVERITY_MEDIUM
			|| severity == GL_DEBUG_SEVERITY_LOW)
	{
		std::string s_severity = (
				severity == GL_DEBUG_SEVERITY_HIGH ? "High" :
				severity == GL_DEBUG_SEVERITY_MEDIUM ? "Medium" : "Low");
		std::cout << "Error " << id << " [severity=" << s_severity << "]: "
				<< message << std::endl;
	}
}
bool Planet::checkOpenGLError ()
{
	bool error = false;
	int glErr = glGetError ();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "[OpenGL] Error: " << glErr << std::endl;
		error = true;
		glErr = glGetError ();
	}
	return !error;
}

bool Planet::printShaderErrors (GLuint shader)
{
	int state = 0;
	glFunctions->glGetShaderiv (shader, GL_COMPILE_STATUS, &state);
	if (state == 1)
		return true;
	int len = 0;
	int chWritten = 0;
	char *log;
	glFunctions->glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*) malloc (len);
		glFunctions->glGetShaderInfoLog (shader, len, &chWritten, log);
		std::cout << "[OpenGL] Shader error: " << log << std::endl;
		free (log);
	}
	return false;
}
bool Planet::printProgramErrors (int program)
{
	int state = 0;
	glFunctions->glGetProgramiv (program, GL_LINK_STATUS, &state);
	if (state == 1)
		return true;
	int len = 0;
	int chWritten = 0;
	char *log;
	glFunctions->glGetProgramiv (program, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*) malloc (len);
		glFunctions->glGetProgramInfoLog (program, len, &chWritten, log);
		std::cout << "[OpenGL] Program error: " << log << std::endl;
		free (log);
	}
	return false;
}

std::string Planet::readShaderSource (std::string filename)
{
	std::string content = "";
	QString qFilename = QString::fromStdString (filename);
	if (!QFile::exists (qFilename))
		qFilename = ":" + qFilename;
	if (!QFile::exists (qFilename))
	{
		std::cerr << "The shader " << filename << " doesn't exist!"
				<< std::endl;
		return "";
	}
	QFile file (qFilename);
	file.open (QIODevice::ReadOnly | QIODevice::Text);
	std::string line;
	QTextStream in (&file);
	while (!in.atEnd ())
	{
		line = in.readLine ().toStdString ();
		content += line + " \n";
	}
	file.close ();
	return content;
}

void Planet::changeViewMode ()
{
	this->wireframe = !wireframe;
}

void Planet::createBuffers ()
{
	glFunctions->glGenVertexArrays (1, &VAO);
	glFunctions->glGenBuffers (1, &VBO);
	glFunctions->glGenBuffers (1, &EBO);

	glFunctions->glBindVertexArray (VAO);

	glFunctions->glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glFunctions->glBufferData (GL_ARRAY_BUFFER,
								vertices.size () * sizeof(Vertex), &vertices[0],
								GL_STATIC_DRAW);

	glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
	glFunctions->glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                indices.size () * sizeof(int),
                                &indices[0], GL_STATIC_DRAW);

	glFunctions->glEnableVertexAttribArray (0);
	glFunctions->glVertexAttribPointer (0, 3, GL_DOUBLE, GL_FALSE,
										sizeof(Vertex), (void*) 0);
	glFunctions->glEnableVertexAttribArray (1);
	glFunctions->glVertexAttribPointer (1, 3, GL_DOUBLE, GL_FALSE,
										sizeof(Vertex),
										(void*) offsetof(Vertex, normal));
	glFunctions->glEnableVertexAttribArray (2);
	glFunctions->glVertexAttribPointer (2, 2, GL_DOUBLE, GL_FALSE,
										sizeof(Vertex),
										(void*) offsetof(Vertex, texCoord));

    glFunctions->glEnableVertexAttribArray (3);
    glFunctions->glVertexAttribPointer (3, 3, GL_DOUBLE, GL_FALSE,
                                        sizeof(Vertex),
                                        (void*) offsetof(Vertex, color));

	glFunctions->glBindBuffer (GL_ARRAY_BUFFER, 0);

	glFunctions->glBindVertexArray (0);
}

void Planet::draw (const qglviewer::Camera *camera)
{

	if (!planetCreated)
		return;

	if (!buffersCreated)
	{
		buffersCreated = true;
		createBuffers ();
	}

	glLightfv (GL_LIGHT1, GL_POSITION, camera->position());
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, camera->viewDirection());
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	if (wireframe)
		glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	//GPU start
	// Récuperation des matrices de projection / vue-modèle
	float pMatrix[16];
	float mvMatrix[16];
	camera->getProjectionMatrix (pMatrix);
	camera->getModelViewMatrix (mvMatrix);
	glFunctions->glUniformMatrix4fv (
			glFunctions->glGetUniformLocation (programID, "proj_matrix"), 1,
			GL_FALSE,
			pMatrix);
	glFunctions->glUniformMatrix4fv (
			glFunctions->glGetUniformLocation (programID, "mv_matrix"), 1,
			GL_FALSE,
			mvMatrix);

  float lightColor[3] = {1,0.9,0.8};
	glFunctions->glUniform3fv(
			glFunctions->glGetUniformLocation(programID, "viewPos"), 1,
      camera->position()
			);

	glFunctions->glUniform3fv(
			glFunctions->glGetUniformLocation(programID, "lightPos"), 1,
      camera->position()
			);
	
	glFunctions->glUniform3fv(
			glFunctions->glGetUniformLocation(programID, "lightColor"), 1,
			lightColor
			);

    glPointSize(4);
	glFunctions->glBindVertexArray (VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}

void Planet::clear ()
{
	if (planetCreated)
	{
		vertices.clear ();
		indices.clear ();
        pos.clear();

		glFunctions->glDeleteVertexArrays (1, &VAO);
		glFunctions->glDeleteBuffers (1, &VBO);
		glFunctions->glDeleteBuffers (1, &EBO);
		buffersCreated = false;
		planetCreated = false;
	}
}

void Planet::save () const
{
	std::string filename = "planet.obj";
	std::ofstream ostream;
	ostream.precision (4);

	ostream.open (filename, std::ios_base::out);
	ostream << "o planet" << std::endl;
	for (size_t i = 0; i < vertices.size (); i++)
	{
		ostream << "v " << std::fixed << vertices[i].pos.x () << " "
				<< std::fixed << vertices[i].pos.y () << " " << std::fixed
				<< vertices[i].pos.z () << std::endl;
	}

	for (size_t i = 0; i < vertices.size (); i++)
	{
		ostream << "vt " << vertices[i].texCoord.x () << " "
				<< vertices[i].texCoord.y () << std::endl;
	}


	for (size_t i = 0; i < vertices.size (); i++)
	{
		ostream << "vn " << vertices[i].normal.x () << " "
				<< vertices[i].normal.y () << " " << vertices[i].normal.z ()
				<< std::endl;
	}
	ostream << "s 0" << std::endl;

	for (size_t i = 0; i < indices.size (); i += 3)
	{
		Vertex v1 = vertices[indices[i]];
		Vertex v2 = vertices[indices[i + 1]];
		Vertex v3 = vertices[indices[i + 2]];
		ostream << "f " << indices[i] << " " << indices[i + 1] << " "
				<< indices[i + 2] << std::endl;
	}

	ostream.close ();

	std::cout << "Wrote to file " << filename << std::endl;
}

void Planet::saveOFF () const
{
	std::string filename = "planet.off";
	std::ofstream ostream;
	ostream.precision (4);

	ostream.open (filename, std::ios_base::out);

	ostream << "OFF " << std::endl;
	ostream << (vertices.size ()) << " " << indices.size () << " 0"
			<< std::endl;

	for (size_t i = 0; i < vertices.size (); i++)
	{
		ostream << vertices[i].pos.x () << " " << vertices[i].pos.y () << " "
				<< vertices[i].pos.z () << std::endl;
	}

	for (unsigned int t = 0; t < indices.size (); t += 3)
	{
		ostream << "3 " << (indices[t]) << " " << (indices[t + 1]) << " "
				<< (indices[t + 2]) << std::endl;
	}

	ostream.close ();

	std::cout << "Wrote to file " << filename << std::endl;
}
