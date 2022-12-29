#ifndef MESH_H
#define MESH_H

#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>
#include <QVector2D>
#include <QVector3D>
#include <vector>
#include <iostream>

struct Vertex {
    QVector3D pos, normal, color;
};

class Mesh
{
private:
    GLuint VAO, VBO, EBO;

public:
    QOpenGLContext* glContext;
    QOpenGLExtraFunctions *glFunctions;
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;
    Mesh(){}

    void Draw(QOpenGLShaderProgram *shader)
    {
        shader->bind();
        glFunctions->glBindVertexArray (VAO);
        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        glFunctions->glBindVertexArray (0);
        shader->release();
    }

    void setContext(QOpenGLContext* context)
    {
        glContext = context;
        glFunctions = glContext->extraFunctions();
    }

    void clear()
    {
        vertices.clear();
        indices.clear();
    }

    void setupMesh(QOpenGLShaderProgram *shader)
    {
        shader->bind();
        std::cout<<"Creating Mesh Buffers..."<<std::endl;
        glFunctions->glGenVertexArrays (1, &VAO);
        glFunctions->glGenBuffers (1, &VBO);
        glFunctions->glGenBuffers (1, &EBO);

        glFunctions->glBindVertexArray (VAO);

        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
        glFunctions->glBufferData (GL_ELEMENT_ARRAY_BUFFER,
          indices.size () * sizeof(unsigned int),
          indices.data(), GL_STATIC_DRAW);

        glFunctions->glBindBuffer (GL_ARRAY_BUFFER, VBO);
        glFunctions->glBufferData (GL_ARRAY_BUFFER,
                vertices.size () * sizeof(Vertex), vertices.data(),
                GL_STATIC_DRAW);

        glFunctions->glEnableVertexAttribArray (0);
        glFunctions->glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,
          sizeof(Vertex), (void*) 0);

        glFunctions->glEnableVertexAttribArray (1);
        glFunctions->glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE,
                sizeof(Vertex),
                (void*) offsetof(Vertex, normal));

        glFunctions->glEnableVertexAttribArray (2);
        glFunctions->glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE,
          sizeof(Vertex),
          (void*) offsetof(Vertex, color));

        glFunctions->glBindBuffer (GL_ARRAY_BUFFER, 0);
        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        glFunctions->glBindVertexArray (0);
        shader->release();

        vertices.clear();
        std::cout<<"Done!"<<std::endl;
    }
};


#endif