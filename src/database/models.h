#pragma once
#include <string>
#include <vector>
#include <ctime>

namespace Models {

// User model
struct User {
    std::string id;
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string city;
    
    User() = default;
    User(const std::string& id, const std::string& name, const std::string& email, 
         const std::string& hash, const std::string& city)
        : id(id), name(name), email(email), passwordHash(hash), city(city) {}
};

// Restaurant model
struct Restaurant {
    std::string id;
    std::string name;
    std::string city;
    std::string category;
    std::string cuisine;
    double rating;
    std::string logoURL;
    double latitude;
    double longitude;
    
    Restaurant() : rating(0.0), latitude(0.0), longitude(0.0) {}
    Restaurant(const std::string& id, const std::string& name, const std::string& city,
               const std::string& category, const std::string& cuisine, double rating,
               const std::string& logo, double lat, double lng)
        : id(id), name(name), city(city), category(category), cuisine(cuisine),
          rating(rating), logoURL(logo), latitude(lat), longitude(lng) {}
};

// MenuItem model
struct MenuItem {
    std::string id;
    std::string restaurant_id;
    std::string itemName;
    double price;
    std::string imageURL;
    
    MenuItem() : price(0.0) {}
    MenuItem(const std::string& id, const std::string& rest_id, const std::string& name,
             double price, const std::string& img)
        : id(id), restaurant_id(rest_id), itemName(name), price(price), imageURL(img) {}
};

// Review model
struct Review {
    std::string id;
    std::string user_id;
    std::string restaurant_id;
    int tasteRating;
    int ambianceRating;
    int overallRating;
    std::string comment;
    std::time_t timestamp;
    std::string userName; // For joined queries
    
    Review() : tasteRating(0), ambianceRating(0), overallRating(0), timestamp(0) {}
    Review(const std::string& id, const std::string& uid, const std::string& rid,
           int taste, int ambiance, int overall, const std::string& comment, const std::string& uName = "")
        : id(id), user_id(uid), restaurant_id(rid), tasteRating(taste),
          ambianceRating(ambiance), overallRating(overall), comment(comment), userName(uName) {
        timestamp = std::time(nullptr);
    }
};

// Location for route finding
struct Location {
    std::string name;
    double latitude;
    double longitude;
    
    Location() : latitude(0.0), longitude(0.0) {}
    Location(const std::string& name, double lat, double lng)
        : name(name), latitude(lat), longitude(lng) {}
};

} // namespace Models
