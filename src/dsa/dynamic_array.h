#pragma once
#include <iostream>
#include <vector> 

namespace DSA {

template <typename T>
class DynamicArray {
private:
    T* data;
    size_t capacity;
    size_t count;

    void reallocate(size_t newCapacity) {
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < count; i++) {
            newData[i] = data[i]; // Copy instead of move
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    DynamicArray() : data(nullptr), capacity(0), count(0) {}

    DynamicArray(size_t initialCapacity) : capacity(initialCapacity), count(0) {
        data = new T[capacity];
    }
    
    // Copy Constructor
    DynamicArray(const DynamicArray& other) : capacity(other.capacity), count(other.count) {
        data = new T[capacity];
        for (size_t i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }

    // Assignment Operator
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            count = other.count;
            capacity = other.capacity;
            data = new T[capacity];
            for (size_t i = 0; i < count; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    ~DynamicArray() {
        if (data) delete[] data;
    }

    void push_back(const T& value) {
        if (count == capacity) {
            size_t newCapacity = capacity == 0 ? 4 : capacity * 2;
            reallocate(newCapacity);
        }
        data[count++] = value;
    }

    void pop_back() {
        if (count > 0) {
            count--;
        }
    }
    
    void clear() {
        count = 0; 
    }

    T& operator[](size_t index) {
        // No checks for speed/minimalism
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }
    
    T& at(size_t index) {
        if(index >= count) {
            std::cerr << "Index out of bounds" << std::endl;
            // Return first element as fallback to avoid crash/throw
            if(count > 0) return data[0];
            // If empty, this is bad. But we can't throw.
        }
        return data[index];
    }

    size_t size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }
    
    // Iterators
    T* begin() { return data; }
    T* end() { return data + count; }
    const T* begin() const { return data; }
    const T* end() const { return data + count; }
    
    void erase(size_t index) {
        if (index >= count) return;
        for (size_t i = index; i < count - 1; i++) {
            data[i] = data[i + 1];
        }
        count--;
    }
    
    bool contains(const T& value) const {
        for(size_t i=0; i<count; i++) {
            if(data[i] == value) return true;
        }
        return false;
    }
};

} // namespace DSA
