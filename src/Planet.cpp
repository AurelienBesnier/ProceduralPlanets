#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <execution>
#include <cassert>
#include <vector>
#include <QRandomGenerator>

#include "Planet.hpp"
#include "Plate.hpp"

#define PI 3.14159265358979323846

Planet::Planet (QOpenGLContext *context)
{
    glContext = context;
    glFunctions = glContext->extraFunctions();
    init ();
    initGLSL ();
}

Planet::~Planet ()
{
    plates.clear();
    one_ring.clear();
}

void Planet::init ()
{
    planetCreated = false;
    needInitBuffers = true;
    program = new QOpenGLShaderProgram();
    oceanProgram = new QOpenGLShaderProgram();
    this->plateNum = 4;
    this->radius = 6370*1000;
    this->elems = 6000;
    this->plateParams.oceanicElevation=-10*1000;
    this->plateParams.continentalElevation=10*1000;
}

void Planet::initGLSL ()
{
    glFunctions->glEnable (GL_LIGHT0);
    glFunctions->glEnable (GL_LIGHT1);
    glFunctions->glEnable (GL_LIGHTING);
    glFunctions->glEnable (GL_COLOR_MATERIAL);
    glFunctions->glEnable (GL_BLEND);
    glFunctions->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable ( GL_DEBUG_OUTPUT);
    glFunctions->glDebugMessageCallback (Planet::MessageCallback, 0);

    std::filesystem::path fs = std::filesystem::current_path ();
    std::string path = fs.string () + "/GLSL/shaders/";
    QString  vShaderPath = QString::fromStdString(path + "planet.vert");
    QString  fShaderPath = QString::fromStdString(path + "planet.frag");
    if(!program->addShaderFromSourceFile(QOpenGLShader::Vertex,vShaderPath))
    {
        std::cerr<<program->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add VERTEX shader"<<std::endl;
    }
    if(!program->addShaderFromSourceFile(QOpenGLShader::Fragment,fShaderPath))
    {
        std::cerr<<program->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add FRAGMENT shader"<<std::endl;
    }

    if(!program->link())
    {
        std::cerr<<program->log().toStdString()<<std::endl;
        std::cerr<<"Error linking program"<<std::endl;
    }
    programID = program->programId();

    vShaderPath = QString::fromStdString(path + "ocean.vert");
    fShaderPath = QString::fromStdString(path + "ocean.frag");
    if(!oceanProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,vShaderPath))
    {
        std::cerr<<oceanProgram->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add VERTEX shader"<<std::endl;
    }
    if(!oceanProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,fShaderPath))
    {
        std::cerr<<oceanProgram->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add FRAGMENT shader"<<std::endl;
    }

    if(!oceanProgram->link())
    {
        std::cerr<<oceanProgram->log().toStdString()<<std::endl;
        std::cerr<<"Error linking program"<<std::endl;
    }
    oceanProgramID = oceanProgram->programId();
}

void Planet::initPlanet ()
{
    makeSphere ();
    triangulate();
    makeOcean();
    makePlates ();
    initElevations();

    planetCreated = true;
}

void Planet::makeOcean ()
{
    std::cout<<"Making ocean mesh..."<<std::endl;
    oceanMesh.vertices.resize(elems);
    std::vector<QVector3D> pos;

    double goldenRatio = (1 + pow(5,0.5))/2;

    for (long i = 0; i < elems; ++i)
    {
        double theta = 2 * PI * i / goldenRatio;
        double phi = acosf(1 - 2 * (i+0.5f) / elems);
        double x = cosf(theta)*sinf(phi), y=sinf(theta)*sinf(phi), z=cosf(phi);

        QVector3D position = QVector3D (x * radius, y * radius, z * radius) ;
        double squareLength = position.x()*position.x() + position.y()*position.y() + position.z()*position.z(); ;
        double length = sqrt(squareLength);
        QVector3D normal = QVector3D(position.x()/length,position.y()/length,position.z()/length);
        pos.push_back(position);
        oceanMesh.vertices[i].pos=position;
        oceanMesh.vertices[i].normal=normal;
        oceanMesh.vertices[i].color=QVector3D(0,0,1);
    }

    Point_set points;
    for(const QVector3D &po: pos)
        points.insert(Point(po.x(),po.y(),po.z()));

    typedef std::array<std::size_t, 3> Facet; // Triple of indices
    std::vector<Facet> facets;
    // The function is called using directly the points raw iterators
    
    CGAL::advancing_front_surface_reconstruction(points.points().begin(),
      points.points().end(),
      std::back_inserter(facets));
    for(size_t i = 0; i<facets.size(); ++i)
    {
        for(size_t j = 0; j<3; ++j)
            oceanMesh.indices.push_back(facets[i][j]);
    }

    std::cout<<"Done!"<<std::endl;
}

