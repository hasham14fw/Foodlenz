#pragma once
#include "../dsa/max_heap.h"
#include "../database/idatabase.h"
#include "../dsa/dynamic_array.h"
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>

namespace Algorithms {

class RecommendationEngine {
private:
    Database::IDatabase* db;
    
    struct PreferenceAnalysis {
      
        // We can use std::map for these counters as they are temporary local vars
        std::map<std::string, int> cuisineFrequency;
        std::map<std::string, double> categoryRatings;
        double avgBudget;
        int totalReviews;
        
        PreferenceAnalysis() : avgBudget(0), totalReviews(0) {}
    };
    
    PreferenceAnalysis analyzeUserPreferences(const std::string& userId) {
        PreferenceAnalysis analysis;
        
        DSA::DynamicArray<Models::Review> userReviews = db->getUserReviews(userId);
        analysis.totalReviews = userReviews.size();
        
        if (userReviews.empty()) {
            return analysis;
        }
        
        for (const auto& review : userReviews) {
            auto* restaurant = db->getRestaurantById(review.restaurant_id);
            if (restaurant) {
                // Count cuisine preferences
                analysis.cuisineFrequency[restaurant->cuisine]++;
                
                // Track category ratings
                analysis.categoryRatings[restaurant->category] += review.overallRating;
            }
        }
        
        // Calculate average ratings per category
        for (auto& pair : analysis.categoryRatings) {
            pair.second /= analysis.totalReviews;
        }
        
        return analysis;
    }
    
    double calculateRecommendationScore(const Models::Restaurant& restaurant,
                                       const PreferenceAnalysis& preferences) {
        double score = 0.0;
        
        // Cuisine preference weight (40%)
        if (preferences.cuisineFrequency.find(restaurant.cuisine) != preferences.cuisineFrequency.end()) {
            score += 0.4 * (preferences.cuisineFrequency.at(restaurant.cuisine) / 
                           static_cast<double>(preferences.totalReviews));
        }
        
        // Rating weight (30%)
        score += 0.3 * (restaurant.rating / 5.0);
        
        // Category preference weight (20%)
        if (preferences.categoryRatings.find(restaurant.category) != preferences.categoryRatings.end()) {
            score += 0.2 * (preferences.categoryRatings.at(restaurant.category) / 5.0);
        }
        
        // Diversity bonus (10%)
        if (preferences.cuisineFrequency.find(restaurant.cuisine) == preferences.cuisineFrequency.end()) {
            score += 0.1 * (restaurant.rating / 5.0);
        }
        
        return score;
    }
    
public:
    RecommendationEngine(Database::IDatabase* database) : db(database) {}
    
    DSA::DynamicArray<Models::Restaurant> getRecommendations(const std::string& userId, int limit = 10) {
        auto preferences = analyzeUserPreferences(userId);
        auto allRestaurants = db->getAllRestaurants();
        
        // Score all restaurants
        // Using DynamicArray of Pairs? simple struct
        struct ScoredRest {
            Models::Restaurant r;
            double s;
            bool operator>(const ScoredRest& other) const { return s > other.s; }
            bool operator<(const ScoredRest& other) const { return s < other.s; }
        };
        
        DSA::DynamicArray<ScoredRest> scoredRestaurants;
        
        for (const auto& restaurant : allRestaurants) {
            double score = calculateRecommendationScore(restaurant, preferences);
            scoredRestaurants.push_back({restaurant, score});
        }
        
        // Sort by score descending (simple bubble sort or std::sort)
        // DynamicArray iterators work with std::sort
        std::sort(scoredRestaurants.begin(), scoredRestaurants.end(),
                 [](const ScoredRest& a, const ScoredRest& b) {
                     return a.s > b.s;
                 });
        
        // Extract top N restaurants
        DSA::DynamicArray<Models::Restaurant> recommendations;
        int count = 0;
        for (const auto& scored : scoredRestaurants) {
            if (count >= limit) break;
            recommendations.push_back(scored.r);
            count++;
        }
        
        return recommendations;
    }
};

} // namespace Algorithms
