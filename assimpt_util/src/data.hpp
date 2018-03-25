#pragma once

#include <vector>

struct VertHeader
{
    std::uint32_t vertexCount_ = 0;
    std::uint32_t vertexSize_ = 0;
    std::uint32_t indexCount_ = 0;
    std::uint32_t indexSize_ = 0;
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
    Normal normal_;
	Tangent tangent_;
	Bitangent bitangent_;
	UV uv_;
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