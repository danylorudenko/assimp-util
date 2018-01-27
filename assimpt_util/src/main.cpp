#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "data.hpp"

void processModel(char const* sourceName, char const* destName);

void recursiveMeshParse(aiNode const* node, aiScene const* scene, std::vector<Mesh>& storage);

Mesh processMesh(aiMesh* mesh, const aiScene* scene);

void serializeMeshPositions(Mesh const& mesh, std::vector<std::uint8_t>& storage);
void serializeMeshIndicies(Mesh const& mesh, std::vector<std::uint8_t>& storage);

int main(int argc, char** argv)
{
    std::vector<Mesh> meshes;

    if (argc == 2) {
        //meshes = processModel(*(argv + 1));
    }
    
    system("pause");
    return 0;
}

Mesh processMesh(aiMesh* mesh, const aiScene* scene)
{
    int const vertexCount = mesh->mNumVertices;
    std::vector<Pos> vertices(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        Pos vert;

        vert.x = mesh->mVertices[i].x;
        vert.y = mesh->mVertices[i].y;
        vert.z = mesh->mVertices[i].z;

        //vert.normal[0] = mesh->mNormals[i].x;
        //vert.normal[1] = mesh->mNormals[i].y;
        //vert.normal[2] = mesh->mNormals[i].z;

        //if (mesh->mTextureCoords[0]) {
        //    vert.uv[0] = mesh->mTextureCoords[0][i].x;
        //    vert.uv[1] = mesh->mTextureCoords[0][i].y;
        //}
        //else {
        //    vert.uv[0] = 0.0f;
        //    vert.uv[1] = 0.0f;
        //}

        vertices.push_back(vert);
    }

    std::vector<std::size_t> indicies;
    size_t const faceCount = mesh->mNumFaces;
    for (size_t i = 0; i < faceCount; i++) {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++) {
            indicies.push_back(face.mIndices[j]);
        }
    }

    Mesh processedMesh;
    processedMesh.vertices = std::move(vertices);
    processedMesh.indicies = std::move(indicies);
    //processedMesh.material = mesh->mMaterialIndex;

    return processedMesh;
}

void processModel(char const* sourceName, char const* destName) {
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(std::string{ sourceName }, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "ASSIMP::IMPORTER::ERROR" << std::endl
            << "Can't read the file " << sourceName << std::endl;
    }
    else {
        std::vector<Mesh> storage;
        recursiveMeshParse(scene->mRootNode, scene, storage);
    }
}

void recursiveMeshParse(aiNode const* mesh, aiScene const* scene, std::vector<Mesh>& storage) {

}

void serializeMeshPositions(Mesh const& mesh, std::vector<std::uint8_t>& storage)
{
    float position[3];
    std::uint8_t buffer[sizeof(position)];
    for (std::size_t i = 0; i < mesh.vertices.size(); i++) {
        position[0] = mesh.vertices[i].x;
        position[1] = mesh.vertices[i].y;
        position[2] = mesh.vertices[i].z;

        for (std::size_t j = 0; j < sizeof(position); j++) {
            storage.emplace_back(buffer[j]);
        }
    }
}

void serializeMeshIndicies(Mesh const& mesh, std::vector<std::uint8_t>& storage)
{
    std::size_t index;
    std::uint8_t buffer[sizeof(index)];
    for (std::size_t i = 0; i < mesh.indicies.size(); i++) {
        index = mesh.indicies[i];

        for (std::size_t j = 0; j < sizeof(index); j++) {
            storage.emplace_back(buffer[j]);
        }
    }
}