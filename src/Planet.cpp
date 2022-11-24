#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <QFile>
#include <QTextStream>


#include "Planet.hpp"

Planet::Planet( QOpenGLContext* context )
{
    glContext = context;
    init();
    initGLSL();
}

Planet::~Planet(){
    positions.clear();
    indices.clear();
    glFunctions->glDeleteVertexArrays(1, &VAO);
    glFunctions->glDeleteBuffers(1, &VBO);
    glFunctions->glDeleteProgram(this->programID);
}

void Planet::init(){

    planetCreated = false;
    this->plateNum = 1;
    this->radius = 1;
    this->elems = 20;
}

void Planet::initGLSL(){
    std::filesystem::path fs = std::filesystem::current_path();
    std::string path = fs.string()+"/GLSL/shaders/";
    std::string vShaderPath = path + "planet.vert";
    std::string fShaderPath = path + "planet.frag";

    glFunctions = glContext->extraFunctions();
    glEnable              ( GL_DEBUG_OUTPUT );
    glFunctions->glDebugMessageCallback(&Planet::MessageCallback, 0 );


    glFunctions->glEnable(GL_LIGHT0);
    glFunctions->glEnable(GL_LIGHT1);
    glFunctions->glEnable(GL_LIGHTING);
    glFunctions->glEnable(GL_DEPTH_TEST);
    glFunctions->glEnable(GL_COLOR_MATERIAL);
    glFunctions->glEnable(GL_BLEND);
    glFunctions->glEnable(GL_TEXTURE_2D);
    glFunctions->glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Create programs and link shaders
    this->programID = glFunctions->glCreateProgram();
    std::string content = readShaderSource(vShaderPath);
    if (!content.empty()) {
        this->vShader = glFunctions->glCreateShader(GL_VERTEX_SHADER);
        const char* src = content.c_str();
        glFunctions->glShaderSource(this->vShader, 1, &src, NULL);
        glFunctions->glCompileShader(this->vShader);
        glFunctions->glAttachShader(this->programID, this->vShader);
        printShaderErrors(this->vShader);
    }
    content = readShaderSource(fShaderPath);
    if (!content.empty()) {
        this->fShader = glFunctions->glCreateShader(GL_FRAGMENT_SHADER);
        const char* src = content.c_str();
        glFunctions->glShaderSource(this->fShader, 1, &src, NULL);
        glFunctions->glCompileShader(this->fShader);
        glFunctions->glAttachShader(this->programID, this->fShader);
        printShaderErrors(this->fShader);
    }

    glFunctions->glLinkProgram(this->programID);
    glFunctions->glUseProgram(programID);
    printProgramErrors(programID);
    checkOpenGLError();
}

void Planet::initPlanet()
{
    makeSphere(this->radius,elems, elems);
    makePlates();

    glFunctions->glGenVertexArrays(1, &VAO);
    glFunctions->glGenBuffers(1,&VBO);
    glFunctions->glGenBuffers(1,&EBO);

    glFunctions->glBindVertexArray(VAO);

    glFunctions->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glFunctions->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float)*3, positions.data(), GL_STATIC_DRAW);

    glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glFunctions->glEnableVertexAttribArray(0);

    glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);

    glFunctions->glBindVertexArray(0);

    planetCreated = true;
}

void Planet::makePlates()
{
    if(plates.size() < 1){
        plates.clear();
        plates.resize(plateNum);
        std::vector<unsigned int> tmp_init;
        tmp_init.resize(plateNum);
    }
    
}

void Planet::setPlateNumber(int _plateNum)
{
    this->plateNum = _plateNum;

    std::cout<<"planet plate number: "<<this->plateNum<<std::endl;
}

void Planet::setRadius(double _r)
{
    this->radius = _r;

    std::cout<<"planet radius set to "<<this->radius<<std::endl;
}

void Planet::setElems(int _elems)
{
    this->elems = _elems;

    std::cout<<"planet elems set to "<<this->elems<<std::endl;
}

void Planet::makeSphere(float radius, int slices, int stacks)
{
    // Calc The Vertices
    for (int i = 0; i <= stacks; ++i){

        float V   = i / (float) stacks;
        float phi = V * 3.14159265;

        // Loop Through Slices
        for (int j = 0; j <= slices; ++j){

            float U = j / (float) slices;
            float theta = U * (3.14159265 * 2);

            // Calc The Vertex Positions
            float x = cosf (theta) * sinf (phi);
            float y = cosf (phi);
            float z = sinf (theta) * sinf (phi);

            // Push Back Vertex Data
            positions.push_back (qglviewer::Vec(x, y, z) * radius);
        }
    }

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i){

        indices.push_back (i);
        indices.push_back (i + slices + 1);
        indices.push_back (i + slices);

        indices.push_back (i + slices + 1);
        indices.push_back (i);
        indices.push_back (i + 1);
    }
}

