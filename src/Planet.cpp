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

unsigned long long rdtsc(){ // random seed
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

Planet::Planet (QOpenGLContext *context)
{
    glContext = context;
    glFunctions = glContext->extraFunctions();
    float frequency = 5.0f, amplitude = 2.0f, lacunarity = 2.0f,persistence = 0.7f;
    this->noise = SimplexNoise(frequency, amplitude, lacunarity, persistence);
    this->octaveContinent=1;
    this->octaveOcean=1;
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
    this->plateNum = 17;
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);
    glFunctions->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable ( GL_DEBUG_OUTPUT);
    glFunctions->glDebugMessageCallback (Planet::MessageCallback, 0);

    std::filesystem::path fs = std::filesystem::current_path ();
    std::string path = fs.string () + "/GLSL/shaders/";
    QString  vShaderPath = QString::fromStdString(path + "planet.vert");
    QString  fShaderPath = QString::fromStdString(path + "planet.frag");
    if(!program->addShaderFromSourceFile(QOpenGLShader::Vertex,vShaderPath)) [[unlikely]]
    {
        std::cerr<<program->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add VERTEX shader"<<std::endl;
    }
    if(!program->addShaderFromSourceFile(QOpenGLShader::Fragment,fShaderPath))[[unlikely]]
    {
        std::cerr<<program->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add FRAGMENT shader"<<std::endl;
    }

    if(!program->link())[[unlikely]]
    {
        std::cerr<<program->log().toStdString()<<std::endl;
        std::cerr<<"Error linking program"<<std::endl;
    }
    programID = program->programId();

    vShaderPath = QString::fromStdString(path + "ocean.vert");
    fShaderPath = QString::fromStdString(path + "ocean.frag");
    if(!oceanProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,vShaderPath))[[unlikely]]
    {
        std::cerr<<oceanProgram->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add VERTEX shader"<<std::endl;
    }
    if(!oceanProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,fShaderPath))[[unlikely]]
    {
        std::cerr<<oceanProgram->log().toStdString()<<std::endl;
        std::cerr<<"Couldn't add FRAGMENT shader"<<std::endl;
    }

    if(!oceanProgram->link())[[unlikely]]
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

QVector3D normalize(const QVector3D &v)
{        
    double squareLength = v.x()*v.x() + v.y()*v.y() + v.z()*v.z();
    double length = sqrt(squareLength);
    
    return QVector3D(v.x()/length,v.y()/length,v.z()/length);
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

        QVector3D position = QVector3D (x * radius, y * radius, z * radius);
        QVector3D normal = normalize(position);
        pos.push_back(position);
        oceanMesh.vertices[i].pos=position;
        oceanMesh.vertices[i].normal=normal;
    }

    Point_set points;
    for(const QVector3D &po: pos)
        points.insert(Point(po.x(),po.y(),po.z()));

    typedef std::array<std::size_t, 3> Facet; // Triple of indices
    std::vector<Facet> facets;
    
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
        QVector2D texCoord = QVector2D((float )i / elems, (float)i / elems);
        pos[i]=position;
        mesh.vertices[i].pos=position;
        mesh.vertices[i].normal=normal;
        mesh.vertices[i].texCoords = texCoord;
    }
    std::cout<<"Done!"<<std::endl;
}


