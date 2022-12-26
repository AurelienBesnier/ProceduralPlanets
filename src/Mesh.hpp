#ifndef MESH_H
#define MESH_H

#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QVector2D>
#include <QVector3D>
#include <vector>

#include <Shader.h>

struct Vertex {
    QVector3D pos;
    QVector3D normal;
    QVector3D color;
    //unsigned int plate_id;
};

class Mesh
{
private:
    unsigned int VBO, EBO;
    QOpenGLContext *glContext;
    QOpenGLExtraFunctions *glFunctions;

public:
    unsigned int VAO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, QOpenGLContext *glContext)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->glContext = glContext;
        glFunctions = glContext->extraFunctions ();
        setupMesh();
    }
    Mesh(){}

    void Draw(Shader &shader)
    {
        shader.use();
        glFunctions->glBindVertexArray(VAO);
        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
        glPointSize(5);
        glDrawElements(GL_POINT, indices.size(), GL_UNSIGNED_INT, (void*)0);
        glFunctions->glBindVertexArray(0);
        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void setContext(QOpenGLContext *glContext)
    {
        this->glContext = glContext;
        glFunctions = glContext->extraFunctions ();
    }

    void clear()
    {
        vertices.clear();
        indices.clear();
        glFunctions->glDeleteVertexArrays(1,&VAO);
        glFunctions->glDeleteBuffers(1,&VBO);
        glFunctions->glDeleteBuffers(1,&EBO);
    }

    void setupMesh()
    {
        glFunctions->glGenVertexArrays (1, &VAO);
        glFunctions->glGenBuffers (1, &VBO);
        glFunctions->glGenBuffers (1, &EBO);

        glFunctions->glBindVertexArray (VAO);

        glFunctions->glBindBuffer (GL_ARRAY_BUFFER, VBO);
        glFunctions->glBufferData (GL_ARRAY_BUFFER,
                vertices.size () * sizeof(Vertex), vertices.data(),
                GL_STATIC_DRAW);

        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
        glFunctions->glBufferData (GL_ELEMENT_ARRAY_BUFFER,
          indices.size () * sizeof(unsigned int),
          indices.data(), GL_STATIC_DRAW);

        glFunctions->glEnableVertexAttribArray (0);
        glFunctions->glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,
          sizeof(Vertex), (void*) 0);

        glFunctions->glEnableVertexAttribArray (1);
        glFunctions->glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE,
                sizeof(Vertex),
                (void*) offsetof(Vertex, normal));

        glFunctions->glEnableVertexAttribArray (2);
        glFunctions->glVertexAttribPointer (3, 3, GL_FLOAT, GL_FALSE,
          sizeof(Vertex),
          (void*) offsetof(Vertex, color));
        glFunctions->glBindVertexArray(0);
        glFunctions->glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};

#endif // MESH_H
