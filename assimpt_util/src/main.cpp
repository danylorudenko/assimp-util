#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


struct ImportSettings
{
    bool m_Triangulate = true;
    bool m_ConvertToLeftHanded = true;
    bool m_GenerateNormals = true;
} g_ImportSettings;

struct ExportSettings
{
    bool m_ExportNormals = true;
    bool m_ExportTangent = true;
    bool m_ExportBitangent = true;
    bool m_ExportUV = true;
} g_ExportSettings;


#define OUTPUT_NORMAL
//#define OUTPUT_TANGENT
//#define OUTPUT_BITANGENT
//#define OUTPUT_UV

#include "data.hpp"

void processFile(char const* sourceName, char const* destName);

void recursiveMeshParse(aiNode const* node, aiScene const* scene, std::vector<Mesh>& storage);

Mesh processMesh(aiMesh const* mesh, aiScene const* scene);

ModelHeader provideHeader(std::vector<Mesh>& meshStorage);

void serializeMeshVertices(Mesh const& mesh, std::vector<std::uint8_t>& storage);
void serializeMeshIndicies(Mesh const& mesh, std::vector<std::uint8_t>& storage);

void writeToFile(std::size_t size, char const* data, char const* destName);

int main(int argc, char** argv)
{
    VtxLayout layout;
    layout.AddAttribute<Pos>();
    auto result = layout.Serialize();
    
    
    if (argc == 3) {
        processFile(*(argv + 1), *(argv + 2));
    }
    
    system("pause");
    return 0;
}

void processFile(char const* sourceName, char const* destName) {
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
            serializeMeshVertices(meshStorage[i], vertexStorage);
            serializeMeshIndicies(meshStorage[i], indexStorage);
        }
        ModelHeader header = provideHeader(meshStorage);

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


Mesh processMesh(aiMesh const* mesh, aiScene const* scene)
{
    assert(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE);

    std::uint32_t const vertexCount = static_cast<std::uint32_t>(mesh->mNumVertices);
    std::vector<Vertex> vertices;
    vertices.reserve(vertexCount);
    for (std::uint32_t i = 0; i < vertexCount; i++) {
        Pos vert{};
        Normal norm{};
		Tangent tangent{};
		Bitangent bitangent{};
		UV uv{ 0.0f, 0.0f };

        vert.x = mesh->mVertices[i].x;
        vert.y = mesh->mVertices[i].y;
        vert.z = mesh->mVertices[i].z;

#ifdef OUTPUT_NORMAL
        norm.x = mesh->mNormals[i].x;
        norm.y = mesh->mNormals[i].y;
        norm.z = mesh->mNormals[i].z;
#endif // OUTPUT_NORMAL

#ifdef OUTPUT_TANGENT
		tangent.x = mesh->mTangents[i].x;
		tangent.y = mesh->mTangents[i].y;
		tangent.z = mesh->mTangents[i].z;
#endif // OUTPUT_TANGENT

#ifdef OUTPUT_BITANGENT
		bitangent.x = mesh->mBitangents[i].x;
		bitangent.y = mesh->mBitangents[i].y;
		bitangent.z = mesh->mBitangents[i].z;
#endif // OUTPUT_BITANGENT


		if (mesh->mTextureCoords[0]) {
#ifdef OUTPUT_UV
			uv.u = mesh->mTextureCoords[0][i].x;
			uv.v = mesh->mTextureCoords[0][i].y;
#endif // OUTPUT_UV
		}
//
//        vertices.push_back( Vertex{ 
//            vert,
//#ifdef OUTPUT_NORMAL
//            norm, 
//#endif // OUTPUT_NORMAL
//#ifdef OUTPUT_TANGENT
//            tangent, 
//#endif // OUTPUT_TANGENT
//#ifdef OUTPUT_BITANGENT
//            bitangent, 
//#endif // OUTPUT_BITANGENT
//#ifdef OUTPUT_UV
//            uv 
//#endif // OUTPUT_UV
//        } );
    }

    std::vector<std::uint32_t> indicies;
    std::size_t const faceCount = mesh->mNumFaces;
    indicies.reserve(faceCount);
    for (std::size_t i = 0; i < faceCount; i++) {
        aiFace& face = mesh->mFaces[i];
        for (std::size_t j = 0; j < face.mNumIndices; j++) {
            indicies.push_back(face.mIndices[j]);
        }
    }

    Mesh processedMesh;
    processedMesh.vertices = std::move(vertices);
    processedMesh.indicies = std::move(indicies);

    return std::move(processedMesh);
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

ModelHeader provideHeader(std::vector<Mesh>& meshStorage)
{
    ModelHeader header{};
    header.m_VertexSize = sizeof(Vertex);
    header.m_IndexSize = sizeof(std::uint32_t);
    for (std::size_t i = 0; i < meshStorage.size(); i++) {
        header.m_VertexCount += static_cast<std::uint32_t>(meshStorage[i].vertices.size());
        header.m_IndexCount += static_cast<std::uint32_t>(meshStorage[i].indicies.size());
    }

    return header;
}

void serializeMeshVertices(Mesh const& mesh, std::vector<std::uint8_t>& storage)
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