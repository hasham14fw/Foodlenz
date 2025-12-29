#pragma once
#include "dynamic_array.h"
#include "linked_list.h"
#include <stdexcept>

namespace DSA {

// ==================== STACK (LIFO) ====================
template<typename T>
class Stack {
private:
    DynamicArray<T> data; // DynamicArray is efficient for Stack (push_back/pop_back is O(1) amortized)

public:
    void push(const T& value) {
        data.push_back(value);
    }

    void pop() {
        if (empty()) throw std::underflow_error("Stack is empty");
        data.pop_back();
    }

    T top() const {
        if (empty()) throw std::underflow_error("Stack is empty");
        return data[data.size() - 1];
    }

    bool empty() const {
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }
};

// ==================== QUEUE (FIFO) ====================
template<typename T>
class Queue {
private:
    // LinkedList is efficient for Queue (enqueue at tail, dequeue at head if optimized)
    // Our SinglyLinkedList append is O(N) because it iterates to tail.
    // For a proper O(1) Queue, we should use a list with tail pointer or a tailored implementation.
    // Let's implement a simple Queue with a dedicated internal structure or improve LinkedList.
    // For this assignment, let's wrap our SinglyLinkedList, acknowledging append is O(N).
    // BETTER: Use DynamicArray as a circular buffer or just logical queue.
    // OR: Re-implement a Node-based queue here for O(1).
    
    struct QNode {
        T data;
        QNode* next;
        QNode(const T& d) : data(d), next(nullptr) {}
    };
    
    QNode* frontNode;
    QNode* rearNode;
    size_t count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}
    
    ~Queue() {
        while (!empty()) dequeue();
    }

    void enqueue(const T& value) {
        QNode* newNode = new QNode(value);
        if (rearNode == nullptr) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    void dequeue() {
        if (empty()) throw std::underflow_error("Queue is empty");
        QNode* temp = frontNode;
        frontNode = frontNode->next;
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        count--;
    }

    T front() const {
        if (empty()) throw std::underflow_error("Queue is empty");
        return frontNode->data;
    }

    bool empty() const {
        return frontNode == nullptr;
    }

    size_t size() const {
        return count;
    }
};

// ==================== DEQUE (Double Ended Queue) ====================
template<typename T>
class Deque {
private:
    // Doubly Linked List is ideal for Deque
    DoublyLinkedList<T> list;
    
    // We need to extend DoublyLinkedList or wrap it.
    // Our current DoublyLinkedList only has `append`.
    // Let's implement Deque using DNode structure directly for full control and O(1).
    
    struct DNode {
        T data;
        DNode* next;
        DNode* prev;
        DNode(const T& d) : data(d), next(nullptr), prev(nullptr) {}
    };
    
    DNode* head;
    DNode* tail;
    size_t count;

public:
    Deque() : head(nullptr), tail(nullptr), count(0) {}
    
    ~Deque() {
        while (!empty()) pop_front();
    }

    void push_front(const T& value) {
        DNode* newNode = new DNode(value);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        count++;
    }

    void push_back(const T& value) {
        DNode* newNode = new DNode(value);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }

    void pop_front() {
        if (empty()) throw std::underflow_error("Deque is empty");
        DNode* temp = head;
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;
        delete temp;
        count--;
    }

    void pop_back() {
        if (empty()) throw std::underflow_error("Deque is empty");
        DNode* temp = tail;
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else head = nullptr;
        delete temp;
        count--;
    }

    T front() const {
        if (empty()) throw std::underflow_error("Deque is empty");
        return head->data;
    }

    T back() const {
        if (empty()) throw std::underflow_error("Deque is empty");
        return tail->data;
    }

    bool empty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }
};

} // namespace DSA
