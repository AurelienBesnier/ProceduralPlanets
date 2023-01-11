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
    QVector3D pos, normal;
    QVector2D texCoords;
    float elevation;
    float plate_id;
};


/**
 * @brief Class reprensenting a Mesh.
 * 
 */
class Mesh
{
private:
    QOpenGLBuffer *VBO=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer), *EBO=new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

public:
    QOpenGLVertexArrayObject *VAO=new QOpenGLVertexArrayObject();
    std::vector<Vertex> vertices;
    std::vector<unsigned int> textures;
    std::vector<unsigned int> indices;
    Mesh(){}


    /**
     * @brief Draws the mesh with the shader param.
     * 
     * @param shader 
     */
    void Draw(QOpenGLShaderProgram *shader)
    {
        shader->bind();
        VAO->bind();
        EBO->bind();
        if(textures.size() != 0)
        {
            for(size_t i = 0; i<textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0+textures[i]);
                glBindTexture(GL_TEXTURE_2D, textures[i]);
            }
        }
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
        VAO->release();
        EBO->release();
        shader->release();
    }

    void setTextures(QOpenGLShaderProgram *shader)
    {
        shader->bind();

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        QImage texture = QImage("./res/snow_02_diff_2k.jpg");
        texture = texture.convertToFormat(QImage::Format_RGB888);
        unsigned char *data = texture.bits();
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 
                        0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            std::cout << "tex failed "<< std::endl;
        }
        
        textures.push_back(textureID);
        
        shader->setUniformValue(shader->uniformLocation("snow"), textureID);

        unsigned int textureID2;
        glGenTextures(1, &textureID2);
        glBindTexture(GL_TEXTURE_2D, textureID2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        texture = QImage("./res/rocks_ground_05_diff_2k.jpg");
        texture = texture.convertToFormat(QImage::Format_RGB888);
        unsigned char *data2 = texture.bits();
        if (data2)
        {
            glTexImage2D(GL_TEXTURE_2D, 
                        0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data2
            );
        }
        else
        {
            std::cout << "tex failed "<< std::endl;
        }
        
        textures.push_back(textureID2);
        
        shader->setUniformValue(shader->uniformLocation("rocks"), textureID2);


        unsigned int textureID3;
        glGenTextures(1, &textureID3);
        glBindTexture(GL_TEXTURE_2D, textureID3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        texture = QImage("./res/coast_sand_01_diff_2k.jpg");
        texture = texture.convertToFormat(QImage::Format_RGB888);
        unsigned char *data3 = texture.bits();
        if (data3)
        {
            glTexImage2D(GL_TEXTURE_2D, 
                        0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data3
            );
        }
        else
        {
            std::cout << "tex failed "<< std::endl;
        }
        textures.push_back(textureID3);
        
        shader->setUniformValue(shader->uniformLocation("sand"), textureID3);


        unsigned int textureID4;
        glGenTextures(1, &textureID4);
        glBindTexture(GL_TEXTURE_2D, textureID4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        texture = QImage("./res/coast_sand_rocks_02_diff_2k.jpg");
        texture = texture.convertToFormat(QImage::Format_RGB888);
        unsigned char *data4 = texture.bits();
        if (data4)
        {
            glTexImage2D(GL_TEXTURE_2D, 
                        0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data4
            );
        }
        else
        {
            std::cout << "tex failed "<< std::endl;
        }
        
        textures.push_back(textureID4);
        
        shader->setUniformValue(shader->uniformLocation("grass"), textureID4);
        shader->release();
    }

    /**
     * @brief Clears all the mesh data
     * 
     */
    void clear()
    {
        vertices.clear();
        textures.clear();
        indices.clear();
        VAO->destroy();
        VBO->destroy();
        EBO->destroy();
    }

    /**
     * @brief Setup the mesh with the shader param.
     * 
     * @param shader 
     */
    void setupMesh(QOpenGLShaderProgram *shader)
    {
        shader->bind();
        std::cout<<"Creating Mesh Buffers..."<<std::endl;

        if(VAO->isCreated())
        {
            VAO->destroy();
            VBO->destroy();
            EBO->destroy();
        }
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
        shader->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texCoords), 2, sizeof(Vertex));
        
        shader->enableAttributeArray(3);
        shader->setAttributeBuffer(3, GL_FLOAT, offsetof(Vertex, elevation), 1, sizeof(Vertex));

        shader->enableAttributeArray(4);
        shader->setAttributeBuffer(4, GL_FLOAT, offsetof(Vertex, plate_id), 1, sizeof(Vertex));

        VAO->release();
        VBO->release();
        EBO->release();

        std::cout<<"Done!"<<std::endl;
    }
};

#endif
