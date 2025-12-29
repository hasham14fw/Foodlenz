#pragma once
#include <string>
#include "../dsa/dynamic_array.h"
#include "../dsa/graph.h"
#include "models.h"

namespace Database {

class IDatabase {
public:
    virtual ~IDatabase() = default;

    // User Operations
    virtual Models::User createUser(const std::string& name, const std::string& email, 
                                   const std::string& passwordHash, const std::string& city) = 0;
    virtual Models::User* getUserByEmail(const std::string& email) = 0;
    virtual Models::User* getUserById(const std::string& id) = 0;

    // Restaurant Operations
    virtual DSA::DynamicArray<Models::Restaurant> getAllRestaurants() = 0;
    virtual DSA::DynamicArray<Models::Restaurant> getTrendingRestaurants(int limit = 6) = 0;
    virtual Models::Restaurant* getRestaurantById(const std::string& id) = 0;
    virtual DSA::DynamicArray<Models::Restaurant> searchRestaurants(const std::string& query,
                                                      const std::string& cuisine = "",
                                                      const std::string& city = "",
                                                      double minRating = 0.0) = 0;

    // Menu Operations
    virtual DSA::DynamicArray<Models::MenuItem> getMenuItems(const std::string& restaurantId) = 0;

    // Review Operations
    virtual Models::Review createReview(const std::string& userId, const std::string& restaurantId,
                                int tasteRating, int ambianceRating, int overallRating,
                                const std::string& comment) = 0;
    virtual bool hasUserReviewed(const std::string& userId, const std::string& restaurantId) = 0;
    virtual DSA::DynamicArray<Models::Review> getReviews(const std::string& restaurantId) = 0;
    virtual DSA::DynamicArray<Models::Review> getUserReviews(const std::string& userId) = 0;

    // Graph Operations
    virtual DSA::Graph& getCityGraph() = 0;
};

} // namespace Database
