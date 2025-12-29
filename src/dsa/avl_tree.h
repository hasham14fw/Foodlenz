#pragma once
#include "../database/models.h"
#include "dynamic_array.h"
#include <string>
#include <vector> 
#include <algorithm>

namespace DSA {

// AVL Tree Node
struct AVLNode {
    std::string cuisine;
    DynamicArray<Models::Restaurant> restaurants; // Use DynamicArray
    int height;
    AVLNode* left;
    AVLNode* right;
    
    AVLNode(const std::string& c) 
        : cuisine(c), height(1), left(nullptr), right(nullptr) {}
};

// AVL Tree for balanced cuisine categories
class AVLTree {
private:
    AVLNode* root;
    
    int getHeight(AVLNode* node) {
        return node == nullptr ? 0 : node->height;
    }
    
    int getBalance(AVLNode* node) {
        return node == nullptr ? 0 : getHeight(node->left) - getHeight(node->right);
    }
    
    void updateHeight(AVLNode* node) {
        if (node != nullptr) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }
    
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    AVLNode* insertHelper(AVLNode* node, const std::string& cuisine, const Models::Restaurant& restaurant) {
        if (node == nullptr) {
            AVLNode* newNode = new AVLNode(cuisine);
            newNode->restaurants.push_back(restaurant);
            return newNode;
        }
        
        if (cuisine < node->cuisine) {
            node->left = insertHelper(node->left, cuisine, restaurant);
        } else if (cuisine > node->cuisine) {
            node->right = insertHelper(node->right, cuisine, restaurant);
        } else {
            // Cuisine already exists, add restaurant to list
            node->restaurants.push_back(restaurant);
            return node;
        }
        
        updateHeight(node);
        
        int balance = getBalance(node);
        
        // Left Left Case
        if (balance > 1 && cuisine < node->left->cuisine) {
            return rotateRight(node);
        }
        
        // Right Right Case
        if (balance < -1 && cuisine > node->right->cuisine) {
            return rotateLeft(node);
        }
        
        // Left Right Case
        if (balance > 1 && cuisine > node->left->cuisine) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right Left Case
        if (balance < -1 && cuisine < node->right->cuisine) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    AVLNode* searchHelper(AVLNode* node, const std::string& cuisine) {
        if (node == nullptr || node->cuisine == cuisine) {
            return node;
        }
        
        if (cuisine < node->cuisine) {
            return searchHelper(node->left, cuisine);
        } else {
            return searchHelper(node->right, cuisine);
        }
    }
    
    void getAllCuisinesHelper(AVLNode* node, DynamicArray<std::string>& cuisines) {
        if (node == nullptr) return;
        getAllCuisinesHelper(node->left, cuisines);
        cuisines.push_back(node->cuisine);
        getAllCuisinesHelper(node->right, cuisines);
    }
    
    void destroyTree(AVLNode* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
    
public:
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        destroyTree(root);
    }
    
    void insert(const std::string& cuisine, const Models::Restaurant& restaurant) {
        root = insertHelper(root, cuisine, restaurant);
    }
    
    DynamicArray<Models::Restaurant> getRestaurantsByCuisine(const std::string& cuisine) {
        AVLNode* node = searchHelper(root, cuisine);
        if (node != nullptr) {
            return node->restaurants;
        }
        return DynamicArray<Models::Restaurant>();
    }
    
    DynamicArray<std::string> getAllCuisines() {
        DynamicArray<std::string> cuisines;
        getAllCuisinesHelper(root, cuisines);
        return cuisines;
    }
};

} // namespace DSA
