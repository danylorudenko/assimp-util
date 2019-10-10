#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "data.hpp"


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


VtxLayout g_VertexLayout;



void processFile(char const* sourceName, char const* destName);

void recursiveMeshParse(aiNode const* node, aiScene const* scene, std::vector<Mesh>& storage);

Mesh processMesh(aiMesh const* mesh, aiScene const* scene);

ModelHeader provideHeader(std::vector<Mesh>& meshStorage);

void serializeMeshVertices(Mesh const& mesh, std::vector<std::byte>& storage);
void serializeMeshIndicies(Mesh const& mesh, std::vector<std::byte>& storage);

void writeToFile(std::size_t size, char const* data, char const* destName);

template<typename T>
void binaryWritePOD(std::vector<std::byte>& storage, T* pod)
{
    storage.insert(
        vertices.end(),
        reinterpret_cast<std::byte*>(&pod),
        reinterpret_cast<std::byte*>(&pod) + sizeof(T)
    );
}

int main(int argc, char** argv)
{
    g_VertexLayout.AddAttribute<Pos>();
    g_VertexLayout.AddAttribute<Normal>();
    g_VertexLayout.AddAttribute<UV>();
    
    
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

        std::vector<std::byte> vertexStorage{};
        std::vector<std::byte> indexStorage{};

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
    std::vector<std::byte> vertices;
    vertices.reserve(vertexCount);
    for (std::uint32_t i = 0; i < vertexCount; i++) {
        // iterate trough attributes to write them in right order
        if(g_VertexLayout.HasAttributeTypes(VERTEX_ATTRIBUTE_TYPE_POSITION))
        {
            Pos vert{};
            vert.x = mesh->mVertices[i].x;
            vert.y = mesh->mVertices[i].y;
            vert.z = mesh->mVertices[i].z;

            binaryWritePOD(vertices, &vert);
        }

        if (g_VertexLayout.HasAttributeTypes(VERTEX_ATTRIBUTE_TYPE_NORMAL))
        {
            assert(mesh->mNormals != nullptr && "There are no normals to read!");
            Normal norm{};
            norm.x = mesh->mNormals[i].x;
            norm.y = mesh->mNormals[i].y;
            norm.z = mesh->mNormals[i].z;
        }

        if (g_VertexLayout.HasAttributeTypes(VERTEX_ATTRIBUTE_TYPE_TANGENT))
        {
            assert(mesh->mTangents != nullptr && "There are no tangents to read!");

            Tangent tan{};
            tan.x = mesh->mTangents[i].x;
            tan.y = mesh->mTangents[i].y;
            tan.z = mesh->mTangents[i].z;
        }

        if (g_VertexLayout.HasAttributeTypes(VERTEX_ATTRIBUTE_TYPE_BITANGENT))
        {
            Bitangent tan{};
            tan.x = mesh->mBitangents[i].x;
            tan.y = mesh->mBitangents[i].y;
            tan.z = mesh->mBitangents[i].z;
        }

        if (g_VertexLayout.HasAttributeTypes(VERTEX_ATTRIBUTE_TYPE_COLOR_RGB))
        {
            //assert(mesh->GetNumColorChannels() >= 3 && "Not enough color channels per vertex!");
            //ColorRGB rgb{};
            //rgb.r = mesh->mColors[i][0];
            assert(false && "I don't support color exporting yet.");
        }

        if (g_VertexLayout.HasAttributeTypes(VERTEX_ATTRIBUTE_TYPE_COLOR_RGBA))
        {
            assert(falst && "I don't support color exporting yet.");
        }
        
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

void serializeMeshVertices(Mesh const& mesh, std::vector<std::byte>& storage)
{
    std::uint8_t buffer[sizeof(Vertex)];
    for (std::size_t i = 0; i < mesh.vertices.size(); i++) {
        
        std::memcpy(buffer, &mesh.vertices[i], sizeof(Vertex));

        for (std::size_t j = 0; j < sizeof(Vertex); j++) {
            storage.emplace_back(buffer[j]);
        }
    }
}

void serializeMeshIndicies(Mesh const& mesh, std::vector<std::byte>& storage)
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