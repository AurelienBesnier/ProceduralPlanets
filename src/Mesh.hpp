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
    unsigned int plate_id;
};

class Mesh
{
private:
    QOpenGLBuffer *VBO, *EBO;

public:
    QOpenGLVertexArrayObject *VAO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Mesh(){}

    void Draw(QOpenGLShaderProgram *shader)
    {
        shader->bind();
        VAO->bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
        VAO->release();
        shader->release();
    }

    void clear()
    {
        vertices.clear();
        indices.clear();
        VAO->destroy();
        VBO->destroy();
        EBO->destroy();
    }

    void setupMesh(QOpenGLShaderProgram *shader)
    {
        shader->bind();
        std::cout<<"Creating Mesh Buffers..."<<std::endl;
        VAO = new QOpenGLVertexArrayObject();
        VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        EBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        VAO->create();
        VBO->create();
        EBO->create();

        VAO->bind();

        EBO->bind();
        EBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
        EBO->allocate(indices.data(),sizeof(unsigned int)*indices.size());

        VBO->bind();
        VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
        VBO->allocate(vertices.data(),sizeof(Vertex)*vertices.size());
        shader->enableAttributeArray(0);
        shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(Vertex));

        shader->enableAttributeArray(1);
        shader->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

        shader->enableAttributeArray(2);
        shader->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex));

        VAO->release();
        VBO->release();
        EBO->release();

        std::cout<<"Done!"<<std::endl;
    }
};

#endif
