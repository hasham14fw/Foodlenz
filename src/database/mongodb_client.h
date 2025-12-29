#pragma once
#include "models.h"
#include "../dsa/max_heap.h"
#include "../dsa/bst.h"
#include "../dsa/avl_tree.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "crow_all.h" // For JSON parsing

namespace Database {

class MongoDBClient {
private:
    std::string apiKey;
    std::string appId;
    std::string clusterName;
    std::string databaseName;
    std::string baseUrl;

    // Helper to perform HTTP request via curl CLI
    std::string performRequest(const std::string& action, const crow::json::wvalue& payload);
    
    // Helper to parse JSON response
    crow::json::rvalue parseResponse(const std::string& response);

    std::string generateId() {
        std::stringstream ss;
        ss << std::time(nullptr) << "_" << std::rand();
        return ss.str();
    }

public:
    MongoDBClient(const std::string& key, const std::string& app, const std::string& cluster, const std::string& db)
        : apiKey(key), appId(app), clusterName(cluster), databaseName(db) {
        // Default to local proxy
        baseUrl = "http://localhost:8080";
    }
    
    void setBaseUrl(const std::string& url) {
        baseUrl = url;
    }

    // User operations
    Models::User createUser(const std::string& name, const std::string& email, 
                           const std::string& passwordHash, const std::string& city);
    
    Models::User* getUserByEmail(const std::string& email);
    
    Models::User* getUserById(const std::string& id);
    
    // Restaurant operations
    std::vector<Models::Restaurant> getAllRestaurants();
    
    std::vector<Models::Restaurant> getTrendingRestaurants(int limit = 6);
    
    Models::Restaurant* getRestaurantById(const std::string& id);
    
    std::vector<Models::Restaurant> searchRestaurants(const std::string& query,
                                                      const std::string& cuisine = "",
                                                      const std::string& city = "",
                                                      double minRating = 0.0);
    
    // Menu operations
    std::vector<Models::MenuItem> getMenuItems(const std::string& restaurantId);
    
    // Review operations
    Models::Review createReview(const std::string& userId, const std::string& restaurantId,
                               int tasteRating, int ambianceRating, int overallRating,
                               const std::string& comment);
    
    // Check if user has already reviewed a restaurant
    bool hasUserReviewed(const std::string& userId, const std::string& restaurantId);
    
    std::vector<Models::Review> getReviews(const std::string& restaurantId);
    
    std::vector<Models::Review> getUserReviews(const std::string& userId);

    // Helper to seed data if empty
    void seedSampleData();
};

} // namespace Database
