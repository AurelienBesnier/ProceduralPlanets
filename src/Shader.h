#ifndef SHADER_H
#define SHADER_H


#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <QVector2D>
#include <QVector3D>

class Shader
{
public:
    unsigned int ID;
    QOpenGLContext *glContext;
    QOpenGLExtraFunctions *glFunctions;

    Shader(){}
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(QOpenGLContext *glContext, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        this->glContext = glContext;
        glFunctions = glContext->extraFunctions();
        glFunctions->glEnable (GL_LIGHT0);
        glFunctions->glEnable (GL_LIGHT1);
        glFunctions->glEnable (GL_LIGHTING);
        glFunctions->glEnable (GL_COLOR_MATERIAL);
        glFunctions->glEnable (GL_BLEND);
        glFunctions->glEnable (GL_TEXTURE_2D);
        glFunctions->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glFunctions->glDebugMessageCallback (&this->MessageCallback, 0);

        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if(geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glFunctions->glCreateShader(GL_VERTEX_SHADER);
        glFunctions->glShaderSource(vertex, 1, &vShaderCode, NULL);
        glFunctions->glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glFunctions->glCreateShader(GL_FRAGMENT_SHADER);
        glFunctions->glShaderSource(fragment, 1, &fShaderCode, NULL);
        glFunctions->glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry=0;
        if(geometryPath != nullptr)
        {
            const char * gShaderCode = geometryCode.c_str();
            geometry = glFunctions->glCreateShader(GL_GEOMETRY_SHADER);
            glFunctions->glShaderSource(geometry, 1, &gShaderCode, NULL);
            glFunctions->glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glFunctions->glCreateProgram();
        glFunctions->glAttachShader(ID, vertex);
        glFunctions->glAttachShader(ID, fragment);
        if(geometryPath != nullptr)
            glFunctions->glAttachShader(ID, geometry);
        glFunctions->glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glFunctions->glDeleteShader(vertex);
        glFunctions->glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glFunctions->glDeleteShader(geometry);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glFunctions->glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glFunctions->glUniform1i(glFunctions->glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glFunctions->glUniform1i(glFunctions->glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glFunctions->glUniform1f(glFunctions->glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const float value[2]) const
    {
        glFunctions->glUniform2fv(glFunctions->glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glFunctions->glUniform2f(glFunctions->glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const float value[3]) const
    {
        glFunctions->glUniform3fv(glFunctions->glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glFunctions->glUniform3f(glFunctions->glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const float value[16]) const
    {
        glFunctions->glUniformMatrix4fv(glFunctions->glGetUniformLocation (ID, name.c_str()), 1, GL_FALSE, value);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type) const
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glFunctions->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glFunctions->glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glFunctions->glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glFunctions->glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }


    bool printShaderErrors (GLuint shader) const
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
            std::cerr << "[OpenGL] Shader error: " << log << std::endl;
            free (log);
        }
        return false;
    }
    bool printProgramErrors (int program) const
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
            std::cerr << "[OpenGL] Program error: " << log << std::endl;
            free (log);
        }
        return false;
    }

    static void MessageCallback (GLenum source, GLenum type,
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
      std::cerr << "Error " << id <<", Source: "<<source<<",  Type: "<< type << ", Length: "<<length<<" [severity=" << s_severity << "]: "
              << message << std::endl;
      }
    }
};
#endif
