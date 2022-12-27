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


class Mesh
{
private:
    QOpenGLBuffer *verticesVBO, *normalsVBO, *colorVBO, *EBO;

public:
    QOpenGLVertexArrayObject *VAO;
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> normals;
    std::vector<QVector3D> colors;
    std::vector<unsigned int> indices;
    Mesh(){}

    void Draw()
    {
        VAO->bind();
        EBO->bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
        VAO->release();
        EBO->release();
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
        VAO = new QOpenGLVertexArrayObject();
        verticesVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        normalsVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        colorVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        EBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        VAO->create();
        verticesVBO->create();
        normalsVBO->create();
        colorVBO->create();
        EBO->create();

        VAO->bind();

        EBO->bind();
        EBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
        EBO->allocate(indices.data(),sizeof(unsigned int)*indices.size());

        verticesVBO->bind();
        verticesVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
        verticesVBO->allocate(vertices.data(),sizeof(QVector3D)*vertices.size());
        shader->enableAttributeArray(0);
        shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));

        normalsVBO->bind();
        normalsVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
        normalsVBO->allocate(normals.data(),sizeof(QVector3D)*normals.size());
        shader->enableAttributeArray(1);
        shader->setAttributeBuffer(1, GL_FLOAT, 0, 3, sizeof(QVector3D));

        colorVBO->bind();
        colorVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
        colorVBO->allocate(colors.data(),sizeof(QVector3D)*colors.size());
        shader->enableAttributeArray(2);
        shader->setAttributeBuffer(2, GL_FLOAT, 0, 3, sizeof(QVector3D));

        VAO->release();
        std::cout<<"Done!"<<std::endl;
    }
};

#endif // MESH_H
