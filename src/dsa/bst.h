#pragma once
#include "../database/models.h"
#include "dynamic_array.h"
#include <string>
#include <algorithm>

namespace DSA {

// BST Node for restaurant search
struct BSTNode {
    Models::Restaurant data;
    BSTNode* left;
    BSTNode* right;
    
    BSTNode(const Models::Restaurant& restaurant) 
        : data(restaurant), left(nullptr), right(nullptr) {}
};

// Binary Search Tree for alphabetical restaurant search
class RestaurantBST {
private:
    BSTNode* root;
    
    BSTNode* insertHelper(BSTNode* node, const Models::Restaurant& restaurant) {
        if (node == nullptr) {
            return new BSTNode(restaurant);
        }
        
        if (restaurant.name < node->data.name) {
            node->left = insertHelper(node->left, restaurant);
        } else {
            node->right = insertHelper(node->right, restaurant);
        }
        return node;
    }
    
    void searchPrefixHelper(BSTNode* node, const std::string& prefix, 
                           DynamicArray<Models::Restaurant>& results) {
        if (node == nullptr) return;
        
        // Check if current node matches prefix
        if (node->data.name.size() >= prefix.size()) {
            std::string nodePrefix = node->data.name.substr(0, prefix.size());
            std::transform(nodePrefix.begin(), nodePrefix.end(), nodePrefix.begin(), ::tolower);
            std::string lowerPrefix = prefix;
            std::transform(lowerPrefix.begin(), lowerPrefix.end(), lowerPrefix.begin(), ::tolower);
            
            if (nodePrefix == lowerPrefix) {
                results.push_back(node->data);
            }
        }
        
        // Search both subtrees for potential matches (since prefix match isn't strictly ordered logic if we search partials)
        // Wait, for BST strict search we can optimize, but for "contains" or "starts with" usually we traverse relevant subtrees.
        // For simple prefix, we can follow ordering. But let's keep it simple full transversal if logic is complex, 
        // or optimized if strict.
        // The original code traversed both. I'll keep that behavior for robustness.
        
        searchPrefixHelper(node->left, prefix, results);
        searchPrefixHelper(node->right, prefix, results);
    }
    
    void inorderHelper(BSTNode* node, DynamicArray<Models::Restaurant>& results) {
        if (node == nullptr) return;
        inorderHelper(node->left, results);
        results.push_back(node->data);
        inorderHelper(node->right, results);
    }
    
    void destroyTree(BSTNode* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
    
public:
    RestaurantBST() : root(nullptr) {}
    
    ~RestaurantBST() {
        destroyTree(root);
    }
    
    void insert(const Models::Restaurant& restaurant) {
        root = insertHelper(root, restaurant);
    }
    
    DynamicArray<Models::Restaurant> searchByPrefix(const std::string& prefix) {
        DynamicArray<Models::Restaurant> results;
        searchPrefixHelper(root, prefix, results);
        return results;
    }
    
    DynamicArray<Models::Restaurant> getAllSorted() {
        DynamicArray<Models::Restaurant> results;
        inorderHelper(root, results);
        return results;
    }
};

} // namespace DSA