void Planet::makeSphere ()
{
    std::cout<<"Making the points of the sphere..."<<std::endl;
    // Calc The Vertices
    mesh.vertices.resize(elems);
    pos.clear();
    pos.resize(elems);

    double goldenRatio = (1 + pow(5,0.5))/2;

    for (long i = 0; i < elems; ++i)
    {
        double theta = 2 * PI * i / goldenRatio;
        double phi = acosf(1 - 2 * (i+0.5f) / elems);
        double x = cosf(theta)*sinf(phi), y=sinf(theta)*sinf(phi), z=cosf(phi);

        QVector3D position = QVector3D (x * radius, y * radius, z * radius) ;
        double squareLength = position.x()*position.x() + position.y()*position.y() + position.z()*position.z(); ;
        double length = sqrt(squareLength);
        QVector3D normal = QVector3D(position.x()/length,position.y()/length,position.z()/length);
        pos[i]=position;
        mesh.vertices[i].pos=position;
        mesh.vertices[i].normal=normal;
    }

    std::cout<<"Done!"<<std::endl;
}


void Planet::triangulate()
{
    std::cout<<"triangulation started..."<<std::endl;
    Point_set points;
    for(const QVector3D &po: pos)
        points.insert(Point(po.x(),po.y(),po.z()));

    typedef std::array<std::size_t, 3> Facet; // Triple of indices
    std::vector<Facet> facets;
    // The function is called using directly the points raw iterators
    
    start = std::chrono::system_clock::now();

    CGAL::advancing_front_surface_reconstruction(points.points().begin(),
      points.points().end(),
      std::back_inserter(facets));

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "elapsed time for triangulation: " << elapsed_seconds.count() << "s\n";

    std::cout << facets.size ()
    << " facet(s) generated by reconstruction." << std::endl;
    for(size_t i = 0; i<facets.size(); ++i)
    {
        for(size_t j = 0; j<3; ++j)
            mesh.indices.push_back(facets[i][j]);
    }
    std::cout<<"triangulation finished!"<<std::endl;
}

void Planet::collect_one_ring (std::vector<QVector3D> const & i_vertices,
    std::vector< unsigned int > const & i_triangles,
    std::vector<std::vector<unsigned int> > & o_one_ring) {
    o_one_ring.clear();
    o_one_ring.resize(i_vertices.size());
    size_t i, j, y, size=i_triangles.size();
    for(i = 0; i<size; i+=3)
    {
        for(j = 0; j<3; ++j) //sommet courant
        {
            for(y = 0; y<3 ;++y) // sommets voisins
            {
                if(j!=y)
                {
                    if(std::find(std::execution::par,o_one_ring.begin(), o_one_ring.end(),
                        o_one_ring[i_triangles[i]]) != o_one_ring.end()){
                        o_one_ring[i_triangles[i+j]].push_back(i_triangles[i+y]);
                    }
                }
            }
        }
    }
}

