#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <vector>
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
    plates.clear();
    pos.clear();
    mesh.clear();
    one_ring.clear();
}

void Planet::init ()
{
    planetCreated = false;
    this->plateNum = 5;
    this->radius = 6370;
    this->elems = 6000;
}

void Planet::initGLSL ()
{
	std::filesystem::path fs = std::filesystem::current_path ();
	std::string path = fs.string () + "/GLSL/shaders/";
	std::string vShaderPath = path + "planet.vert";
    std::string fShaderPath = path + "planet.frag";
    mesh.setContext(glContext);
    shader = Shader(glContext,vShaderPath.c_str(),fShaderPath.c_str());
}


void Planet::initPlanet ()
{
    makeSphere (this->radius, elems);
    triangulate();
    makePlates ();
    initElevations();
    mesh.setupMesh(shader);

    planetCreated = true;
}

void Planet::makeSphere (float radius, int elems)
{
    // Calc The Vertices
    pos.reserve(elems);
    std::vector<QVector3D> normals;
    normals.reserve(elems);

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
        pos.push_back( position );
        normals.push_back(normal);
    }

    for(long i = 0; i < elems ; ++i)
    {
        Vertex newVertex = { .pos = pos[i], .normal = normals[i]};
        mesh.vertices.push_back (newVertex);
    }
    pos.shrink_to_fit();
}


void Planet::triangulate()
{
    Point_set points;
    for(const QVector3D &po: pos)
        points.insert(Point(po.x(),po.y(),po.z()));

    typedef std::array<std::size_t, 3> Facet; // Triple of indices
    std::vector<Facet> facets;
    // The function is called using directly the points raw iterators
    CGAL::advancing_front_surface_reconstruction(points.points().begin(),
      points.points().end(),
      std::back_inserter(facets));

    std::cout << facets.size ()
    << " facet(s) generated by reconstruction." << std::endl;
    for(size_t i = 0; i<facets.size(); ++i)
    {
        for(size_t j = 0; j<3; ++j)
            mesh.indices.push_back(facets[i][j]);
    }
}

void collect_one_ring (std::vector<QVector3D> const & i_vertices,
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
                    if(std::find(o_one_ring.begin(), o_one_ring.end(),
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
    collect_one_ring(pos,mesh.indices,one_ring);
    one_ring.shrink_to_fit();

    plates.clear ();
    plates.resize (plateNum);

    QRandomGenerator prng;
    prng.seed(time(nullptr));
    std::set<unsigned int> tmp_init;
    std::vector<QVector3D> colors(plateNum);
    std::vector<std::vector<unsigned int>> last_ids;
    last_ids.resize(plateNum);

    for(QVector3D &c : colors)
        c = QVector3D(prng.generateDouble()*1,prng.generateDouble()*1,prng.generateDouble()*1);

    plates[0].type=OCEANIC;
    plates[1].type=CONTINENTAL;
    for(unsigned short i = 2; i < plateNum; ++i)
        plates[i].type = prng.generateDouble() > 0.5 ? OCEANIC : CONTINENTAL;

    for(unsigned short i = 0; i < plateNum; ++i)
    {
        unsigned int first_point_of_plate = prng.bounded((unsigned int)pos.size()-1);
        while(std::find(tmp_init.begin(), tmp_init.end(), first_point_of_plate) != tmp_init.end())
        { first_point_of_plate = prng.bounded((unsigned int)pos.size()-1); }
        tmp_init.insert(first_point_of_plate);

        mesh.vertices[first_point_of_plate].color = colors[i];
        plates[i].points.push_back(first_point_of_plate);
        last_ids[i] = one_ring[first_point_of_plate];
    }

    std::vector<std::vector<unsigned int>> next_ids;
    next_ids.resize(plateNum);

    while(pos.size() != tmp_init.size())
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
                        next_ids[i].push_back(neighbours[n]);
                    }
                }
            }
            last_ids[i] = next_ids[i];
            next_ids[i].clear();
        }
    }
}

void Planet::initElevations()
{

}

void Planet::changeViewMode ()
{
	this->wireframe = !wireframe;
}


void Planet::drawPlanet(const qglviewer::Camera *camera)
{
    //GPU start
    // Récuperation des matrices de projection / vue-modèle
    float pMatrix[16];
    float mvMatrix[16];
    camera->getProjectionMatrix (pMatrix);
    camera->getModelViewMatrix (mvMatrix);
    float lightColor[3] = {1,1,1};

    shader.use();
    shader.setMat4("proj_matrix",pMatrix);
    shader.setMat4("mv_matrix",mvMatrix);

    shader.setVec3("viewPos", (float)camera->position().x,(float)camera->position().y,(float)camera->position().z);
    shader.setVec3("lightPos",(float)camera->position().x,(float)camera->position().y,(float)camera->position().z);
    shader.setVec3("lightColor", lightColor);
    shader.setBool("lighting", this->shaderLight);

    mesh.Draw(shader);
}

void Planet::draw (const qglviewer::Camera *camera)
{
	if (!planetCreated)
		return;

    drawPlanet(camera);
}

void Planet::clear ()
{
	if (planetCreated)
    {
        plates.clear();
        pos.clear();
        mesh.clear();
        one_ring.clear();

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

    for (unsigned int t = 0; t < mesh.indices.size (); t += 3)
	{
        ostream << "3 " << (mesh.indices[t]) << " " << (mesh.indices[t + 1]) << " "
            << (mesh.indices[t + 2]) << std::endl;
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
  this->shaderLight = !this->shaderLight;
}
