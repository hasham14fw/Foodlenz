#define _USE_MATH_DEFINES
#include "routes.h"
#include "dsa/trie.h"
#include <string>
#include <iostream>
#include <vector>

// Helper for tolower
static void strToLower(std::string& s) {
    for(size_t i=0; i<s.length(); i++) {
        if(s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] + 32;
        }
    }
}

// Simple Token Gen without ctime
static std::string generateToken(const std::string& userId) {
    static int counter = 0;
    counter++;
    return "token_" + userId + "_" + std::to_string(counter);
}

static std::string extractUserIdFromToken(const std::string& token) {
    size_t pos1 = 0;
    size_t pos2 = 0;
    
    for(size_t i=0; i<token.length(); i++) {
        if(token[i] == '_') {
            pos1 = i;
            break;
        }
    }
    
    for(size_t i=token.length()-1; i>0; i--) {
        if(token[i] == '_') {
            pos2 = i;
            break;
        }
    }
    
    if (pos1 != 0 && pos2 != 0 && pos1 != pos2) {
        return token.substr(pos1 + 1, pos2 - pos1 - 1);
    }
    return "";
}

// Simple password hashing
static std::string hashPassword(const std::string& password) {
    std::string hashed = "hash_" + password;
    return hashed;
}

// Chatbot state management
struct ChatConversation {
    enum State { GREETING, ASK_CUISINE, ASK_BUDGET, ASK_CITY, COMPLETED };
    State state = GREETING;
    std::string cuisine;
    int budget = 0;
    std::string city;
};

// Replace std::map with std::vector<pair>
struct SessionEntry {
    std::string userId;
    ChatConversation conversation;
};

static std::vector<SessionEntry> chatSessions;

static ChatConversation& getSession(const std::string& userId) {
    for(auto& session : chatSessions) {
        if (session.userId == userId) {
            return session.conversation;
        }
    }
    ChatConversation newConv;
    chatSessions.push_back({userId, newConv});
    return chatSessions.back().conversation;
}

static void removeSession(const std::string& userId) {
    for(size_t i=0; i<chatSessions.size(); i++) {
        if (chatSessions[i].userId == userId) {
            chatSessions.erase(chatSessions.begin() + i);
            return;
        }
    }
}

struct ChatIntent {
    enum Type { NONE, GREETING, RESET, CUISINE, CITY, BUDGET_KEY, HELP };
    Type type = NONE;
    std::string value;
};

static dsa::Trie<ChatIntent> chatbotTrie;
static bool isTrieInitialized = false;

static void initChatbotTrie() {
    if (isTrieInitialized) return;
    
    // Greetings
    chatbotTrie.insert("hi", {ChatIntent::GREETING, ""});
    chatbotTrie.insert("hello", {ChatIntent::GREETING, ""});
    chatbotTrie.insert("hey", {ChatIntent::GREETING, ""});
    chatbotTrie.insert("greetings", {ChatIntent::GREETING, ""});
    
    // Reset
    chatbotTrie.insert("reset", {ChatIntent::RESET, ""});
    chatbotTrie.insert("start over", {ChatIntent::RESET, ""});
    chatbotTrie.insert("restart", {ChatIntent::RESET, ""});
    
    // Cuisines
    std::vector<std::string> cuisines = {"italian", "chinese", "pakistani", "japanese", "indian", "french", "korean", "mexican", "thai", "american", "fast food", "continental", "desi"};
    for(const auto& c : cuisines) {
        chatbotTrie.insert(c, {ChatIntent::CUISINE, c});
        chatbotTrie.insert(c + " food", {ChatIntent::CUISINE, c});
    }

    // Cities
    chatbotTrie.insert("islamabad", {ChatIntent::CITY, "Islamabad"});
    chatbotTrie.insert("lahore", {ChatIntent::CITY, "Lahore"});
    chatbotTrie.insert("karachi", {ChatIntent::CITY, "Karachi"});
    chatbotTrie.insert("rawalpindi", {ChatIntent::CITY, "Rawalpindi"});

    // Budget keys (simple detection of intent to talk about money)
    chatbotTrie.insert("budget", {ChatIntent::BUDGET_KEY, ""});
    chatbotTrie.insert("price", {ChatIntent::BUDGET_KEY, ""});
    chatbotTrie.insert("cost", {ChatIntent::BUDGET_KEY, ""});

    isTrieInitialized = true;
}

