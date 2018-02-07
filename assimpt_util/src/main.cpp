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

VertHeader provideHeader(std::vector<Mesh>& meshStorage);

void serializeMeshPositions(Mesh const& mesh, std::vector<std::uint8_t>& storage);
void serializeMeshIndicies(Mesh const& mesh, std::vector<std::uint8_t>& storage);

void writeToFile(std::size_t size, char const* data, char const* destName);

int main(int argc, char** argv)
{
    if (argc == 3) {
        processModel(*(argv + 1), *(argv + 2));
    }
    
    system("pause");
    return 0;
}

Mesh processMesh(aiMesh const* mesh, aiScene const* scene)
{
    assert(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE);

    int const vertexCount = mesh->mNumVertices;
    std::vector<Vertex> vertices(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        Pos vert{};
        Normal norm{};

        vert.x = mesh->mVertices[i].x;
        vert.y = mesh->mVertices[i].y;
        vert.z = mesh->mVertices[i].z;

        norm.x = mesh->mNormals[i].x;
        norm.y = mesh->mNormals[i].y;
        norm.z = mesh->mNormals[i].z;

        vertices[i] = Vertex{ vert, norm };
    }

    std::vector<std::uint32_t> indicies;
    std::size_t const faceCount = mesh->mNumFaces;
    for (std::size_t i = 0; i < faceCount; i++) {
        aiFace& face = mesh->mFaces[i];
        for (std::size_t j = 0; j < face.mNumIndices; j++) {
            indicies.push_back(face.mIndices[j]);
        }
    }

    Mesh processedMesh;
    processedMesh.vertices = std::move(vertices);
    processedMesh.indicies = std::move(indicies);

    return processedMesh;
}

void processModel(char const* sourceName, char const* destName) {
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(std::string{ sourceName }, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals);
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
        auto header = provideHeader(meshStorage);

        //writeToFile(sizeof(header), reinterpret_cast<char const*>(&header), destName);
        //writeToFile(vertexStorage.size(), reinterpret_cast<char const*>(vertexStorage.data()), destName);
        //writeToFile(indexStorage.size(), reinterpret_cast<char const*>(indexStorage.data()), destName);

        std::ofstream ostream{ destName, std::ios_base::binary | std::ios_base::trunc };
        assert(ostream.is_open());

        ostream.write(reinterpret_cast<char const*>(&header), sizeof(header));
        ostream.write(reinterpret_cast<char const*>(vertexStorage.data()), vertexStorage.size());
        ostream.write(reinterpret_cast<char const*>(indexStorage.data()), indexStorage.size());

        ostream.close();
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

VertHeader provideHeader(std::vector<Mesh>& meshStorage)
{
    VertHeader header{};
    header.vertexSize_ = sizeof(Vertex);
    header.indexSize_ = sizeof(std::uint32_t);
    for (std::size_t i = 0; i < meshStorage.size(); i++) {
        header.vertexCount_ += meshStorage[i].vertices.size();
        header.indexCount_ += meshStorage[i].indicies.size();
    }

    return header;
}

void serializeMeshPositions(Mesh const& mesh, std::vector<std::uint8_t>& storage)
{
    std::uint8_t buffer[sizeof(Vertex)];
    for (std::size_t i = 0; i < mesh.vertices.size(); i++) {
        
        std::memcpy(buffer, &mesh.vertices[i], sizeof(Vertex));

        for (std::size_t j = 0; j < sizeof(Vertex); j++) {
            storage.emplace_back(buffer[j]);
        }
    }
}

void serializeMeshIndicies(Mesh const& mesh, std::vector<std::uint8_t>& storage)
{
    std::uint32_t index;
    std::uint8_t buffer[sizeof(index)];
    for (std::size_t i = 0; i < mesh.indicies.size(); i++) {
        index = mesh.indicies[i];

        std::memcpy(buffer, &index, sizeof(index));

        for (std::size_t j = 0; j < sizeof(index); j++) {
            storage.emplace_back(buffer[j]);
        }
    }
}

void writeToFile(std::size_t size, char const* data, char const* destName)
{
    std::ofstream outStream{};
    outStream.open(destName, std::ios_base::binary);

    if (!outStream.is_open()) {
        std::cerr << "STD::OFSTREAM::ERROR" << std::endl
            << "Can't open file " << destName << std::endl;

        return;
    }

    outStream.seekp(std::ios_base::end);
    outStream.write(data, size);
    outStream.close();
}