void /*GLAPIENTRY */Planet::MessageCallback( GLenum source, GLenum type,
                                              GLuint id, GLenum severity,
                                              GLsizei length, const GLchar* message,
                                              const void* userParam )
{
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_LOW) {
        std::string s_severity = (severity == GL_DEBUG_SEVERITY_HIGH ? "High" : severity == GL_DEBUG_SEVERITY_MEDIUM ? "Medium" : "Low");
        std::cout << "Error " << id << " [severity=" << s_severity << "]: " << message << std::endl;
    }
}
bool Planet::checkOpenGLError()
{
    bool error = false;
    int glErr = glGetError();
    while(glErr != GL_NO_ERROR)
    {
        std::cout << "[OpenGL] Error: " << glErr << std::endl;
        error = true;
        glErr = glGetError();
    }
    return !error;
}

bool Planet::printShaderErrors(GLuint shader)
{
    int state = 0;
    glFunctions->glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
    if (state == 1)
        return true;
    int len = 0;
    int chWritten = 0;
    char* log;
    glFunctions->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char*)malloc(len);
        glFunctions->glGetShaderInfoLog(shader, len, &chWritten, log);
        std::cout << "[OpenGL] Shader error: " << log << std::endl;
        free(log);
    }
    return false;
}
bool Planet::printProgramErrors(int program)
{
    int state = 0;
    glFunctions->glGetProgramiv(program, GL_LINK_STATUS, &state);
    if (state == 1)
        return true;
    int len = 0;
    int chWritten = 0;
    char* log;
    glFunctions->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char*)malloc(len);
        glFunctions->glGetProgramInfoLog(program, len, &chWritten, log);
        std::cout << "[OpenGL] Program error: " << log << std::endl;
        free(log);
    }
    return false;
}

std::string Planet::readShaderSource(std::string filename)
{
    std::string content = "";
    QString qFilename = QString::fromStdString(filename);
    if (!QFile::exists(qFilename))
        qFilename = ":" + qFilename;
    if (!QFile::exists(qFilename)) {
        std::cerr << "The shader " << filename << " doesn't exist!" << std::endl;
        return "";
    }
    QFile file(qFilename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    std::string line;
    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine().toStdString();
        content += line + " \n";
    }
    file.close();
    return content;
}

void Planet::changeViewMode()
{
    this->wireframe = !wireframe;
}


void Planet::draw( const qglviewer::Camera * camera ){

    if(!planetCreated)
        return;

    glFunctions->glDisable(GL_LIGHTING);

    if(wireframe)
        glPolygonMode( GL_FRONT_AND_BACK , GL_LINE );
    else
        glPolygonMode(GL_FRONT_AND_BACK , GL_FILL );

    //GPU start
    // Récuperation des matrices de projection / vue-modèle
    float pMatrix[16];
    float mvMatrix[16];
    camera->getProjectionMatrix(pMatrix);
    camera->getModelViewMatrix(mvMatrix);
    glFunctions->glUniformMatrix4fv(glFunctions->glGetUniformLocation(programID, "proj_matrix"),
                                   1, GL_FALSE, pMatrix);
    glFunctions->glUniformMatrix4fv(glFunctions->glGetUniformLocation(programID, "mv_matrix"),
                                   1, GL_FALSE, mvMatrix);

    /*glFunctions->glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS,0,positions.size());*/

    glPointSize(4);
    glBegin(GL_TRIANGLES);
    for(size_t i =0; i< indices.size(); i+=3)
    {
        glVertex3f(positions[indices[i]].x,positions[indices[i]].y,positions[indices[i]].z);
        glVertex3f(positions[indices[i+1]].x,positions[indices[i+1]].y,positions[indices[i+1]].z);
        glVertex3f(positions[indices[i+2]].x,positions[indices[i+2]].y,positions[indices[i+2]].z);
    }
    glEnd();

    glFunctions->glEnable(GL_LIGHTING);
}

void Planet::clear(){
    if( planetCreated )
    {
        positions.clear();
        indices.clear();

        glFunctions->glDeleteVertexArrays(1, &VAO);
        glFunctions->glDeleteBuffers(1, &VBO);
        planetCreated = false;
    }
}


void Planet::save()
{
    std::string filename = "planet.obj";
    std::ofstream ostream;

    ostream.open(filename, std::ios_base::out);

    ostream<<"o "<<filename<<std::endl;

    for(size_t i = 0; i< positions.size(); ++i)
    {
        ostream<<"v "<<positions[i].x<<" "<<positions[i].y<<" "<<positions[i].z<<std::endl;
    }

    for(size_t i = 0; i< indices.size(); i+=9)
    {
        ostream<<"f  "<<indices[i]<<"/"<<indices[i+1]<<"/"<<indices[i+2]<<" "
            <<indices[i+3]<<"/"<<indices[i+4]<<"/"<<indices[i+5]<<" "
            <<indices[i+6]<<"/"<<indices[i+7]<<"/"<<indices[i+8]<<std::endl;
    }

    ostream.close();

    std::cout<<"Wrote to file "<<filename<<std::endl;
}
