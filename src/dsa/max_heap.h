#pragma once
#include "../database/models.h"
#include "dynamic_array.h"
#include <algorithm>

namespace DSA {

// Max Heap for trending restaurants (sorted by rating)
class MaxHeap {
private:
    DynamicArray<Models::Restaurant> heap;
    
    void heapifyUp(size_t index) {
        if (index == 0) return;
        size_t parent = (index - 1) / 2;
        if (heap[index].rating > heap[parent].rating) {
            std::swap(heap[index], heap[parent]);
            heapifyUp(parent);
        }
    }
    
    void heapifyDown(size_t index) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t largest = index;
        
        if (left < heap.size() && heap[left].rating > heap[largest].rating) {
            largest = left;
        }
        if (right < heap.size() && heap[right].rating > heap[largest].rating) {
            largest = right;
        }
        if (largest != index) {
            std::swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }
    
public:
    void insert(const Models::Restaurant& restaurant) {
        heap.push_back(restaurant);
        heapifyUp(heap.size() - 1);
    }
    
    Models::Restaurant extractMax() {
        if (heap.empty()) {
            return Models::Restaurant();
        }
        Models::Restaurant max = heap[0];
        heap[0] = heap[heap.size() - 1]; // Move last to root
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return max;
    }
    
    DynamicArray<Models::Restaurant> getTopN(int n) {
        DynamicArray<Models::Restaurant> result;
        // Create a copy to preserve original heap
        MaxHeap tempHeap = *this;
        for (int i = 0; i < n && !tempHeap.heap.empty(); i++) {
            result.push_back(tempHeap.extractMax());
        }
        return result;
    }
    
    bool isEmpty() const {
        return heap.empty();
    }
    
    size_t size() const {
        return heap.size();
    }
};

} // namespace DSA
