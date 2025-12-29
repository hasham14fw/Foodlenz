#include "mongodb_client.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

namespace Database {

// Helper to execute shell command and get output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string MongoDBClient::performRequest(const std::string& action, const crow::json::wvalue& payload) {
    std::string url = baseUrl + "/action/" + action;
    std::string jsonStr = payload.dump();
    
    // Escape quotes for command line (basic escaping)
    // For Windows cmd, we need verify correct escaping.
    // Ideally, write to a temporary file and use -d @file, but simple string might work for now
    // if we escape inner quotes.
    // JSON uses double quotes. We can wrap the whole JSON in single quotes for some shells,
    // or escape double quotes.
    // Windows cmd is tricky.
    // Safer approach: Write to temp file.
    
    std::string tempFile = "temp_payload.json";
    FILE* fp = fopen(tempFile.c_str(), "w");
    if (fp) {
        fputs(jsonStr.c_str(), fp);
        fclose(fp);
    } else {
        return "{}";
    }

    std::string cmd = "curl.exe -s -X POST -H \"Content-Type: application/json\" -d @" + tempFile + " " + url;
    
    try {
        std::string response = exec(cmd.c_str());
        // Clean up
        remove(tempFile.c_str());
        return response;
    } catch (...) {
        remove(tempFile.c_str());
        return "{}";
    }
}

crow::json::rvalue MongoDBClient::parseResponse(const std::string& response) {
    auto json = crow::json::load(response);
    if (!json) {
        std::cerr << "Failed to parse JSON response: " << response << std::endl;
        return crow::json::rvalue();
    }
    return json;
}

Models::User MongoDBClient::createUser(const std::string& name, const std::string& email, 
                       const std::string& passwordHash, const std::string& city) {
    std::string id = generateId();
    
    crow::json::wvalue document;
    document["id"] = id;
    document["name"] = name;
    document["email"] = email;
    document["passwordHash"] = passwordHash;
    document["city"] = city;
    
    crow::json::wvalue payload;
    payload["collection"] = "users";
    payload["document"] = std::move(document);
    
    performRequest("insertOne", payload);
    return Models::User(id, name, email, passwordHash, city);
}

Models::User* MongoDBClient::getUserByEmail(const std::string& email) {
    crow::json::wvalue filter;
    filter["email"] = email;
    
    crow::json::wvalue payload;
    payload["collection"] = "users";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("findOne", payload);
    auto json = parseResponse(response);
    
    if (json && json.has("document") && json["document"].t() != crow::json::type::Null) {
        const auto& doc = json["document"];
        return new Models::User(
            doc["id"].s(),
            doc["name"].s(),
            doc["email"].s(),
            doc["passwordHash"].s(),
            doc["city"].s()
        );
    }
    return nullptr;
}

Models::User* MongoDBClient::getUserById(const std::string& id) {
    crow::json::wvalue filter;
    filter["id"] = id;
    
    crow::json::wvalue payload;
    payload["collection"] = "users";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("findOne", payload);
    auto json = parseResponse(response);
    
    if (json && json.has("document") && json["document"].t() != crow::json::type::Null) {
        const auto& doc = json["document"];
        return new Models::User(
            doc["id"].s(),
            doc["name"].s(),
            doc["email"].s(),
            doc["passwordHash"].s(),
            doc["city"].s()
        );
    }
    return nullptr;
}

std::vector<Models::Restaurant> MongoDBClient::getAllRestaurants() {
    crow::json::wvalue payload;
    payload["collection"] = "restaurants";
    
    std::string response = performRequest("find", payload);
    auto json = parseResponse(response);
    
    std::vector<Models::Restaurant> results;
    if (json && json.has("documents")) {
        for (const auto& doc : json["documents"]) {
            results.emplace_back(
                doc["id"].s(),
                doc["name"].s(),
                doc["city"].s(),
                doc["category"].s(),
                doc["cuisine"].s(),
                doc["rating"].d(),
                doc["logoURL"].s(),
                doc["latitude"].d(),
                doc["longitude"].d()
            );
        }
    }
    return results;
}

std::vector<Models::Restaurant> MongoDBClient::getTrendingRestaurants(int limit) {
    crow::json::wvalue sort;
    sort["rating"] = -1;
    
    crow::json::wvalue payload;
    payload["collection"] = "restaurants";
    payload["sort"] = std::move(sort);
    payload["limit"] = limit;
    
    std::string response = performRequest("find", payload);
    auto json = parseResponse(response);
    
    std::vector<Models::Restaurant> results;
    if (json && json.has("documents")) {
        for (const auto& doc : json["documents"]) {
            results.emplace_back(
                doc["id"].s(),
                doc["name"].s(),
                doc["city"].s(),
                doc["category"].s(),
                doc["cuisine"].s(),
                doc["rating"].d(),
                doc["logoURL"].s(),
                doc["latitude"].d(),
                doc["longitude"].d()
            );
        }
    }
    return results;
}

