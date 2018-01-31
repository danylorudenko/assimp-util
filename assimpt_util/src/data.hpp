#pragma once

#include <vector>

struct VertHeader
{
    std::uint32_t vertexCount_ = 0;
    std::uint32_t vertexSize_ = 0;
    std::uint32_t indexCount_ = 0;
    std::uint32_t indexSize_ = 0;
};

struct Vertex
{
    float position[3];
    float normal[3];
    float uv[2];
};

struct Pos
{
    float x, y, z;
};

struct Normal
{
    float x, y;
};

struct UV
{
    float u, v;
};

struct Mesh
{
    Mesh() = default;
    Mesh(Mesh const&) = default;
    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh const&) = default;
    Mesh& operator=(Mesh&&) = default;


    std::vector<Pos> vertices;
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