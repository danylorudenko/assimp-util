#pragma once

#include <vector>
#include <cstdint>

enum VertexAttributeFlags : std::uint32_t
{
    VERTEX_ATTRIBUTE_POSITION   = 1,
    VERTEX_ATTRIBUTE_NORMAL     = 1 << 1,
    VERTEX_ATTRIBUTE_TANGENT    = 1 << 2,
    VERTEX_ATTRIBUTE_BITANGENT  = 1 << 3,
    VERTEX_ATTRIBUTE_UV         = 1 << 4
};

template<typename TAttribute>
struct VtxAttributeLayout
{
    static constexpr std::uint32_t AttributeSize() { return static_cast<std::uint32_t>(sizeof(VtxAttributeLayout)); }
};

class VtxLayout
{
public:

};

struct ModelHeader
{
    std::uint32_t m_VertexCount;
    std::uint32_t m_VertexSize;
    std::uint32_t m_IndexCount;
    std::uint32_t m_IndexSize;
    std::uint32_t m_VertexAttributeFlags;
};

struct Pos
{
    float x, y, z;
};

struct Normal
{
    float x, y, z;
};

struct Tangent
{
	float x, y, z;
};

struct Bitangent
{
	float x, y, z;
};

struct UV
{
	float u, v;
};

struct Vertex
{
    
};

struct Mesh
{
    Mesh() = default;
    Mesh(Mesh const&) = default;
    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh const&) = default;
    Mesh& operator=(Mesh&&) = default;


    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indicies;

    //int material;
};