// Helper to extract budget number from string
static int extractBudget(const std::string& text) {
    int budget = 0;
    std::string temp;
    // Manual parsing logic
    std::string currentNum;
    for (char c : text) {
        if (c >= '0' && c <= '9') {
            currentNum += c;
        } else {
            if (!currentNum.empty()) {
                // Parse
                 try {
                     int val = std::stoi(currentNum);
                     if (val > 100 && val < 100000) {
                         budget = val;
                         break;
                     }
                 } catch(...) {}
                 currentNum = "";
            }
        }
    }
    // Check last
    if (!currentNum.empty()) {
         try {
             int val = std::stoi(currentNum);
             if (val > 100 && val < 100000) {
                 budget = val;
             }
         } catch(...) {}
    }
    return budget;
}


// Chatbot pattern matching
static crow::json::wvalue processChatbotQuery(const std::string& userId, const std::string& message, Database::IDatabase& db) {
    crow::json::wvalue response;
    std::string lowerMsg = message;
    strToLower(lowerMsg);
    
    if (!isTrieInitialized) {
        initChatbotTrie();
    }
    
    // Use Trie to extract all known intents/entities from the message
    auto intents = chatbotTrie.searchInText(message);
    
    // Check for RESET or GREETING first
    for(const auto& item : intents) {
        if (item.second.type == ChatIntent::RESET) {
            removeSession(userId);
            response["message"] = "Okay, let's start over! What kind of cuisine are you looking for?";
            ChatConversation& session = getSession(userId);
            session.state = ChatConversation::ASK_CUISINE;
            return response;
        }
    }
    
    ChatConversation& session = getSession(userId);
    
    bool foundCuisine = false;
    bool foundCity = false;
    
    for(const auto& item : intents) {
        if (item.second.type == ChatIntent::CUISINE) {
            session.cuisine = item.second.value;
            if(!session.cuisine.empty()) session.cuisine[0] = toupper(session.cuisine[0]);
            foundCuisine = true;
        } else if (item.second.type == ChatIntent::CITY) {
            session.city = item.second.value;
            foundCity = true;
        }
    }
    
    // Try to find budget if ANY number is present
    int extractedBudget = extractBudget(message);
    if (extractedBudget > 0) {
        session.budget = extractedBudget;
    }

    // Smart State Transition Logic
    // If we have cuisine and city, we can probably search, regardless of current state.
    if (!session.cuisine.empty() && !session.city.empty()) {
        // We have enough info to search!
        session.state = ChatConversation::COMPLETED; // Or ready to show results
    } else if (!session.cuisine.empty()) {
        // We have cuisine, need city (and maybe budget)
         if (session.state == ChatConversation::ASK_CUISINE || session.state == ChatConversation::GREETING) {
             session.state = ChatConversation::ASK_BUDGET;
         }
    }
    
    // --- State Machine Execution ---
    
    // 1. GREETING
    if (session.state == ChatConversation::GREETING) {
        // Check if user just said "hi" without extra info
        bool justGreeting = false;
        for(const auto& item : intents) {
            if (item.second.type == ChatIntent::GREETING) justGreeting = true;
        }
        
        if (justGreeting && !foundCuisine && !foundCity) {
            response["message"] = "Hello! I can help you find a restaurant. What kind of cuisine are you craving? (e.g., Italian, Chinese)";
            session.state = ChatConversation::ASK_CUISINE;
            return response;
        } else {
            // User might have said "Hi I want Italian", so move to next state
            session.state = ChatConversation::ASK_CUISINE;
            // Fallthrough to next checks
        }
    }
    
    // 2. ASK_CUISINE
    if (session.state == ChatConversation::ASK_CUISINE) {
        if (!session.cuisine.empty()) {
             // Cuisine found (either previously or just now)
             if (session.budget == 0) {
                 response["message"] = "Great choice! " + session.cuisine + ". What is your budget per person? (e.g., 2000)";
                 session.state = ChatConversation::ASK_BUDGET;
                 return response;
             } else {
                 session.state = ChatConversation::ASK_BUDGET;
                 // Fallthrough
             }
        } else {
             response["message"] = "What kind of cuisine would you like? (Italian, Chinese, Pakistani, etc.)";
             return response; // Stay in ASK_CUISINE
        }
    }
    
    // 3. ASK_BUDGET
    if (session.state == ChatConversation::ASK_BUDGET) {
        if (session.budget > 0) {
            if (session.city.empty()) {
                response["message"] = "Got the budget. Which city are you in? (Islamabad, Lahore, Karachi)";
                session.state = ChatConversation::ASK_CITY;
                return response;
            } else {
                session.state = ChatConversation::ASK_CITY;
                // Fallthrough
            }
        } else {
             // If user explicitly asked to skip or said "any", we might handle that, but for now Ask Again or check keywords
             response["message"] = "How much would you like to spend? (e.g. 1500)";
             return response;
        }
    }
    
    // 4. ASK_CITY (or Final Search)
    if (session.state == ChatConversation::ASK_CITY || (!session.cuisine.empty() && !session.city.empty())) {
        if (!session.city.empty()) {
            // PERFORM SEARCH
             auto restaurants = db.searchRestaurants("", session.cuisine, session.city);
             
             if (!restaurants.empty()) {
                response["message"] = "Here are " + session.cuisine + " restaurants in " + session.city + ":";
                crow::json::wvalue::list suggestions;
                int count = 0;
                for (const auto& r : restaurants) {
                    if (count++ >= 5) break; 
                    crow::json::wvalue rest;
                    rest["id"] = r.id;
                    rest["name"] = r.name;
                    rest["cuisine"] = r.cuisine;
                    rest["rating"] = r.rating;
                    rest["city"] = r.city;
                    rest["logoURL"] = r.logoURL;
                    suggestions.push_back(std::move(rest));
                }
                response["restaurants"] = std::move(suggestions);
             } else {
                 response["message"] = "I couldn't find any " + session.cuisine + " restaurants in " + session.city + ". try another city or 'reset'.";
                 response["restaurants"] = crow::json::wvalue::list();
             }
             // Reset for next conversation, or keep context? 
             // Let's keep context but go to GREETING for new requests
             session.state = ChatConversation::GREETING; 
             session.cuisine = "";
             session.city = "";
             session.budget = 0;
             return response;
        } else {
             response["message"] = "Which city?";
             return response;
        }
    }
    
    response["message"] = "I'm not sure. Try saying 'start over'.";
    return response;
}

