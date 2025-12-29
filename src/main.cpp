#define _USE_MATH_DEFINES
#include "crow_all.h"
#include <iostream>
#include "database/idatabase.h"
#include "database/db_factory.h"
#include "algorithms/recommendation_engine.h"
#include "algorithms/route_finder.h"
#include "routes.h"

int main() {
    std::cout << "Starting Backend Server V4 - CSV CLOUD MODE" << std::endl;
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);
    
    // Initialize Database
    std::unique_ptr<Database::IDatabase> db = Database::createDatabase();
    
    // Initialize Engines
    Algorithms::RecommendationEngine recommender(db.get());
    Algorithms::RouteFinder routeFinder(db.get());
    
    std::cout << "=== Restaurant Finder API Server ===" << std::endl;
    std::cout << "Database initialized." << std::endl;
    
    setupRoutes(app, *db, recommender, routeFinder);
    
    std::cout << "Server starting on port 5050..." << std::endl;
    app.port(5050).multithreaded().run();
}
