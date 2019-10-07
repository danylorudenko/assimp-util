#pragma once

#include <vector>
#include <cstdint>

enum ModelVertexContentFlags : std::uint32_t
{
    MODEL_VERTEX_CONTENT_POSITION = 1,
    MODEL_VERTEX_CONTENT_NORMAL = 1 << 1,
    MODEL_VERTEX_CONTENT_TANGENT = 1 << 2,
    MODEL_VERTEX_CONTENT_BITANGENT = 1 << 3,
    MODEL_VERTEX_CONTENT_UV = 1 << 4
};

struct ModelHeader
{
    std::uint32_t vertexCount_ = 0;
    std::uint32_t vertexSize_ = 0;
    std::uint32_t indexCount_ = 0;
    std::uint32_t indexSize_ = 0;
    std::uint32_t vertexContentFlags_ = 0;
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
    Pos position_;
#ifdef OUTPUT_NORMAL
    Normal normal_;
#endif // OUTPUT_NORMAL
#ifdef OUTPUT_TANGENT
	Tangent tangent_;
#endif // OUTPUT_TANGENT
#ifdef OUTPUT_BITANGENT
	Bitangent bitangent_;
#endif // OUTPUT_BITANGENT
#ifdef OUTPUT_UV
	UV uv_;
#endif // OUTPUT_UV
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

template<typename T>
class Node
{
public:
    Node() = default;
    Node(T&& data) : data_{ std::move(data) } { }
    Node(Node<T> const&) = delete;
    Node(Node<T>&& rhs) = default;
    Node& operator=(Node<T> const&) = delete;
    Node& operator=(Node<T>&& rhs) = default;

    template<typename U>
    Node<T>& AddChild(U&& data)
    {
        children_.emplace_back(std::forward(data));
    }

    Node<T>& Child(std::size_t index)
    {
        return children_[index];
    }
    Node<T>& operator[](std::size_t index)
    {
        return Child(index);
    }
private:
    T data_;
    std::vector<Node<T>> children_;
};