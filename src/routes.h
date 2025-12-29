#pragma once
#include "crow_all.h"
#include "database/idatabase.h"
#include "algorithms/recommendation_engine.h"
#include "algorithms/route_finder.h"

void setupRoutes(crow::SimpleApp& app, Database::IDatabase& db, 
                Algorithms::RecommendationEngine& recommender,
                Algorithms::RouteFinder& routeFinder);
