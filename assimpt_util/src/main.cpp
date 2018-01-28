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

Mesh processMesh(aiMesh const* mesh, aiScene const* scene);

void serializeMeshPositions(Mesh const& mesh, std::vector<std::uint8_t>& storage);
void serializeMeshIndicies(Mesh const& mesh, std::vector<std::uint8_t>& storage);

void mapToFile(std::size_t size, std::uint8_t const* data, char const* destName);

int main(int argc, char** argv)
{
    if (argc == 3) {
        processModel(*(argv + 1), *(argv + 2));
    }
    
    system("pause");
    return 0;
}

Mesh processMesh(aiMesh const* mesh, aiScene const * scene)
{
    int const vertexCount = mesh->mNumVertices;
    std::vector<Pos> vertices(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        Pos vert;

        vert.x = mesh->mVertices[i].x;
        vert.y = mesh->mVertices[i].y;
        vert.z = mesh->mVertices[i].z;


        vertices.push_back(vert);
    }

    std::vector<std::size_t> indicies;
    std::size_t const faceCount = mesh->mNumFaces;
    for (std::size_t i = 0; i < faceCount; i++) {
        aiFace face = mesh->mFaces[i];
        for (std::size_t j = 0; j < face.mNumIndices; j++) {
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
    std::cerr << importer.GetErrorString() << std::endl;
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "ASSIMP::IMPORTER::ERROR" << std::endl
            << "Can't read the file " << sourceName << std::endl;
    }
    else {
        std::vector<Mesh> meshStorage;
        recursiveMeshParse(scene->mRootNode, scene, meshStorage);

        std::vector<std::uint8_t> vertexStorage{};
        std::vector<std::uint8_t> indexStorage{};

        std::size_t const meshCount = meshStorage.size();
        for (std::size_t i = 0; i < meshCount; i++) {
            serializeMeshPositions(meshStorage[i], vertexStorage);
            serializeMeshIndicies(meshStorage[i], indexStorage);
        }

        mapToFile(vertexStorage.size(), vertexStorage.data(), destName);
        mapToFile(indexStorage.size(), indexStorage.data(), destName);
    }
}

void recursiveMeshParse(aiNode const* node, aiScene const* scene, std::vector<Mesh>& storage) {
    std::size_t const meshCount = node->mNumMeshes;
    for (std::size_t i = 0; i < meshCount; i++) {
        aiMesh const* mesh = scene->mMeshes[node->mMeshes[i]];
        storage.push_back(processMesh(mesh, scene));
    }

    std::size_t childCount = node->mNumChildren;
    for (std::size_t i = 0; i < childCount; i++) {
        recursiveMeshParse(node->mChildren[i], scene, storage);
    }
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

void mapToFile(std::size_t size, std::uint8_t const* data, char const* destName)
{
    std::ofstream outStream{};
    outStream.open(destName, std::ios_base::app | std::ios_base::binary);

    if (!outStream.is_open()) {
        std::cerr << "STD::OFSTREAM::ERROR" << std::endl
            << "Can't open file " << destName << std::endl;

        return;
    }
    outStream.write(reinterpret_cast<char const*>(&size), sizeof(size));
    outStream.write(reinterpret_cast<char const*>(data), static_cast<std::streamsize>(size));
    outStream.close();
}