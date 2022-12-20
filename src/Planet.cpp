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
#include "Plate.hpp"

#define PI 3.14159265358979323846

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
    plates.clear();
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
    this->elems = 600;
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
    triangulate();
    makePlates ();

	planetCreated = true;
}

void Planet::makeSphere (float radius, int elems)
{
    // Calc The Vertices
    pos.reserve(elems);
    std::vector<QVector3D> normals;
    normals.reserve(elems);
    std::vector<QVector2D> texCoords;
    texCoords.reserve(elems);

    double goldenRatio = (1 + pow(5,0.5))/2;

    for (int i = 0; i < elems; ++i)
    {
        double theta = 2 * PI * i / goldenRatio;
        double phi = acosf(1 - 2 * (i+0.5f) / elems);
        double x = cosf(theta)*sinf(phi), y=sinf(theta)*sinf(phi), z=cosf(phi);

        Point position = Point (x * radius, y * radius, z * radius) ;
        double squareLength = position.x()*position.x() + position.y()*position.y() + position.z()*position.z(); ;
        double length = sqrt(squareLength);
        QVector3D normal = QVector3D(position.x()/length,position.y()/length,position.z()/length);
        QVector2D texCoord = QVector2D ((float )i / elems, (float)i / elems);
        pos.push_back( position );
        normals.push_back(normal);
        texCoords.push_back(texCoord);
    }

    for(size_t i = 0; i < pos.size(); i++)
    {
        Vertex newVertex = { .pos = pos[i], .normal = normals[i], .texCoord =
                texCoords[i]};
        vertices.push_back (newVertex);
    }
    std::cout<<pos.size()<<std::endl;
}

void collect_one_ring (std::vector<Point> const & i_vertices,
                       std::vector< unsigned int > const & i_triangles,
                       std::vector<std::vector<unsigned int> > & o_one_ring) {
    o_one_ring.clear();
    o_one_ring.resize(i_vertices.size());
    size_t i, j, y, size=i_triangles.size();
    for(i = 0; i<size; i+=3)
    {
        for(j = 0; j<3; j++) //sommet courant
        {
            for(y = 0; y<3 ;y++) // sommets voisins
            {
                if(j!=y)
                {
                    if(std::find(o_one_ring.begin(), o_one_ring.end(),
                     o_one_ring[i_triangles[i]]) != o_one_ring.end()){
                        o_one_ring[i_triangles[i+j]].push_back(i_triangles[i+y]);
                        /*o_one_ring[i_triangles[i]].push_back(i_triangles[i+y+1]);
                        o_one_ring[i_triangles[i]].push_back(i_triangles[i+y+2]);*/
                    }
                }

            }
        }
    }
}

void Planet::makePlates ()
{
    std::vector<std::vector<unsigned int> >  one_ring;
    collect_one_ring(pos,indices,one_ring);

    plates.clear ();
    plates.resize (plateNum);
    QRandomGenerator prng;
    prng.seed(time(NULL));
    std::vector<unsigned int> tmp_init;
    std::vector<QVector3D> colors(plateNum);
    std::vector<std::vector<unsigned int>> last_ids;
    last_ids.resize(plateNum);
    for(QVector3D &c : colors)
        c = QVector3D(prng.generateDouble()*1,prng.generateDouble()*1,prng.generateDouble()*1);

    plates[0].type=OCEANIC;
    plates[1].type=CONTINENTAL;
    for(unsigned short i = 2; i < plateNum; i++)
        plates[i].type = prng.generateDouble() > 0.5 ? OCEANIC : CONTINENTAL;

    for(unsigned short i = 0; i< plateNum; i++)
    {
        unsigned int first_point_of_plate = prng.bounded((unsigned int)pos.size());
        while(std::find(tmp_init.begin(), tmp_init.end(), first_point_of_plate) != tmp_init.end())
        { first_point_of_plate = prng.bounded((unsigned int)pos.size()); }
        tmp_init.push_back(first_point_of_plate);

        vertices[first_point_of_plate].color = colors[i];
        plates[i].points.push_back(first_point_of_plate);
        last_ids[i].push_back(first_point_of_plate);
    }
    while(tmp_init.size() < 100)
    {
        std::cout<<pos.size()<<" | "<<tmp_init.size()<<std::endl;
        for(unsigned short i = 0; i<plateNum; i++)
        {
            if(!last_ids[i].empty())
            {
                unsigned int current_vertex = last_ids[i].back();
                std::cout<<current_vertex<<std::endl;
                std::vector<unsigned int> neighbours = one_ring[current_vertex];
                for(size_t n = 0; n < neighbours.size(); n++)
                {
                    //std::cout<<"current vertex: "<<current_vertex<<"\n neighbour: "<<neighbours[n]<<std::endl;
                    if(std::find(tmp_init.begin(), tmp_init.end(), neighbours[n]) == tmp_init.end())
                    {
                        tmp_init.push_back(neighbours[n]);
                        plates[i].points.push_back(neighbours[n]);
                        vertices[neighbours[n]].color=colors[i];
                        last_ids[i].push_back(neighbours[n]);
                    }
                }
            }
        }
    }
}

void Planet::triangulate()
{
    Point_set points;
    for(Point po: pos)
    {
        points.insert(po);
    }
    typedef std::array<std::size_t, 3> Facet; // Triple of indices
    std::vector<Facet> facets;
    // The function is called using directly the points raw iterators
    CGAL::advancing_front_surface_reconstruction(points.points().begin(),
                                                 points.points().end(),
                                                 std::back_inserter(facets));

    std::cout << facets.size ()
              << " facet(s) generated by reconstruction." << std::endl;
    for(size_t i = 0; i<facets.size(); i++)
    {
        for(size_t j = 0; j<3; j++)
            indices.push_back(facets[i][j]);
    }

    //CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh (pos, facets, mesh);
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
        std::cout << "Error " << id <<", Type: "<< type << " [severity=" << s_severity << "]: "
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
                                indices.size () * sizeof(unsigned int),
                                &indices[0], GL_STATIC_DRAW);

	glFunctions->glEnableVertexAttribArray (0);
    glFunctions->glVertexAttribPointer (0, 3, GL_DOUBLE, GL_FALSE,
                                        sizeof(Vertex), (void*) 0);

	glFunctions->glEnableVertexAttribArray (1);
    glFunctions->glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE,
										sizeof(Vertex),
										(void*) offsetof(Vertex, normal));

	glFunctions->glEnableVertexAttribArray (2);
    glFunctions->glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE,
										sizeof(Vertex),
										(void*) offsetof(Vertex, texCoord));

    glFunctions->glEnableVertexAttribArray (3);
    glFunctions->glVertexAttribPointer (3, 3, GL_FLOAT, GL_FALSE,
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

    glFunctions->glUniform1i(
            glFunctions->glGetUniformLocation(programID, "lighting"),
            this->shaderLigth
            );

	glFunctions->glBindVertexArray (VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}

void Planet::clear ()
{
	if (planetCreated)
	{
		vertices.clear ();
		indices.clear ();
        plates.clear();
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
        /*Vertex v1 = vertices[indices[i]];
		Vertex v2 = vertices[indices[i + 1]];
        Vertex v3 = vertices[indices[i + 2]];*/
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

void Planet::shaderLighting ()
{
    this->shaderLigth = !this->shaderLigth;
}