void Planet::makePlates ()
{
    std::cout<<"Segmentation started..."<<std::endl;
    start = std::chrono::system_clock::now();
    collect_one_ring(pos,mesh.indices,one_ring);
    end = std::chrono::system_clock::now();
    one_ring.shrink_to_fit();

    elapsed_seconds = end - start;
    std::cout << "elapsed time for one ring computation: " << elapsed_seconds.count() << "s\n";

    plates.clear ();
    plates.resize (plateNum);

    QRandomGenerator prng;
    prng.seed(time(nullptr));
    std::set<unsigned int> tmp_init;
    std::vector<QVector3D> colors(plateNum);
    std::vector<std::vector<unsigned int>> last_ids;
    last_ids.resize(plateNum);

    plates[0].type=OCEANIC; colors[0] = QVector3D(0,0,1);
    plates[1].type=CONTINENTAL; colors[1] = QVector3D(0,1,0);
    for(unsigned short i = 2; i < plateNum; ++i){
        double rng = prng.generateDouble();
        plates[i].type = rng > 0.5 ? OCEANIC : CONTINENTAL;
        colors[i] = rng > 0.5 ? QVector3D(0,0,1) : QVector3D(0,1,0);
    }

    for(unsigned short i = 0; i < plateNum; ++i)
    {
        unsigned int first_point_of_plate = prng.bounded((unsigned int)mesh.vertices.size()-1);
        while(std::find(std::execution::par,tmp_init.begin(), tmp_init.end(), first_point_of_plate) != tmp_init.end())
        { first_point_of_plate = prng.bounded((unsigned int)mesh.vertices.size()-1); }
        tmp_init.insert(first_point_of_plate);

        mesh.vertices[first_point_of_plate].color = colors[i];
        mesh.vertices[first_point_of_plate].plate_id = i;
        plates[i].points.push_back(first_point_of_plate);
        last_ids[i] = one_ring[first_point_of_plate];
    }

    std::vector<std::vector<unsigned int>> next_ids;
    next_ids.resize(plateNum);

    while(mesh.vertices.size() != tmp_init.size())
    {
        for(unsigned short i = 0; i<plateNum; ++i)
        {
            while(!last_ids[i].empty())
            {
                unsigned int current_vertex = last_ids[i].back();
                last_ids[i].pop_back();
                std::vector<unsigned int> neighbours = one_ring[current_vertex];
                for(size_t n = 0; n < neighbours.size(); ++n)
                {
                    if(!tmp_init.contains(neighbours[n]))
                    {
                        tmp_init.insert(neighbours[n]);
                        plates[i].points.push_back(neighbours[n]);
                        mesh.vertices[neighbours[n]].color=colors[i];
                        mesh.vertices[neighbours[n]].plate_id=i;
                        next_ids[i].push_back(neighbours[n]);
                    }
                }
            }
            last_ids[i] = next_ids[i];
            next_ids[i].clear();
        }
    }

    for(size_t i = 0; i< mesh.vertices.size(); ++i){ 
      std::vector<unsigned int> neighbours = one_ring[i];
      for(size_t n = 0; n < neighbours.size(); ++n)
      {
        if(mesh.vertices[neighbours[n]].plate_id!= mesh.vertices[i].plate_id && mesh.vertices[neighbours[n]].color != QVector3D(0,0,0))
        {
          mesh.vertices[neighbours[n]].color=QVector3D(0,0,0);
        }
      }
    }
    std::cout<<"Segmentation finished!"<<std::endl;
}

