#pragma once
#include "graph.h"
#include "stack_queue.h"
#include <string>
#include <limits>

namespace DSA {

// Custom Min Priority Queue for Dijkstra
struct PathNode {
    std::string vertex;
    double distance;
    
    PathNode(const std::string& v, double d) : vertex(v), distance(d) {}
    PathNode() : distance(0) {} // Default
    
    bool operator>(const PathNode& other) const {
        return distance > other.distance;
    }
    bool operator<(const PathNode& other) const {
        return distance < other.distance;
    }
};

class MinHeap {
private:
    DynamicArray<PathNode> heap;
    
    void heapifyUp(size_t index) {
        if (index == 0) return;
        size_t parent = (index - 1) / 2;
        if (heap[index].distance < heap[parent].distance) {
            std::swap(heap[index], heap[parent]);
            heapifyUp(parent);
        }
    }
    
    void heapifyDown(size_t index) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;
        
        if (left < heap.size() && heap[left].distance < heap[smallest].distance) {
            smallest = left;
        }
        if (right < heap.size() && heap[right].distance < heap[smallest].distance) {
            smallest = right;
        }
        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }
    
public:
    void push(const PathNode& node) {
        heap.push_back(node);
        heapifyUp(heap.size() - 1);
    }
    
    PathNode pop() {
        if (heap.empty()) return PathNode();
        PathNode min = heap[0];
        heap[0] = heap[heap.size() - 1]; // Use last element
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
        return min;
    }
    
    bool empty() const { return heap.empty(); }
};

// Dijkstra's shortest path algorithm
class Dijkstra {
public:
    struct PathResult {
        DynamicArray<std::string> path;
        double totalDistance;
        bool found;
        
        PathResult() : totalDistance(0), found(false) {}
    };
    
    static PathResult findShortestPath(Graph& graph, const std::string& start, const std::string& end) {
        PathResult result;
        
        if (!graph.hasVertex(start) || !graph.hasVertex(end)) {
            return result;
        }
        
        CustomHashTable<std::string, double> distances;
        CustomHashTable<std::string, std::string> previous;
        MinHeap pq;
        
        // Initialize distances
        DynamicArray<std::string> vertices = graph.getAllVertices();
        for (size_t i = 0; i < vertices.size(); i++) {
            distances.insert(vertices[i], std::numeric_limits<double>::infinity());
        }
        
        distances.insert(start, 0);
        pq.push(PathNode(start, 0));
        
        // Processing
        while (!pq.empty()) {
            PathNode current = pq.pop();
            
            if (current.vertex == end) break;
            
            double* currentDistPtr = distances.get(current.vertex);
            if (!currentDistPtr) continue;
            
            if (current.distance > *currentDistPtr) continue;
            
            DynamicArray<std::string> neighbors = graph.getNeighbors(current.vertex);
            for (size_t i = 0; i < neighbors.size(); i++) {
                std::string neighbor = neighbors[i];
                double edgeWeight = graph.getEdgeWeight(current.vertex, neighbor);
                double newDistance = *currentDistPtr + edgeWeight;
                
                double* neighborDistPtr = distances.get(neighbor);
                if (neighborDistPtr && newDistance < *neighborDistPtr) {
                    // Update distance
                    distances.insert(neighbor, newDistance); // Overwrite logic in HashTable
                    previous.insert(neighbor, current.vertex);
                    pq.push(PathNode(neighbor, newDistance));
                }
            }
        }
        
        // Reconstruct path
        double* endDistPtr = distances.get(end);
        if (endDistPtr && *endDistPtr != std::numeric_limits<double>::infinity()) {
            Stack<std::string> pathStack;
            std::string current = end;
            
            while (current != start) {
                pathStack.push(current);
                std::string* prevPtr = previous.get(current);
                if (!prevPtr) return result; // Should not happen if path found
                current = *prevPtr;
            }
            pathStack.push(start);
            
            while (!pathStack.empty()) {
                result.path.push_back(pathStack.top());
                pathStack.pop();
            }
            
            result.totalDistance = *endDistPtr;
            result.found = true;
        }
        
        return result;
    }
};

} // namespace DSA