Models::Restaurant* MongoDBClient::getRestaurantById(const std::string& id) {
    crow::json::wvalue filter;
    filter["id"] = id;
    
    crow::json::wvalue payload;
    payload["collection"] = "restaurants";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("findOne", payload);
    auto json = parseResponse(response);
    
    if (json && json.has("document") && json["document"].t() != crow::json::type::Null) {
        const auto& doc = json["document"];
        return new Models::Restaurant(
            doc["id"].s(),
            doc["name"].s(),
            doc["city"].s(),
            doc["category"].s(),
            doc["cuisine"].s(),
            doc["rating"].d(),
            doc["logoURL"].s(),
            doc["latitude"].d(),
            doc["longitude"].d()
        );
    }
    return nullptr;
}

std::vector<Models::Restaurant> MongoDBClient::searchRestaurants(const std::string& query,
                                                  const std::string& cuisine,
                                                  const std::string& city,
                                                  double minRating) {
    crow::json::wvalue filter;
    
    if (!query.empty()) {
        crow::json::wvalue regex;
        regex["$regex"] = query;
        regex["$options"] = "i";
        filter["name"] = std::move(regex);
    }
    
    if (!cuisine.empty()) {
        filter["cuisine"] = cuisine;
    }
    
    if (!city.empty()) {
        filter["city"] = city;
    }
    
    if (minRating > 0) {
        crow::json::wvalue ratingFilter;
        ratingFilter["$gte"] = minRating;
        filter["rating"] = std::move(ratingFilter);
    }
    
    crow::json::wvalue payload;
    payload["collection"] = "restaurants";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("find", payload);
    auto json = parseResponse(response);
    
    std::vector<Models::Restaurant> results;
    if (json && json.has("documents")) {
        for (const auto& doc : json["documents"]) {
            results.emplace_back(
                doc["id"].s(),
                doc["name"].s(),
                doc["city"].s(),
                doc["category"].s(),
                doc["cuisine"].s(),
                doc["rating"].d(),
                doc["logoURL"].s(),
                doc["latitude"].d(),
                doc["longitude"].d()
            );
        }
    }
    return results;
}

std::vector<Models::MenuItem> MongoDBClient::getMenuItems(const std::string& restaurantId) {
    crow::json::wvalue filter;
    filter["restaurant_id"] = restaurantId;
    
    crow::json::wvalue payload;
    payload["collection"] = "menuItems";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("find", payload);
    auto json = parseResponse(response);
    
    std::vector<Models::MenuItem> results;
    if (json && json.has("documents")) {
        for (const auto& doc : json["documents"]) {
            results.emplace_back(
                doc["id"].s(),
                doc["restaurant_id"].s(),
                doc["itemName"].s(),
                doc["price"].d(),
                doc["imageURL"].s()
            );
        }
    }
    return results;
}

Models::Review MongoDBClient::createReview(const std::string& userId, const std::string& restaurantId,
                           int tasteRating, int ambianceRating, int overallRating,
                           const std::string& comment) {
    std::string id = generateId();
    
    crow::json::wvalue document;
    document["id"] = id;
    document["user_id"] = userId;
    document["restaurant_id"] = restaurantId;
    document["tasteRating"] = tasteRating;
    document["ambianceRating"] = ambianceRating;
    document["overallRating"] = overallRating;
    document["comment"] = comment;
    document["timestamp"] = (long long)std::time(nullptr);
    
    crow::json::wvalue payload;
    payload["collection"] = "reviews";
    payload["document"] = std::move(document);
    
    performRequest("insertOne", payload);
    
    // Create and return review object
    Models::Review review(id, userId, restaurantId, tasteRating, ambianceRating, overallRating, comment);
    review.timestamp = (long long)std::time(nullptr);
    return review;
}

bool MongoDBClient::hasUserReviewed(const std::string& userId, const std::string& restaurantId) {
    crow::json::wvalue filter;
    filter["user_id"] = userId;
    filter["restaurant_id"] = restaurantId;
    
    crow::json::wvalue payload;
    payload["collection"] = "reviews";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("findOne", payload);
    auto json = parseResponse(response);
    
    if (json && json.has("document") && json["document"].t() != crow::json::type::Null) {
        return true;
    }
    return false;
}