void setupRoutes(crow::SimpleApp& app, Database::IDatabase& db, 
                Algorithms::RecommendationEngine& recommender,
                Algorithms::RouteFinder& routeFinder) {

    // Initialize Trie on startup
    initChatbotTrie();

    // CORS Handler
    auto corsHandler = []() {
        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return res;
    };

    CROW_ROUTE(app, "/").methods("OPTIONS"_method)(corsHandler);
    CROW_ROUTE(app, "/api/login").methods("OPTIONS"_method)(corsHandler);
    CROW_ROUTE(app, "/api/signup").methods("OPTIONS"_method)(corsHandler);
    CROW_ROUTE(app, "/api/chatbot").methods("OPTIONS"_method)(corsHandler);
    CROW_ROUTE(app, "/api/route").methods("OPTIONS"_method)(corsHandler);
    
    // Wildcard CORS
    CROW_ROUTE(app, "/api/restaurants/<string>").methods("OPTIONS"_method)
    ([](const std::string&){
        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return res;
    });

    CROW_ROUTE(app, "/api/restaurants/<string>/reviews").methods("OPTIONS"_method)
    ([](const std::string&){
        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return res;
    });

    // Serve static files
    CROW_ROUTE(app, "/")
    ([](const crow::request&, crow::response& res){
        res.set_static_file_info("public/signin.html");
        res.end();
    });

    CROW_ROUTE(app, "/css/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/css/" + filename);
        res.end();
    });
    
    CROW_ROUTE(app, "/js/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/js/" + filename);
        res.end();
    });
    
    CROW_ROUTE(app, "/api/signup").methods("POST"_method)
    ([&db](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        std::string name = body["name"].s();
        std::string email = body["email"].s();
        std::string password = body["password"].s();
        std::string city = body["city"].s();
        
        if (db.getUserByEmail(email) != nullptr) {
            crow::json::wvalue error;
            error["error"] = "Email already registered";
            crow::response res(409, error);
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        std::string passwordHash = hashPassword(password);
        auto user = db.createUser(name, email, passwordHash, city);
        std::string token = generateToken(user.id);
        
        crow::json::wvalue response;
        response["user"]["id"] = user.id;
        response["user"]["name"] = user.name;
        response["user"]["email"] = user.email;
        response["user"]["city"] = user.city;
        response["token"] = token;
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // POST /api/login
    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([&db](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        std::string email = body["email"].s();
        std::string password = body["password"].s();
        
        auto* user = db.getUserByEmail(email);
        if (user == nullptr) {
            crow::json::wvalue error;
            error["error"] = "Invalid credentials";
            crow::response res(401, error);
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        std::string passwordHash = hashPassword(password);
        if (user->passwordHash != passwordHash) {
            crow::json::wvalue error;
            error["error"] = "Invalid credentials";
            crow::response res(401, error);
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        std::string token = generateToken(user->id);
        
        crow::json::wvalue response;
        response["user"]["id"] = user->id;
        response["user"]["name"] = user->name;
        response["user"]["email"] = user->email;
        response["user"]["city"] = user->city;
        response["token"] = token;
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    CROW_ROUTE(app, "/api/restaurants/trending")
    ([&db](){
        auto trending = db.getTrendingRestaurants(6);
        
        crow::json::wvalue::list restaurants;
        for (const auto& r : trending) {
            crow::json::wvalue rest;
            rest["id"] = r.id;
            rest["name"] = r.name;
            rest["city"] = r.city;
            rest["category"] = r.category;
            rest["cuisine"] = r.cuisine;
            rest["rating"] = r.rating;
            rest["logoURL"] = r.logoURL;
            restaurants.push_back(std::move(rest));
        }
        
        crow::json::wvalue response;
        response["restaurants"] = std::move(restaurants);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // GET /api/restaurants/search
    CROW_ROUTE(app, "/api/restaurants/search")
    ([&db](const crow::request& req){
        auto q = req.url_params.get("q");
        auto cuisine = req.url_params.get("cuisine");
        auto city = req.url_params.get("city");
        auto rating = req.url_params.get("rating");
        
        std::string query = q ? std::string(q) : "";
        std::string cuisineFilter = cuisine ? std::string(cuisine) : "";
        std::string cityFilter = city ? std::string(city) : "";
        double minRating = rating ? std::stod(std::string(rating)) : 0.0;
        
        auto results = db.searchRestaurants(query, cuisineFilter, cityFilter, minRating);
        
        crow::json::wvalue::list restaurants;
        for (const auto& r : results) {
            crow::json::wvalue rest;
            rest["id"] = r.id;
            rest["name"] = r.name;
            rest["city"] = r.city;
            rest["category"] = r.category;
            rest["cuisine"] = r.cuisine;
            rest["rating"] = r.rating;
            rest["logoURL"] = r.logoURL;
            restaurants.push_back(std::move(rest));
        }
        
        crow::json::wvalue response;
        response["restaurants"] = std::move(restaurants);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // GET /api/restaurants/recommended
    CROW_ROUTE(app, "/api/restaurants/recommended")
    ([&db, &recommender](const crow::request& req){
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.empty()) {
            crow::response res(401, "Unauthorized");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        std::string token = authHeader.substr(7);
        std::string userId = extractUserIdFromToken(token);
        
        auto recommended = recommender.getRecommendations(userId, 10);
        
        crow::json::wvalue::list restaurants;
        for (const auto& r : recommended) {
            crow::json::wvalue rest;
            rest["id"] = r.id;
            rest["name"] = r.name;
            rest["city"] = r.city;
            rest["category"] = r.category;
            rest["cuisine"] = r.cuisine;
            rest["rating"] = r.rating;
            rest["logoURL"] = r.logoURL;
            restaurants.push_back(std::move(rest));
        }
        
        crow::json::wvalue response;
        response["restaurants"] = std::move(restaurants);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // GET /api/restaurants/:id
    CROW_ROUTE(app, "/api/restaurants/<string>")
    ([&db](const std::string& id){
        auto* restaurant = db.getRestaurantById(id);
        if (!restaurant) {
            crow::response res(404, "Restaurant not found");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        crow::json::wvalue response;
        response["id"] = restaurant->id;
        response["name"] = restaurant->name;
        response["city"] = restaurant->city;
        response["category"] = restaurant->category;
        response["cuisine"] = restaurant->cuisine;
        response["rating"] = restaurant->rating;
        response["logoURL"] = restaurant->logoURL;
        response["latitude"] = restaurant->latitude;
        response["longitude"] = restaurant->longitude;
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // GET /api/restaurants/:id/menu
    CROW_ROUTE(app, "/api/restaurants/<string>/menu")
    ([&db](const std::string& id){
        auto menuItems = db.getMenuItems(id);
        
        crow::json::wvalue::list items;
        for (const auto& item : menuItems) {
            crow::json::wvalue menuItem;
            menuItem["id"] = item.id;
            menuItem["itemName"] = item.itemName;
            menuItem["price"] = item.price;
            menuItem["imageURL"] = item.imageURL;
            items.push_back(std::move(menuItem));
        }
        
        crow::json::wvalue response;
        response["menuItems"] = std::move(items);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // GET /api/restaurants/:id/reviews
    CROW_ROUTE(app, "/api/restaurants/<string>/reviews")
    ([&db](const std::string& id){
        auto reviews = db.getReviews(id);
        
        crow::json::wvalue::list reviewList;
        for (const auto& review : reviews) {
            crow::json::wvalue rev;
            rev["id"] = review.id;
            rev["userName"] = review.userName; // Set in CSVDatabase
            rev["tasteRating"] = review.tasteRating;
            rev["ambianceRating"] = review.ambianceRating;
            rev["overallRating"] = review.overallRating;
            rev["comment"] = review.comment;
            rev["timestamp"] = static_cast<long long>(review.timestamp);
            reviewList.push_back(std::move(rev));
        }
        
        crow::json::wvalue response;
        response["reviews"] = std::move(reviewList);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // POST /api/restaurants/:id/reviews
    CROW_ROUTE(app, "/api/restaurants/<string>/reviews").methods("POST"_method)
    ([&db](const crow::request& req, const std::string& id){
        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.empty()) {
            crow::response res(401, "Unauthorized");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        std::string token = authHeader.substr(7);
        std::string userId = extractUserIdFromToken(token);
        
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        int taste = body["tasteRating"].i();
        int ambiance = body["ambianceRating"].i();
        int overall = body["overallRating"].i();
        std::string comment = body["comment"].s();
        
        if (db.hasUserReviewed(userId, id)) {
            crow::json::wvalue error;
            error["error"] = "You have already reviewed this restaurant";
            crow::response res(409, error);
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        auto review = db.createReview(userId, id, taste, ambiance, overall, comment);
        
        crow::json::wvalue response;
        response["review"]["id"] = review.id;
        response["review"]["tasteRating"] = review.tasteRating;
        response["review"]["ambianceRating"] = review.ambianceRating;
        response["review"]["overallRating"] = review.overallRating;
        response["review"]["comment"] = review.comment;
        response["message"] = "Review submitted successfully";
        
        crow::response res(201, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    CROW_ROUTE(app, "/api/chatbot").methods("POST"_method)
    ([&db](const crow::request& req){
        auto authHeader = req.get_header_value("Authorization");
        std::string userId = "anonymous";
        if (!authHeader.empty()) {
             std::string token = authHeader.substr(7);
             userId = extractUserIdFromToken(token);
        }

        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        std::string message = body["message"].s();
        auto response = processChatbotQuery(userId, message, db);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    CROW_ROUTE(app, "/api/route").methods("POST"_method)
    ([&db, &routeFinder](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        std::string startLocation = body["startLocation"].s();
        std::string restaurantId = body["restaurantId"].s();
        
        auto* restaurant = db.getRestaurantById(restaurantId);
        if (!restaurant) {
            crow::response res(404, "Restaurant not found");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        
        Models::Location start(startLocation, 33.6844, 73.0479); 
        Models::Location destination(restaurant->name, restaurant->latitude, restaurant->longitude);
        
        auto routeResult = routeFinder.calculateRoute(start, destination);
        
        crow::json::wvalue response;
        if (routeResult.found) {
            // Fix destination coordinates in the path
            if (routeResult.waypoints.size() > 0) {
                auto& lastPoint = routeResult.waypoints[routeResult.waypoints.size() - 1];
                if (lastPoint.latitude == 0 && lastPoint.longitude == 0) {
                     lastPoint.latitude = restaurant->latitude;
                     lastPoint.longitude = restaurant->longitude;
                }
            }

            crow::json::wvalue::list waypoints;
            for (const auto& loc : routeResult.waypoints) {
                crow::json::wvalue point;
                point["name"] = loc.name;
                point["lat"] = loc.latitude;
                point["lng"] = loc.longitude;
                waypoints.push_back(std::move(point));
            }
            response["path"] = std::move(waypoints);
            response["distance"] = routeResult.totalDistance;
            response["estimatedMinutes"] = routeResult.estimatedMinutes;
            response["found"] = true;
            
            // --- Graph Visualization Data (Nodes & Edges) ---
            crow::json::wvalue graphData;
            
            // 1. Get All Nodes
            auto allLocs = routeFinder.getAllLocations();
            crow::json::wvalue::list nodeList;
            for(const auto& name : allLocs) {
                nodeList.push_back(name);
            }
            graphData["nodes"] = std::move(nodeList);
            
            // 2. Get Edges (Simulated or Real)
            // Ideally RouteFinder would expose getAdjacencyList(). 
            // Here we construct edges for visualization: we'll simulate connections 
            // between known locations to represent the graph.
            crow::json::wvalue::list edgeList;
            
            // CONNECTIVITY LOGIC:
            // For now, we create a simple connected structure so the frontend
            // sees a graph. In a real scenario, we'd query the WeightedGraph object.
            for (size_t i = 0; i < allLocs.size(); ++i) {
                // Connect to a few "next" nodes to make a loop/mesh
                for(int j = 1; j <= 3; ++j) {
                    size_t neighborIdx = (i + j * 7) % allLocs.size(); // Pseudo-random neighbor
                    if (neighborIdx != i) {
                        crow::json::wvalue edge;
                        edge["from"] = allLocs[i];
                        edge["to"] = allLocs[neighborIdx];
                        edgeList.push_back(std::move(edge));
                    }
                }
            }
            
            graphData["edges"] = std::move(edgeList);
            response["graph"] = std::move(graphData);
            
        } else {
            response["found"] = false;
            response["message"] = "Route not found";
        }
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });
    
    // GET /api/locations
    CROW_ROUTE(app, "/api/locations")
    ([&routeFinder](){
        auto locations = routeFinder.getAllLocations();
        
        crow::json::wvalue::list locationList;
        for (const auto& loc : locations) {
            crow::json::wvalue location;
            location["name"] = loc;
            locationList.push_back(std::move(location));
        }
        
        crow::json::wvalue response;
        response["locations"] = std::move(locationList);
        
        crow::response res(200, response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // ==================== PUBLIC STATIC ROUTES ====================
    
    CROW_ROUTE(app, "/public/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/" + filename);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.end();
    });

    CROW_ROUTE(app, "/public/Photoes/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/Photoes/" + filename);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.end();
    });

    CROW_ROUTE(app, "/public/css/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/css/" + filename);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.end();
    });

    CROW_ROUTE(app, "/public/js/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/js/" + filename);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.end();
    });

    // Generic static file handler (MUST BE LAST)
    CROW_ROUTE(app, "/<string>")
    ([](const crow::request&, crow::response& res, std::string filename){
        res.set_static_file_info("public/" + filename);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.end();
    });
}
