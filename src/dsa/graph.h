#pragma once
#include "hash_table.h"
#include "dynamic_array.h"
#include <string>
#include <limits>

namespace DSA {

// Graph representation using custom Adjacency List (HashTable + DynamicArray)
class Graph {
public:
    struct Edge {
        std::string destination;
        double weight; // distance in km
        
        Edge(const std::string& dest, double w) : destination(dest), weight(w) {}
        Edge() : weight(0) {} // Default constructor for DynamicArray
    };

private:
    CustomHashTable<std::string, DynamicArray<Edge>> adjList;
    DynamicArray<std::string> vertices;
    
public:
    void addVertex(const std::string& vertex) {
        if (!adjList.contains(vertex)) {
            adjList.insert(vertex, DynamicArray<Edge>());
            vertices.push_back(vertex);
        }
    }
    
    void addEdge(const std::string& from, const std::string& to, double weight) {
        addVertex(from);
        addVertex(to);
        
        DynamicArray<Edge>* edgesFrom = adjList.get(from);
        if (edgesFrom) edgesFrom->push_back(Edge(to, weight));
        
        DynamicArray<Edge>* edgesTo = adjList.get(to);
        if (edgesTo) edgesTo->push_back(Edge(from, weight)); // Undirected graph
    }
    
    DynamicArray<std::string> getNeighbors(const std::string& vertex) {
        DynamicArray<std::string> neighbors;
        DynamicArray<Edge>* edges = adjList.get(vertex);
        if (edges) {
            for (size_t i = 0; i < edges->size(); i++) {
                neighbors.push_back((*edges)[i].destination);
            }
        }
        return neighbors;
    }
    
    double getEdgeWeight(const std::string& from, const std::string& to) {
        DynamicArray<Edge>* edges = adjList.get(from);
        if (edges) {
            for (size_t i = 0; i < edges->size(); i++) {
                if ((*edges)[i].destination == to) {
                    return (*edges)[i].weight;
                }
            }
        }
        return std::numeric_limits<double>::infinity();
    }
    
    DynamicArray<std::string> getAllVertices() {
        return vertices;
    }
    
    bool hasVertex(const std::string& vertex) {
        return adjList.contains(vertex);
    }
};

} // namespace DSA
