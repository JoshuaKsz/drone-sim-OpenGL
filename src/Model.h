#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>
#include <iostream>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class Model
{
public:
    Model(const char* path)
    {
        loadModel(path);
    }

    ~Model()
    {
        // Cleanup
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void Draw()
    {
        // Bind VAO and draw the model
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;

    void loadModel(const char* path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::string base_dir = path;
        base_dir = base_dir.substr(0, base_dir.find_last_of("/\\") + 1);

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, base_dir.c_str()))
        {
            std::cerr << "Failed to load/parse .obj file!" << std::endl;
            std::cerr << warn << err << std::endl;
            return;
        }

        for (const auto& shape : shapes)
        {
            for (size_t f = 0; f < shape.mesh.indices.size(); f++)
            {
                tinyobj::index_t idx = shape.mesh.indices[f];
                Vertex vertex = {};

                vertex.position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };

                // Assign color from material if available
                if (!materials.empty())
                {
                    int material_id = shape.mesh.material_ids[f / 3];
                    if (material_id >= 0)
                    {
                        tinyobj::material_t material = materials[material_id];
                        vertex.color = {
                            material.diffuse[0],
                            material.diffuse[1],
                            material.diffuse[2]
                        };
                    }
                }
                else
                {
                    vertex.color = {1.0f, 1.0f, 1.0f}; // Default to white if no material
                }

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }

        // Generate buffers and arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif
