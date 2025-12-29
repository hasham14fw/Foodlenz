#include "csv_database.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>

namespace Database {

    CSVDatabase::CSVDatabase() {
        const char* envToken = std::getenv("GITHUB_TOKEN");
        if (envToken) githubToken = std::string(envToken);
        else { 
             // Try to read from .env manually if getenv fails
             std::ifstream envFile(".env");
             if (!envFile.is_open()) {
                 envFile.open("../.env");
             }
             
             std::string line;
             while(std::getline(envFile, line)) {
                  if(line.find("GITHUB_TOKEN=") == 0) {
                      githubToken = line.substr(13);
                      if(!githubToken.empty() && githubToken.back()=='\r') githubToken.pop_back();
                  }
             }
        }
        
        loadData();
    }

    void CSVDatabase::loadData() {
        std::cout << "Initializing Database from Cloud..." << std::endl;
        loadUsers();
        loadRestaurants();
        loadMenuItems();
        loadReviews();
        loadLocations();
    }

    std::string CSVDatabase::generateId() {
        return std::to_string(std::time(nullptr)) + "_" + std::to_string(std::rand());
    }

    DSA::DynamicArray<std::string> CSVDatabase::split(const std::string& line, char delimiter) {
        DSA::DynamicArray<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(line);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

     // Helper: Fetch URL using curl
    std::string CSVDatabase::fetchURL(const std::string& url) {
        std::cout << "Fetching " << url << "..." << std::endl;
        std::string cmd = "curl -s";
        if (!githubToken.empty()) {
            cmd += " -H \"Authorization: token " + githubToken + "\"";
        }
        cmd += " \"" + url + "\"";
        
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
        if (!pipe) {
            std::cerr << "Failed to run curl command." << std::endl;
            return "";
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        
        // Basic check for 404
        if (result.find("404: Not Found") != std::string::npos) {
            std::cerr << "URL not found: " << url << std::endl;
            return ""; 
        }
        return result;
    }
    
    // --- Cloud Write Helpers ---
    
    // Base64 encoding table
    static const std::string base64_chars = 
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";

    std::string CSVDatabase::base64_encode(const std::string& in) {
        std::string out;
        int val = 0, valb = -6;
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back(base64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4) out.push_back('=');
        return out;
    }
    
    std::string CSVDatabase::getFileSHA(const std::string& filename) {
        // GET /repos/{owner}/{repo}/contents/{path}
        std::string url = "https://api.github.com/repos/" + REPO_OWNER + "/" + REPO_NAME + "/contents/" + filename;
        std::string response = fetchURL(url);
        
        // Very basic JSON parsing for "sha": "..."
        std::string search = "\"sha\": \"";
        size_t pos = response.find(search);
        if (pos != std::string::npos) {
            size_t end = response.find("\"", pos + search.length());
            return response.substr(pos + search.length(), end - (pos + search.length()));
        }
        return "";
    }
    
    void CSVDatabase::saveToCloud(const std::string& filename, const std::string& content) {
        if (githubToken.empty()) {
            std::cerr << "Cannot save to cloud: GITHUB_TOKEN missing." << std::endl;
            return;
        }
        
        std::string sha = getFileSHA(filename);
        std::string encodedContent = base64_encode(content);
        
        // Construct JSON body
        // Note: content must be the WHOLE file content, not just the append.
        // GitHub API replaces the file.
        // So we need to fetch existing content? NO, we just read it into memory on startup.
        // But we need to make sure we reconstruct the CSV properly.
        // Since we don't keep the full CSV raw string in memory, we have to re-serialize our data structures!
        // This is expensive but necessary for this implementation.
        // OR: fetch current content, append, then push. 
        // Let's go with: Load current content from Cloud (fresh), append new line, push back.
        // It's safer against race conditions (slightly) and easier than re-serializing everything.
        
        std::string currentContent = fetchURL(CLOUD_BASE + filename);
        if(!currentContent.empty() && currentContent.back() != '\n') currentContent += "\n";
        
        std::string newContent = currentContent + content;
        std::string newEncoded = base64_encode(newContent);
        
        // Escape json strings
        auto escape = [](const std::string& s) { /* simplified */ return s; }; 
        
        std::string jsonBody = "{";
        jsonBody += "\"message\": \"Update " + filename + " via Backend\",";
        jsonBody += "\"content\": \"" + newEncoded + "\",";
        if (!sha.empty()) jsonBody += "\"sha\": \"" + sha + "\",";
        jsonBody += "\"branch\": \"" + BRANCH + "\"";
        jsonBody += "}";
        
        // Use curl to PUT
        std::string url = "https://api.github.com/repos/" + REPO_OWNER + "/" + REPO_NAME + "/contents/" + filename;
        
        std::cout << "Pushing update to " << filename << "..." << std::endl;
        
        // Write body to temp file to avoid command line length limits
        std::ofstream tmp("body.json");
        tmp << jsonBody;
        tmp.close();
        
        // Capture output to see GitHub response
        std::string cmd = "curl -v -X PUT -H \"Authorization: token " + githubToken + "\" "
                          "-H \"Content-Type: application/json\" "
                          "-d @body.json \"" + url + "\" 2>&1";
                          
        std::string output;
        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
        
        if (!pipe) {
             std::cerr << "Failed to run curl push command." << std::endl;
        } else {
             while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                output += buffer.data();
             }
             std::cout << "GitHub API Response: " << output << std::endl;
             
             if (output.find("\"commit\"") != std::string::npos) {
                  std::cout << "Successfully pushed to GitHub." << std::endl;
             } else {
                  std::cerr << "FAILED to push to GitHub. Check response above." << std::endl;
             }
        }
        
        std::remove("body.json");
    }

    void CSVDatabase::loadUsers() {
        std::string content = fetchURL(CLOUD_BASE + "users.csv");
        std::istringstream stream(content);
        if (content.empty()) {
            std::cout << "Fallback to local users.csv" << std::endl;
            std::ifstream localFile(USERS_FILE);
            std::stringstream buffer;
            buffer << localFile.rdbuf();
            stream.str(buffer.str());
        }
        
        std::string line;
        if (std::getline(stream, line)) {} // process header
        
        while (std::getline(stream, line)) {
            // Remove \r if present
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            
            auto tokens = split(line, ',');
            if (tokens.size() >= 5) {
                Models::User user(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]);
                usersById.insert(user.id, user);
                usersByEmail.insert(user.email, user);
            }
        }
    }

    void CSVDatabase::loadRestaurants() {
        std::string content = fetchURL(CLOUD_BASE + "restaurants.csv");
        std::istringstream stream(content);
        if (content.empty()) {
            std::cout << "Fallback to local restaurants.csv" << std::endl;
            std::ifstream localFile(RESTAURANTS_FILE);
            std::stringstream buffer;
            buffer << localFile.rdbuf();
            stream.str(buffer.str());
        }

        std::string line;
        if (std::getline(stream, line)) {} // header
        
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            
            auto tokens = split(line, ',');
            if (tokens.size() >= 9) {
                try {
                    Models::Restaurant r(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                                         std::stod(tokens[5]), tokens[6], std::stod(tokens[7]), std::stod(tokens[8]));
                    
                    allRestaurants.push_back(r);
                    restaurantsById.insert(r.id, r);
                    trendingHeap.insert(r);
                    searchBST.insert(r);
                    cuisineTree.insert(r.cuisine, r);
                } catch(...) {
                     std::cerr << "Error parsing restaurant line: " << line << std::endl;
                }
            }
        }
    }

    void CSVDatabase::loadMenuItems() {
        std::string content = fetchURL(CLOUD_BASE + "menu_items.csv");
        std::istringstream stream(content);
        if (content.empty()) { // Fallback/Empty check
             std::ifstream localFile(MENU_FILE);
             std::stringstream buffer;
             buffer << localFile.rdbuf();
             stream.str(buffer.str());
        }

        std::string line;
        if (std::getline(stream, line)) {} // header
        
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            
            auto tokens = split(line, ',');
            if (tokens.size() >= 5) {
                try {
                    Models::MenuItem item(tokens[0], tokens[1], tokens[2], std::stod(tokens[3]), tokens[4]);
                    
                    DSA::DynamicArray<Models::MenuItem>* items = menuItemsByRestaurantId.get(item.restaurant_id);
                    if (!items) {
                        DSA::DynamicArray<Models::MenuItem> newList;
                        newList.push_back(item);
                        menuItemsByRestaurantId.insert(item.restaurant_id, newList);
                    } else {
                        items->push_back(item);
                    }
                } catch(...) {}
            }
        }
    }

    void CSVDatabase::loadReviews() {
        std::string content = fetchURL(CLOUD_BASE + "reviews.csv");
        std::istringstream stream(content);
        if (content.empty()) {
             std::ifstream localFile(REVIEWS_FILE);
             std::stringstream buffer;
             buffer << localFile.rdbuf();
             stream.str(buffer.str());
        }

        std::string line;
        if (std::getline(stream, line)) {} // header
        
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            
            auto tokens = split(line, ',');
            if (tokens.size() >= 8) {
                try {
                    // Try to parse userName if present (column index 8)
                    std::string uName = "Unknown User";
                    if (tokens.size() >= 9) {
                        uName = tokens[8];
                        // Handle potential carriage return in the last token if not handled by split/getline clean up
                        if (!uName.empty() && uName.back() == '\r') uName.pop_back();
                    }

                    Models::Review rev(tokens[0], tokens[1], tokens[2], std::stoi(tokens[3]), 
                                      std::stoi(tokens[4]), std::stoi(tokens[5]), tokens[6], uName);
                    try {
                        rev.timestamp = std::stoll(tokens[7]);
                    } catch(...) { rev.timestamp = std::time(nullptr); }
    
                    // Add to Restaurant Reviews
                    auto* restReviews = reviewsByRestaurantId.get(rev.restaurant_id);
                    if (!restReviews) {
                        DSA::DynamicArray<Models::Review> newList;
                        newList.push_back(rev);
                        reviewsByRestaurantId.insert(rev.restaurant_id, newList);
                    } else {
                        restReviews->push_back(rev);
                    }
                    
                    // Add to User Reviews
                    auto* userReviews = reviewsByUserId.get(rev.user_id);
                    if (!userReviews) {
                        DSA::DynamicArray<Models::Review> newList;
                        newList.push_back(rev);
                        reviewsByUserId.insert(rev.user_id, newList);
                    } else {
                        userReviews->push_back(rev);
                    }
                } catch(...) {}
            }
        }
    }

    void CSVDatabase::loadLocations() {
        std::string content = fetchURL(CLOUD_BASE + "locations.csv");
        std::istringstream stream(content);
        if (content.empty()) {
             std::ifstream localFile(LOCATIONS_FILE);
             std::stringstream buffer;
             buffer << localFile.rdbuf();
             stream.str(buffer.str());
        }

        std::string line;
        if (std::getline(stream, line)) {} // header
        
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            
            auto tokens = split(line, ',');
            if (tokens.size() >= 3) {
                std::string name = tokens[0];
                cityGraph.addVertex(name);
            }
        }
        
        // Build Graph Edges (Mock Logic remains)
        auto rests = allRestaurants;
        for(size_t i=0; i<rests.size(); i++) {
            cityGraph.addVertex(rests[i].name);
            if (rests[i].city == "Islamabad") {
                 cityGraph.addEdge("Islamabad Center", rests[i].name, 5.0);
                 cityGraph.addEdge("F-6 Islamabad", rests[i].name, 3.0);
            } else if (rests[i].city == "Lahore") {
                 cityGraph.addEdge("Lahore Center", rests[i].name, 5.0);
            }
        }
    }

    Models::User CSVDatabase::createUser(const std::string& name, const std::string& email, 
                           const std::string& passwordHash, const std::string& city) {
        if (usersByEmail.contains(email)) {
             throw std::runtime_error("User already exists");
        }
        
        Models::User newUser(generateId(), name, email, passwordHash, city);
        
        // Save to Memory
        usersById.insert(newUser.id, newUser);
        usersByEmail.insert(newUser.email, newUser);
        
        // Format for CSV
        std::string row = newUser.id + "," + newUser.name + "," + newUser.email + ","
                        + newUser.passwordHash + "," + newUser.city + "\n";
             
        std::cout << "Saving new user to Cloud..." << std::endl;
        saveToCloud("users.csv", row);
        
        return newUser;
    }
    
    Models::User* CSVDatabase::getUserByEmail(const std::string& email) {
        return usersByEmail.get(email);
    }
    
    Models::User* CSVDatabase::getUserById(const std::string& id) {
        return usersById.get(id);
    }
    
    DSA::DynamicArray<Models::Restaurant> CSVDatabase::getAllRestaurants() {
        return allRestaurants;
    }
    
    DSA::DynamicArray<Models::Restaurant> CSVDatabase::getTrendingRestaurants(int limit) {
        return trendingHeap.getTopN(limit);
    }
    
    Models::Restaurant* CSVDatabase::getRestaurantById(const std::string& id) {
        return restaurantsById.get(id);
    }
    
    DSA::DynamicArray<Models::Restaurant> CSVDatabase::searchRestaurants(const std::string& query,
                                                      const std::string& cuisine,
                                                      const std::string& city,
                                                      double minRating) {
        DSA::DynamicArray<Models::Restaurant> results;
        DSA::DynamicArray<Models::Restaurant> candidates;
        
        if (!query.empty()) {
            candidates = searchBST.searchByPrefix(query);
        } else if (!cuisine.empty()) {
            candidates = cuisineTree.getRestaurantsByCuisine(cuisine);
        } else {
            candidates = allRestaurants;
        }
        
        for (size_t i = 0; i < candidates.size(); i++) {
            Models::Restaurant r = candidates[i];
            bool match = true;
            
            if (!cuisine.empty() && r.cuisine != cuisine) match = false;
            if (!city.empty() && r.city != city) match = false;
            if (r.rating < minRating) match = false;
            
            if (!query.empty() && candidates.size() == allRestaurants.size()) {
                 std::string lowerName = r.name;
                 std::string lowerQuery = query;
                 std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                 std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
                 if (lowerName.find(lowerQuery) == std::string::npos) match = false;
             }
            
            if (match) results.push_back(r);
        }
        
        return results;
    }
    
    DSA::DynamicArray<Models::MenuItem> CSVDatabase::getMenuItems(const std::string& restaurantId) {
        DSA::DynamicArray<Models::MenuItem>* items = menuItemsByRestaurantId.get(restaurantId);
        if (items) return *items;
        return DSA::DynamicArray<Models::MenuItem>();
    }
    
    Models::Review CSVDatabase::createReview(const std::string& userId, const std::string& restaurantId,
                                int tasteRating, int ambianceRating, int overallRating,
                                const std::string& comment) {
                                
        if (hasUserReviewed(userId, restaurantId)) {
             throw std::runtime_error("User has already reviewed this restaurant");
        }
        
        // Fetch user name
        std::string userName = "Unknown User";
        Models::User* u = getUserById(userId);
        if (u) userName = u->name;

        Models::Review rev(generateId(), userId, restaurantId, tasteRating, ambianceRating, overallRating, comment, userName);
        
        auto* restList = reviewsByRestaurantId.get(restaurantId);
        if (!restList) {
             DSA::DynamicArray<Models::Review> n; n.push_back(rev);
             reviewsByRestaurantId.insert(restaurantId, n);
        } else restList->push_back(rev);
        
        auto* userList = reviewsByUserId.get(userId);
        if (!userList) {
             DSA::DynamicArray<Models::Review> n; n.push_back(rev);
             reviewsByUserId.insert(userId, n);
        } else userList->push_back(rev);
        
        // Format for CSV
        std::string row = rev.id + "," + rev.user_id + "," + rev.restaurant_id + ","
             + std::to_string(rev.tasteRating) + "," + std::to_string(rev.ambianceRating) + "," + std::to_string(rev.overallRating) + ","
             + rev.comment + "," + std::to_string(rev.timestamp) + "," + rev.userName + "\n";
             
        std::cout << "Saving new review to Cloud..." << std::endl;
        saveToCloud("reviews.csv", row);
        
        return rev;
    }
    
    bool CSVDatabase::hasUserReviewed(const std::string& userId, const std::string& restaurantId) {
        DSA::DynamicArray<Models::Review>* userReviews = reviewsByUserId.get(userId);
        if (userReviews) {
            for(size_t i=0; i<userReviews->size(); i++) {
                if ((*userReviews)[i].restaurant_id == restaurantId) return true;
            }
        }
        return false;
    }
    
    DSA::DynamicArray<Models::Review> CSVDatabase::getReviews(const std::string& restaurantId) {
        DSA::DynamicArray<Models::Review>* list = reviewsByRestaurantId.get(restaurantId);
        if (!list) return DSA::DynamicArray<Models::Review>();
        
        DSA::DynamicArray<Models::Review> decorated = *list; 
        for(size_t i=0; i<decorated.size(); i++) {
            Models::User* u = getUserById(decorated[i].user_id);
            if (u) decorated[i].userName = u->name;
            else decorated[i].userName = "Unknown User";
        }
        return decorated;
    }
    
    DSA::DynamicArray<Models::Review> CSVDatabase::getUserReviews(const std::string& userId) {
        DSA::DynamicArray<Models::Review>* list = reviewsByUserId.get(userId);
        if (list) return *list;
        return DSA::DynamicArray<Models::Review>();
    }
    
    DSA::Graph& CSVDatabase::getCityGraph() { return cityGraph; }

} // namespace Database