std::vector<Models::Review> MongoDBClient::getReviews(const std::string& restaurantId) {
    crow::json::wvalue filter;
    filter["restaurant_id"] = restaurantId;
    
    crow::json::wvalue sort;
    sort["timestamp"] = -1;
    
    crow::json::wvalue payload;
    payload["collection"] = "reviews";
    payload["filter"] = std::move(filter);
    payload["sort"] = std::move(sort);
    
    std::string response = performRequest("find", payload);
    auto json = parseResponse(response);
    
    std::vector<Models::Review> results;
    if (json && json.has("documents")) {
        for (const auto& doc : json["documents"]) {
            Models::Review review(
                doc["id"].s(),
                doc["user_id"].s(),
                doc["restaurant_id"].s(),
                doc["tasteRating"].i(),
                doc["ambianceRating"].i(),
                doc["overallRating"].i(),
                doc["comment"].s()
            );
            
            // Handle timestamp safely if missing
            if (doc.has("timestamp")) {
                if (doc["timestamp"].t() == crow::json::type::Number)
                    review.timestamp = (long long)doc["timestamp"].i();
                else 
                    review.timestamp = 0;
            }

            // Fetch user name
            Models::User* user = getUserById(review.user_id);
            if (user) {
                review.userName = user->name;
            }
            results.push_back(review);
        }
    }
    return results;
}

std::vector<Models::Review> MongoDBClient::getUserReviews(const std::string& userId) {
    crow::json::wvalue filter;
    filter["user_id"] = userId;
    
    crow::json::wvalue payload;
    payload["collection"] = "reviews";
    payload["filter"] = std::move(filter);
    
    std::string response = performRequest("find", payload);
    auto json = parseResponse(response);
    
    std::vector<Models::Review> results;
    if (json && json.has("documents")) {
        for (const auto& doc : json["documents"]) {
            Models::Review review(
                doc["id"].s(),
                doc["user_id"].s(),
                doc["restaurant_id"].s(),
                doc["tasteRating"].i(),
                doc["ambianceRating"].i(),
                doc["overallRating"].i(),
                doc["comment"].s()
            );
             if (doc.has("timestamp")) {
                review.timestamp = (long long)doc["timestamp"].i();
            }
            results.push_back(review);
        }
    }
    return results;
}

void MongoDBClient::seedSampleData() {
    // Check if restaurants exist
    auto restaurants = getAllRestaurants();
    if (!restaurants.empty()) {
        std::cout << "Database already has data. Skipping seed." << std::endl;
        return;
    }
    
    std::cout << "Seeding sample data..." << std::endl;
    
    std::vector<Models::Restaurant> sampleRestaurants = {
        Models::Restaurant("r001", "La Bella Italia", "Islamabad", "Fine Dining", "Italian", 4.7, 
                          "https://img.freepik.com/free-photo/italian-food-background_1220-4318.jpg", 33.6844, 73.0479),
        Models::Restaurant("r002", "Sushi World", "Islamabad", "Fine Dining", "Japanese", 4.8,
                          "https://img.freepik.com/free-photo/sushi-set-table_140725-5024.jpg", 33.7077, 73.0478),
        Models::Restaurant("r003", "Spice Garden", "Lahore", "Casual Dining", "Pakistani", 4.5,
                          "https://img.freepik.com/free-photo/chicken-karahi-pakistani-indian-food_1205-11026.jpg", 31.5204, 74.3587),
        Models::Restaurant("r004", "Burger Lab", "Karachi", "Fast Food", "American", 4.4,
                          "https://img.freepik.com/free-photo/front-view-burger-stand_141793-15542.jpg", 24.8607, 67.0011),
        Models::Restaurant("r005", "Tandoori Nights", "Islamabad", "Casual Dining", "Pakistani", 4.6,
                          "https://img.freepik.com/free-photo/chicken-tikka-skewers_1220-4493.jpg", 33.69, 73.05),
        Models::Restaurant("r006", "Wok This Way", "Lahore", "Fast Food", "Chinese", 4.3,
                          "https://img.freepik.com/free-photo/chinese-food-table_140725-2426.jpg", 31.55, 74.33)
    };
    
    crow::json::wvalue::list docs;
    for (const auto& r : sampleRestaurants) {
        crow::json::wvalue doc;
        doc["id"] = r.id;
        doc["name"] = r.name;
        doc["city"] = r.city;
        doc["category"] = r.category;
        doc["cuisine"] = r.cuisine;
        doc["rating"] = r.rating;
        doc["logoURL"] = r.logoURL;
        doc["latitude"] = r.latitude;
        doc["longitude"] = r.longitude;
        docs.push_back(std::move(doc));
    }
    
    crow::json::wvalue payload;
    payload["collection"] = "restaurants";
    payload["documents"] = std::move(docs);
    
    performRequest("insertMany", payload);
}

} // namespace Database