void Planet::triangulate()
{
    std::cout<<"triangulation started..."<<std::endl;
    Point_set points;
    for(const QVector3D &po: pos)
        points.insert(Point(po[0],po[1],po[2]));

    typedef std::array<std::size_t, 3> Facet; // Triple of indices
    std::vector<Facet> facets;
    
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
                if(j!=y) [[likely]]
                {
                    if(std::find(std::execution::par,o_one_ring.begin(), o_one_ring.end(),
                        o_one_ring[i_triangles[i]]) != o_one_ring.end()) [[likely]]
                    {
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
    
    if(one_ring.size()==0){
        collect_one_ring(pos,mesh.indices,one_ring);
        one_ring.shrink_to_fit();
    }

    plates.clear ();
    plates.resize (plateNum);

    QRandomGenerator prng;
    prng.seed(rdtsc());
    std::set<unsigned int> tmp_init;
    std::vector<std::vector<unsigned int>> last_ids;
    last_ids.resize(plateNum);

    plates[0].type=OCEANIC; plates[0].color = QColor(prng.generateDouble(),prng.generateDouble(),prng.generateDouble());
    plates[1].type=CONTINENTAL; plates[1].color = QColor(prng.generateDouble(),prng.generateDouble(),prng.generateDouble());

    plates[0].mouvement = normalize(QVector3D(prng.generateDouble(),prng.generateDouble(),prng.generateDouble()));
    plates[1].mouvement = normalize(QVector3D(prng.generateDouble(),prng.generateDouble(),prng.generateDouble()));

    for(unsigned short i = 2; i < plateNum; ++i){
        double rng = prng.generateDouble();
        plates[i].type = rng > 0.5 ? OCEANIC : CONTINENTAL;
        plates[i].color = QColor(prng.generateDouble(),prng.generateDouble(),prng.generateDouble());
        plates[i].mouvement = normalize(QVector3D(prng.generateDouble(),prng.generateDouble(),prng.generateDouble()));
    }

    for(unsigned short i = 0; i < plateNum; ++i)
    {
        unsigned int first_point_of_plate = prng.bounded((unsigned int)mesh.vertices.size()-1);
        while(std::find(std::execution::par,tmp_init.begin(), tmp_init.end(), first_point_of_plate) != tmp_init.end())
        { first_point_of_plate = prng.bounded((unsigned int)mesh.vertices.size()-1); }
        tmp_init.insert(first_point_of_plate);

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
                        mesh.vertices[neighbours[n]].plate_id=i;
                        next_ids[i].push_back(neighbours[n]);
                    }
                }
            }
            last_ids[i] = next_ids[i];
            next_ids[i].clear();
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "elapsed time for segmentation: " << elapsed_seconds.count() << "s"<<std::endl;
    std::cout<<"Segmentation finished!"<<std::endl;
}

