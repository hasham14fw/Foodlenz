#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>
#include <algorithm>
#include <ctime>
#include "../dsa/dynamic_array.h"
#include "../dsa/linked_list.h"
#include "../dsa/stack_queue.h"
#include "../dsa/hash_table.h"
#include "../dsa/max_heap.h"
#include "../dsa/bst.h"
#include "../dsa/avl_tree.h"
#include "../dsa/graph.h"
#include "idatabase.h"

namespace Database {

class CSVDatabase : public IDatabase {
private:
    // Core Data Structures
    DSA::CustomHashTable<std::string, Models::User> usersById;
    DSA::CustomHashTable<std::string, Models::User> usersByEmail;
    
    DSA::DynamicArray<Models::Restaurant> allRestaurants;
    DSA::CustomHashTable<std::string, Models::Restaurant> restaurantsById;
    
    DSA::CustomHashTable<std::string, DSA::DynamicArray<Models::MenuItem>> menuItemsByRestaurantId;
    DSA::CustomHashTable<std::string, DSA::DynamicArray<Models::Review>> reviewsByRestaurantId;
    DSA::CustomHashTable<std::string, DSA::DynamicArray<Models::Review>> reviewsByUserId;
    
    // DSA for Features
    DSA::MaxHeap trendingHeap;
    DSA::RestaurantBST searchBST;
    DSA::AVLTree cuisineTree;
    DSA::Graph cityGraph;
    
    // Paths
    const std::string USERS_FILE = "data/users.csv";
    const std::string RESTAURANTS_FILE = "data/restaurants.csv";
    const std::string MENU_FILE = "data/menu_items.csv";
    const std::string REVIEWS_FILE = "data/reviews.csv";
    const std::string LOCATIONS_FILE = "data/locations.csv";

    // Cloud URL
    const std::string CLOUD_BASE = "https://raw.githubusercontent.com/hasham14fw/csv_files_DS/refs/heads/main/";
    
    // GitHub API
    std::string githubToken;
    const std::string REPO_OWNER = "hasham14fw";
    const std::string REPO_NAME = "csv_files_DS";
    const std::string BRANCH = "main";

    // Helper: Split CSV line
    DSA::DynamicArray<std::string> split(const std::string& line, char delimiter);
    std::string generateId();
    std::string fetchURL(const std::string& url);
    
    // Cloud Write Helpers
    void saveToCloud(const std::string& filename, const std::string& content);
    std::string getFileSHA(const std::string& filename);
    std::string base64_encode(const std::string& in);

    // Loaders
    void loadData();
    void loadUsers();
    void loadRestaurants();
    void loadMenuItems();
    void loadReviews();
    void loadLocations();

public:
    CSVDatabase();

    // User Operations
    Models::User createUser(const std::string& name, const std::string& email, 
                           const std::string& passwordHash, const std::string& city) override;
    Models::User* getUserByEmail(const std::string& email) override;
    Models::User* getUserById(const std::string& id) override;
    
    // Restaurant Operations
    DSA::DynamicArray<Models::Restaurant> getAllRestaurants() override;
    DSA::DynamicArray<Models::Restaurant> getTrendingRestaurants(int limit = 6) override;
    Models::Restaurant* getRestaurantById(const std::string& id) override;
    DSA::DynamicArray<Models::Restaurant> searchRestaurants(const std::string& query,
                                                      const std::string& cuisine = "",
                                                      const std::string& city = "",
                                                      double minRating = 0.0) override;
    
    // Menu Operations
    DSA::DynamicArray<Models::MenuItem> getMenuItems(const std::string& restaurantId) override;
    
    // Review Operations
    Models::Review createReview(const std::string& userId, const std::string& restaurantId,
                                int tasteRating, int ambianceRating, int overallRating,
                                const std::string& comment) override;
    bool hasUserReviewed(const std::string& userId, const std::string& restaurantId) override;
    DSA::DynamicArray<Models::Review> getReviews(const std::string& restaurantId) override;
    DSA::DynamicArray<Models::Review> getUserReviews(const std::string& userId) override;
    
    // Graph Operations
    DSA::Graph& getCityGraph() override;
};

} // namespace Database
