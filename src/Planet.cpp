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
    initPlanet();
}

Planet::~Planet(){
    glFunctions->glDeleteVertexArrays(1, &VAO);
    glFunctions->glDeleteBuffers(1, &VBO);
}

void Planet::init(){

    planetCreated = false;

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
    makeSphere(0,0,0,10,10);

    glFunctions->glGenVertexArrays(1, &VAO);
    glFunctions->glGenBuffers(1,&VBO);

    glFunctions->glBindVertexArray(VAO);

    glFunctions->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glFunctions->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float)*3, positions.data(), GL_STATIC_DRAW);

    glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glFunctions->glEnableVertexAttribArray(0);

    glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);

    glFunctions->glBindVertexArray(0);

}

void Planet::setPlateNumber(int _plateNum)
{
    this->plateNum = _plateNum;

    std::cout<<"planet plate number: "<<this->plateNum<<std::endl;
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

void Planet::makeSphere(float x,float y,float z,int slices,int stacks)
{
   if(stacks < 2){stacks = 2;}
   if(stacks > 20){stacks = 20;}
   if(slices < 3){slices = 3;}
   if(slices > 30){slices = 30;}
   //Pas essentiel ...

   int Nb = slices*stacks +2;
   positions.clear();
   positions.resize(Nb);

   qglviewer::Vec centre(x,y,z);

   float sinP , cosP , sinT , cosT , Phi , Theta;
   positions[0] = qglviewer::Vec(0,0,1);
   positions[Nb-1] = qglviewer::Vec(0,0,-1);

   for(int i=1; i<=stacks; i++)
   {
       Phi = 90 - (float)(i*180)/(float)(stacks+1);
       sinP = sinf(Phi*3.14159265/180);
       cosP = cosf(Phi*3.14159265/180);

       for(int j=1; j<=slices; j++)
       {
           Theta = (float)(j*360)/(float)(slices);
           sinT = sinf(Theta*3.14159265/180);
           cosT = cosf(Theta*3.14159265/180);

           positions[ j + (i-1)*slices ] = qglviewer::Vec(cosT*cosP,sinT*cosP,sinP);
       }
   }
}

void Planet::draw( const qglviewer::Camera * camera ){

    if(!planetCreated)
        return;

    glFunctions->glDisable(GL_LIGHTING);

    glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );

    //TODO complete
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

    glFunctions->glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,positions.size());
    glFunctions->glEnable(GL_LIGHTING);
}

void Planet::clear(){
    if( planetCreated )
        init();
}