void Planet::initElevations()
{
    std::cout<<"Initializing plate states..."<<std::endl;
    start = std::chrono::system_clock::now();
    QRandomGenerator prng;
    prng.seed(rdtsc());
    qint32 offsetX = prng.bounded(0,10000), offsetY = prng.bounded(0,10000);
    for(Plate &plate: plates){
        if(plate.type == OCEANIC) // intialize oceanic plate
        {
            for(const unsigned int &point : plate.points)
            { 
                double rng = noise.fractal(octaveOcean,mesh.vertices[point].pos.x()+offsetX,mesh.vertices[point].pos.y()+offsetY)+1.0;
                double elevation = (plateParams.oceanicElevation) * rng;
                mesh.vertices[point].pos = mesh.vertices[point].pos + ((elevation) * mesh.vertices[point].normal); // move the point along the normal's direction
                mesh.vertices[point].elevation = -rng;
            }

        } else { // intialize continental plate
            for(const unsigned int &point : plate.points)
            {
                double rng = noise.fractal(octaveContinent,mesh.vertices[point].pos.x()+offsetX,mesh.vertices[point].pos.y()+offsetY)+0.5;
                double elevation = (plateParams.continentalElevation)* rng;
                mesh.vertices[point].pos = mesh.vertices[point].pos + ((elevation) * mesh.vertices[point].normal);
                mesh.vertices[point].elevation = rng;
            }
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "Initialization time: " << elapsed_seconds.count() << "s\n";
    std::cout<<"Initialization finished!"<<std::endl;
}

void Planet::resetHeights()
{
    for(Plate &plate: plates){ // Reinitialize height of the points
        if(plate.type == OCEANIC)
        {
            for(const unsigned int &point : plate.points)
            {
                double elevation = (plateParams.continentalElevation) * mesh.vertices[point].elevation;
                mesh.vertices[point].pos = mesh.vertices[point].pos - ((elevation) * mesh.vertices[point].normal); // move the point along the normal's direction
                mesh.vertices[point].elevation = 0.0;
            }

        } else { 
            for(const unsigned int &point : plate.points)
            {
                double elevation = (plateParams.continentalElevation) * mesh.vertices[point].elevation;
                mesh.vertices[point].pos = mesh.vertices[point].pos - ((elevation) * mesh.vertices[point].normal);
                mesh.vertices[point].elevation = 0.0;
            }
        }
    }
}

void Planet::resegment()
{
    std::cout<<"Resegmentating..."<<std::endl;
    start = std::chrono::system_clock::now();

    resetHeights();

    needInitBuffers = true;
    makePlates();
    initElevations();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "Resegmentating time: " << elapsed_seconds.count() << "s\n";
    std::cout<<"Resegmentating finished!"<<std::endl;
}

void Planet::reelevateOcean()
{
    for(Plate &plate: plates){ // Reinitialize height of the points
        if(plate.type == OCEANIC)
        {
            for(const unsigned int &point : plate.points)
            {
                double elevation = (plateParams.continentalElevation) * mesh.vertices[point].elevation;
                mesh.vertices[point].pos = mesh.vertices[point].pos - ((elevation) * mesh.vertices[point].normal); // move the point along the normal's direction
                mesh.vertices[point].elevation = 0.0;
            }

        }
    }
    needInitBuffers = true;

    QRandomGenerator prng;
    prng.seed(rdtsc());
    qint32 offsetX = prng.bounded(0,10000), offsetY = prng.bounded(0,10000);
    for(Plate &plate: plates){
        if(plate.type == OCEANIC) // intialize oceanic plate
        {
            for(const unsigned int &point : plate.points)
            { 
                double rng = noise.fractal(octaveOcean,mesh.vertices[point].pos.x()+offsetX,mesh.vertices[point].pos.y()+offsetY)+1.0;
                double elevation = (plateParams.oceanicElevation) * rng;
                mesh.vertices[point].pos = mesh.vertices[point].pos + ((elevation) * mesh.vertices[point].normal); // move the point along the normal's direction
                mesh.vertices[point].elevation = -rng;
            }

        } 
    }

}

void Planet::reelevateContinent()
{
    for(Plate &plate: plates){ // Reinitialize height of the points
        if(plate.type == CONTINENTAL)
        {
            for(const unsigned int &point : plate.points)
            {
                double elevation = (plateParams.continentalElevation) * mesh.vertices[point].elevation;
                mesh.vertices[point].pos = mesh.vertices[point].pos - ((elevation) * mesh.vertices[point].normal);
                mesh.vertices[point].elevation = 0.0;
            }
        }
    }
    needInitBuffers = true;

    QRandomGenerator prng;
    prng.seed(rdtsc());
    qint32 offsetX = prng.bounded(0,10000), offsetY = prng.bounded(0,10000);
    for(Plate &plate: plates){
        if(plate.type == CONTINENTAL) 
        {
            for(const unsigned int &point : plate.points)
            {
                double rng = noise.fractal(octaveContinent,mesh.vertices[point].pos.x()+offsetX,mesh.vertices[point].pos.y()+offsetY)+0.5;
                double elevation = (plateParams.continentalElevation)* rng;
                mesh.vertices[point].pos = mesh.vertices[point].pos + ((elevation) * mesh.vertices[point].normal);
                mesh.vertices[point].elevation = rng;
            }
        }
    }
}

void Planet::move()
{
    std::cout<<"Movement"<<std::endl;
}

float dist(const QVector3D& p, const QVector3D &p2)
{
    float delta_x = p[0] - p2[0];
    float delta_y = p[1] - p2[1];
    float delta_z = p[2] - p2[2];

    return sqrt((delta_x*delta_x) + (delta_y*delta_y) + (delta_z*delta_z));
}

void Planet::closestPoint(QVector3D point)
{
    float cloestDist = std::numeric_limits<float>::max();
    size_t idClosest = 0;

    for(size_t i = 0; i < mesh.vertices.size(); i++)
    {
        const QVector3D &p = mesh.vertices[i].pos;


        float dist_ =dist(p,point);

        if(cloestDist > dist_)
        {
            cloestDist = dist_;
            idClosest = i;
        }
    }
    selectedPlateID = mesh.vertices[idClosest].plate_id;

    std::cout<< "Selected plate n°"<<selectedPlateID<<std::endl;
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
    program->setUniformValue(program->uniformLocation("lighting"), int(shaderLighting));
    program->setUniformValue(program->uniformLocation("selected_plate"), selectedPlateID);
    program->setUniformValue(program->uniformLocation("texRender"), int(textures));

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
    QVector3D lightColor(1,1,1);
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
    oceanProgram->setUniformValue(oceanProgram->uniformLocation("lighting"), int(this->shaderLighting));

    oceanMesh.Draw(oceanProgram);
    oceanProgram->release();
}

void Planet::draw (const qglviewer::Camera *camera)
{
	if (!planetCreated)
		return;

    if(needInitBuffers && planetCreated){
        mesh.setupMesh(program);
        //mesh.setTextures(program);
        if(!oceanMesh.VAO->isCreated())[[unlikely]]
            oceanMesh.setupMesh(oceanProgram);
        oceanMesh.vertices.clear();
        needInitBuffers = false;
    } else { [[likely]]
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
    needInitBuffers = true;

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

void Planet::setOceanicOctave(int _o)
{
    this->octaveOcean = _o;
    std::cout << "oceanic octave set to " << this->octaveOcean << std::endl;
}

void Planet::setContinentalOctave(int _o)
{
    this->octaveContinent = _o;
    std::cout << "continental octave set to " << this->octaveContinent << std::endl;
}