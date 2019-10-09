#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

enum VertexAttributeType : std::uint32_t
{
    VERTEX_ATTRIBUTE_TYPE_NONE       = 0,
    VERTEX_ATTRIBUTE_TYPE_POSITION   = 1,
    VERTEX_ATTRIBUTE_TYPE_NORMAL     = 1 << 1,
    VERTEX_ATTRIBUTE_TYPE_TANGENT    = 1 << 2,
    VERTEX_ATTRIBUTE_TYPE_BITANGENT  = 1 << 3,
    VERTEX_ATTRIBUTE_TYPE_UV         = 1 << 4,
    VERTEX_ATTRIBUTE_TYPE_COLOR_RGB  = 1 << 5,
    VERTEX_ATTRIBUTE_TYPE_COLOR_RGBA = 1 << 6,

};


struct VtxAttributeLayout
{
    std::uint32_t m_Size;
    VertexAttributeType m_Type;
};

class VtxLayout
{
public:
    template<typename TAttribute>
    void AddAttribute()
    {
        m_Attributes.emplace_back(VtxAttributeLayout{ static_cast<std::uint32_t>(sizeof(TAttribute)), TAttribute::Type() });
    }

    std::uint32_t GetVertexCount() const { return static_cast<std::uint32_t>(m_Attributes.size()); }

    std::uint32_t GetVertexSize() const 
    { 
        std::uint32_t result = 0;
        for (auto const& attribute : m_Attributes)
            result += attribute.m_Size;

        return result;
    }

    std::vector<std::byte> Serialize() const
    {
        return { 
            reinterpret_cast<std::byte const*>(m_Attributes.data()), 
            reinterpret_cast<std::byte const*>(m_Attributes.data() + m_Attributes.size()) 
        };
    }

    void Deserialize(std::vector<std::byte> const& data)
    {
        std::uint32_t const attributesCount = data.size() / sizeof(VtxAttributeLayout);
        // hmmm
    }


private:
    std::vector<VtxAttributeLayout> m_Attributes;
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
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_POSITION; }
};

struct ColorRGB
{
    float r, g, b;
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_COLOR_RGB; }
};

struct ColorRGBA
{
    float r, g, b, a;
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_COLOR_RGBA; }
};

struct Normal
{
    float x, y, z;
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_NORMAL; }
};

struct Tangent
{
	float x, y, z;
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_TANGENT; }
};

struct Bitangent
{
	float x, y, z;
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_BITANGENT; }
};

struct UV
{
	float u, v;
    static constexpr VertexAttributeType Type() { return VERTEX_ATTRIBUTE_TYPE_UV; }
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
