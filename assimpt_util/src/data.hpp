#pragma once

#include <vector>

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
    std::vector<Pos> vertices;
    std::vector<std::size_t> indicies;

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