void Planet::initElevations()
{
    std::cout<<"Initializing plate states..."<<std::endl;
    start = std::chrono::system_clock::now();
    QRandomGenerator prng;
    prng.seed(time(nullptr));
    for(Plate &plate: plates){
        if(plate.type == OCEANIC) // intialize oceanic plate
        {
            for(unsigned int &point : plate.points)
            {
                double rng = prng.generateDouble();
                if(rng < 0.2f)
                    mesh.vertices[point].color = QVector3D(0.0f,0.0f,0.2f);
                if(rng >= 0.2f && rng < 0.8f)
                    mesh.vertices[point].color = QVector3D(0.0f,0.0f,0.8f);
                if( rng >= 0.8f)
                    mesh.vertices[point].color = QVector3D(0.0f,0.3f,0.8f);
                double elevation = (plateParams.oceanicElevation) * rng;
                mesh.vertices[point].pos = mesh.vertices[point].pos + ((elevation) * mesh.vertices[point].normal); // move the point along the normal's direction
            }

        } else { // intialize continental plate
            for(unsigned int &point : plate.points)
            {
                double rng = prng.generateDouble();
                if(rng < 0.2f)
                    mesh.vertices[point].color = QVector3D(0.6f,0.5f,0.1f);
                if(rng >= 0.2f && rng < 0.8f)
                    mesh.vertices[point].color = QVector3D(0.1f,0.7f,0.1f);
                if( rng >= 0.8f)
                    mesh.vertices[point].color = QVector3D(1.0f,1.0f,1.0f);
                double elevation = (plateParams.continentalElevation)* rng;
                mesh.vertices[point].pos = mesh.vertices[point].pos + ((elevation) * mesh.vertices[point].normal);
            }
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "Initialization time: " << elapsed_seconds.count() << "s\n";
    std::cout<<"Initialization finished!"<<std::endl;
}


void Planet::drawPlanet(const qglviewer::Camera *camera)
{
    float pMatrix[16];
    float mvMatrix[16];
    camera->getProjectionMatrix (pMatrix);
    camera->getModelViewMatrix (mvMatrix);

    QVector3D camPos(camera->position().x,camera->position().y,camera->position().z);
    QVector3D lightColor(1,0.9,0.8);
    program->bind();

    glFunctions->glUniformMatrix4fv (
             glFunctions->glGetUniformLocation (programID, "proj_matrix"), 1,
             GL_FALSE,
             pMatrix);
    glFunctions->glUniformMatrix4fv (
             glFunctions->glGetUniformLocation (programID, "mv_matrix"), 1,
             GL_FALSE,
             mvMatrix);
    program->setUniformValue(program->uniformLocation("lightColor"), lightColor);
    program->setUniformValue(program->uniformLocation("viewPos"), camPos);
    program->setUniformValue(program->uniformLocation("lightPos"), camPos);
    program->setUniformValue(program->uniformLocation("lighting"), int(this->shaderLighting));

    mesh.Draw(program);
    program->release();
}

void Planet::drawOcean(const qglviewer::Camera *camera)
{
    float pMatrix[16];
    float mvMatrix[16];
    camera->getProjectionMatrix (pMatrix);
    camera->getModelViewMatrix (mvMatrix);

    QVector3D camPos(camera->position().x,camera->position().y,camera->position().z);
    QVector3D lightColor(1,0.9,0.8);
    oceanProgram->bind();

    glFunctions->glUniformMatrix4fv (
             glFunctions->glGetUniformLocation (oceanProgramID, "proj_matrix"), 1,
             GL_FALSE,
             pMatrix);
    glFunctions->glUniformMatrix4fv (
             glFunctions->glGetUniformLocation (oceanProgramID, "mv_matrix"), 1,
             GL_FALSE,
             mvMatrix);
    oceanProgram->setUniformValue(oceanProgram->uniformLocation("lightColor"), lightColor);
    oceanProgram->setUniformValue(oceanProgram->uniformLocation("viewPos"), camPos);
    oceanProgram->setUniformValue(oceanProgram->uniformLocation("lightPos"), camPos);
    oceanProgram->setUniformValue(oceanProgram->uniformLocation("lighting"), int(false));

    oceanMesh.Draw(oceanProgram);
    oceanProgram->release();
}

void Planet::draw (const qglviewer::Camera *camera)
{
	if (!planetCreated)
		return;

    if(needInitBuffers && planetCreated){
        mesh.setupMesh(program);
        oceanMesh.setupMesh(oceanProgram);
        oceanMesh.vertices.clear();
        needInitBuffers = false;
    } else {
        if(oceanDraw)
            drawOcean(camera);
        drawPlanet(camera);
    }
}

void Planet::clear ()
{
	if (planetCreated)
    {
        plates.clear();
        mesh.clear();
        oceanMesh.clear();
        one_ring.clear();

		planetCreated = false;
        needInitBuffers = true;
	}
}

void Planet::save () const
{
	std::string filename = "planet.obj";
	std::ofstream ostream;
	ostream.precision (4);

	ostream.open (filename, std::ios_base::out);
	ostream << "o planet" << std::endl;
    for (size_t i = 0; i < mesh.vertices.size (); ++i)
	{
        ostream << "v " << std::fixed << mesh.vertices[i].pos.x () << " "
            << std::fixed << mesh.vertices[i].pos.y () << " " << std::fixed
            << mesh.vertices[i].pos.z () << std::endl;
	}

    for (size_t i = 0; i < mesh.vertices.size (); ++i)
	{
        ostream << "vn " << mesh.vertices[i].normal.x () << " "
            << mesh.vertices[i].normal.y () << " " << mesh.vertices[i].normal.z ()
			<< std::endl;
    }

    for (size_t i = 0; i < mesh.indices.size (); i += 3)
    {
        ostream << "f " << mesh.indices[i] << " " << mesh.indices[i + 1] << " "
            << mesh.indices[i + 2] << std::endl;
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
    ostream << (mesh.vertices.size ()) << " " << mesh.indices.size () << " 0"
		<< std::endl;

    for (size_t i = 0; i < mesh.vertices.size (); ++i)
	{
        ostream << mesh.vertices[i].pos.x () << " " << mesh.vertices[i].pos.y () << " "
            << mesh.vertices[i].pos.z () << std::endl;
	}

    for (size_t t = 0; t < mesh.indices.size (); t += 3)
	{
        ostream << "3 " << (mesh.indices[t]) << " " << (mesh.indices[t + 1]) << " "
            << (mesh.indices[t + 2]) << std::endl;
	}

	ostream.close ();

    std::cout << "Wrote to file " << filename << std::endl;
}

void Planet::setOceanicElevation (double _e)
{
  plateParams.oceanicElevation = _e*1000;
  std::cout << "Oceanic elevation: " << this->plateParams.oceanicElevation
    << std::endl;
}

void Planet::setContinentalElevation (double _e)
{
  plateParams.continentalElevation = _e*1000;
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
  this->radius = _r*1000;

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

