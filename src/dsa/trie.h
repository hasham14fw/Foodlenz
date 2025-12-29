#pragma once
#include <string>
#include <vector>
#include <iostream>

namespace dsa {

template <typename T>
class TrieNode {
public:
    // Simple child storage using vector for minimalism
    std::vector<std::pair<char, TrieNode*>> children;
    bool isEndOfWord;
    T value;

    TrieNode() : isEndOfWord(false) {}
    
    ~TrieNode() {
        for(auto& pair : children) {
            delete pair.second;
        }
    }
};

template <typename T>
class Trie {
private:
    TrieNode<T>* root;

    char toLower(char c) const {
        if (c >= 'A' && c <= 'Z') return c + 32;
        return c;
    }
    
    TrieNode<T>* findChild(TrieNode<T>* node, char c) const {
        for(auto& pair : node->children) {
            if (pair.first == c) return pair.second;
        }
        return nullptr;
    }

public:
    Trie() : root(new TrieNode<T>()) {}
    
    ~Trie() {
        delete root;
    }

    void insert(const std::string& key, const T& value) {
        TrieNode<T>* current = root;
        
        for (char c : key) {
            char lowerC = toLower(c);
            TrieNode<T>* child = findChild(current, lowerC);
            if (!child) {
                child = new TrieNode<T>();
                current->children.push_back({lowerC, child});
            }
            current = child;
        }
        current->isEndOfWord = true;
        current->value = value;
    }

    bool search(const std::string& key, T* outValue = nullptr) const {
        TrieNode<T>* current = root;
        
        for (char c : key) {
            char lowerC = toLower(c);
            TrieNode<T>* child = findChild(current, lowerC);
            if (!child) return false;
            current = child;
        }
        
        if (current->isEndOfWord && outValue) {
            *outValue = current->value;
        }
        return current->isEndOfWord;
    }

    std::vector<std::pair<std::string, T>> searchInText(const std::string& text) const {
        std::vector<std::pair<std::string, T>> results;
        
        for (size_t i = 0; i < text.length(); ++i) {
            TrieNode<T>* current = root;
            size_t j = i;
            
            size_t lastMatchEnd = (size_t)-1;
            T lastMatchValue;
            bool foundAny = false;

            while (j < text.length()) {
                char lowerC = toLower(text[j]);
                TrieNode<T>* child = findChild(current, lowerC);
                
                if (!child) break;
                
                current = child;
                if (current->isEndOfWord) {
                    lastMatchEnd = j;
                    lastMatchValue = current->value;
                    foundAny = true;
                }
                j++;
            }

            if (foundAny) {
                std::string matchedStr = text.substr(i, lastMatchEnd - i + 1);
                
                // Boundary check without cctype
                bool startOk = (i == 0) || (text[i-1] == ' ');
                bool endOk = (lastMatchEnd == text.length() - 1) || (text[lastMatchEnd+1] == ' ');

                if (startOk && endOk) {
                    results.push_back({matchedStr, lastMatchValue});
                    i = lastMatchEnd; 
                }
            }
        }
        return results;
    }
};

}
