#pragma once
#include "dynamic_array.h"
#include <string>
#include <algorithm>
#include <iostream>

namespace DSA {

// ==================== SINGLY LINKED LIST ====================
template<typename T>
struct Node {
    T data;
    Node* next;
    Node(const T& data) : data(data), next(nullptr) {}
};

template<typename T>
class LinkedList {
private:
    Node<T>* head;
    int count;
    
public:
    LinkedList() : head(nullptr), count(0) {}
    
    // Rule of 3/5 could be implemented, but skipping for brevity in this context
    // Ideally should implement deep copy and destructor.
    
    ~LinkedList() {
        clear();
    }
    
    void append(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        if (head == nullptr) {
            head = newNode;
        } else {
            Node<T>* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        count++;
    }
    
    void prepend(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        newNode->next = head;
        head = newNode;
        count++;
    }
    
    DynamicArray<T> toDynamicArray() {
        DynamicArray<T> result;
        Node<T>* current = head;
        while (current != nullptr) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }
    
    void clear() {
        while (head != nullptr) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
        count = 0;
    }
    
    int size() const { return count; }
    bool isEmpty() const { return head == nullptr; }
};

// Specialized linked list for restaurant name suggestions
class SearchSuggestions {
private:
    LinkedList<std::string> suggestions;
    
public:
    void addSuggestion(const std::string& name) {
        suggestions.append(name);
    }
    
    DynamicArray<std::string> filter(const std::string& query) {
        DynamicArray<std::string> allSuggestions = suggestions.toDynamicArray();
        DynamicArray<std::string> filtered;
        
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        for (size_t i = 0; i < allSuggestions.size(); i++) {
            std::string suggestion = allSuggestions[i];
            std::string lowerSuggestion = suggestion;
            std::transform(lowerSuggestion.begin(), lowerSuggestion.end(), lowerSuggestion.begin(), ::tolower);
            
            if (lowerSuggestion.find(lowerQuery) != std::string::npos) {
                filtered.push_back(suggestion);
                if (filtered.size() >= 10) break; 
            }
        }
        return filtered;
    }
    
    void clear() {
        suggestions.clear();
    }
};

// ==================== DOUBLY LINKED LIST ====================
template<typename T>
struct DNode {
    T data;
    DNode* next;
    DNode* prev;
    DNode(const T& data) : data(data), next(nullptr), prev(nullptr) {}
};

template<typename T>
class DoublyLinkedList {
private:
    DNode<T>* head;
    DNode<T>* tail;
    int count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~DoublyLinkedList() {
        clear();
    }

    void append(const T& data) {
        DNode<T>* newNode = new DNode<T>(data);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }
    
    // Iterate forward
    DynamicArray<T> traverseForward() {
        DynamicArray<T> result;
        DNode<T>* current = head;
        while (current) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }
    
    // Iterate backward
    DynamicArray<T> traverseBackward() {
        DynamicArray<T> result;
        DNode<T>* current = tail;
        while (current) {
            result.push_back(current->data);
            current = current->prev;
        }
        return result;
    }

    void clear() {
        DNode<T>* current = head;
        while (current) {
            DNode<T>* temp = current;
            current = current->next;
            delete temp;
        }
        head = tail = nullptr;
        count = 0;
    }
    
    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

// ==================== CIRCULAR LINKED LIST ====================
// Useful for auto-sliding carousels
template<typename T>
class CircularLinkedList {
private:
    Node<T>* head;
    int count;

public:
    CircularLinkedList() : head(nullptr), count(0) {}
    
    ~CircularLinkedList() {
        clear();
    }

    void append(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        if (!head) {
            head = newNode;
            newNode->next = head; // Point to self
        } else {
            Node<T>* temp = head;
            while (temp->next != head) {
                temp = temp->next;
            }
            temp->next = newNode;
            newNode->next = head;
        }
        count++;
    }
    
    // Get next item in cycle
    T getNext(T currentData) {
        if (!head) return T();
        Node<T>* temp = head;
        do {
            if (temp->data == currentData) {
                return temp->next->data;
            }
            temp = temp->next;
        } while (temp != head);
        return head->data; // Default to head if not found
    }
    
    DynamicArray<T> toLinearArray() {
        DynamicArray<T> result;
        if (!head) return result;
        
        Node<T>* temp = head;
        do {
            result.push_back(temp->data);
            temp = temp->next;
        } while (temp != head);
        return result;
    }

    void clear() {
        if (!head) return;
        Node<T>* current = head;
        do {
            Node<T>* temp = current;
            current = current->next;
            if (current == head) {
                delete temp;
                break;
            }
            delete temp;
        } while (true);
        head = nullptr;
        count = 0;
    }
};

} // namespace DSA
