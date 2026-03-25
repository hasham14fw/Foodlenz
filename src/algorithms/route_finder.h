#pragma once
#include "../database/models.h"

#include "../dsa/stack_queue.h"
#include "../database/idatabase.h"
#include "../dsa/graph.h"
#include "../dsa/dijkstra.h"
#include "../dsa/dynamic_array.h"
#include <string>
#include <cmath>
#include <algorithm>

namespace Algorithms {

struct RouteResult {
    DSA::DynamicArray<Models::Location> waypoints;
    double totalDistance;
    int estimatedMinutes;
    bool found;
    
    RouteResult() : totalDistance(0), estimatedMinutes(0), found(false) {}
};

class RouteFinder {
private:
    Database::IDatabase* db; // Use DB graph
    
public:
    RouteFinder(Database::IDatabase* database) : db(database) {
        // If database is null, we can't do much. 
        // We assume it's passed.
    }
    
    RouteResult calculateRoute(const Models::Location& start, const Models::Location& destination) {
        RouteResult result;
        if (!db) return result;
        
        DSA::Graph& cityGraph = db->getCityGraph();
        
        // Find nearest graph nodes to start and destination
        std::string startNode = findNearestNode(cityGraph, start);
        std::string destNode = findNearestNode(cityGraph, destination);
        
        if (startNode.empty() || destNode.empty()) {
            return result;
        }
        
        // Run Dijkstra's algorithm
        auto pathResult = DSA::Dijkstra::findShortestPath(cityGraph, startNode, destNode);
        
        if (!pathResult.found) {
            return result;
        }
        
        // Convert path to Location waypoints
        for (size_t i = 0; i < pathResult.path.size(); i++) {
            std::string nodeName = pathResult.path[i];
            Models::Location waypoint = getLocationFromNode(nodeName);
            result.waypoints.push_back(waypoint);
        }
        
        result.totalDistance = pathResult.totalDistance;
        result.estimatedMinutes = static_cast<int>(pathResult.totalDistance * 3); // ~20 km/h avg
        result.found = true;
        
        return result;
    }
    
    DSA::DynamicArray<std::string> getAllLocations() {
        if (db) return db->getCityGraph().getAllVertices();
        return DSA::DynamicArray<std::string>();
    }
    
private:
    std::string findNearestNode(DSA::Graph& graph, const Models::Location& location) {
        // Current Graph impl doesn't have lat/lng stored easily without looking up.
        // We cheated in previous impl by string matching "Islamabad", "Lahore".
        // Let's stick to that simple logic for now as getting edge weights correct for all nodes 
        // requires a full spatial index which we don't have.
        
        std::string cityLower = location.name;
        std::transform(cityLower.begin(), cityLower.end(), cityLower.begin(), ::tolower);
        
        // Check exact match first
        if (graph.hasVertex(location.name)) return location.name;
        
        if (cityLower.find("islamabad") != std::string::npos) return "Islamabad Center";
        if (cityLower.find("lahore") != std::string::npos) return "Lahore Center";
        if (cityLower.find("karachi") != std::string::npos) return "Karachi Center";
        
        // Better: search all vertices for substring?
        auto vertices = graph.getAllVertices();
        for(const auto& v : vertices) {
             if (v == location.name) return v;
        }
        
        return "Islamabad Center"; // Default fallback
    }
    
    Models::Location getLocationFromNode(const std::string& nodeName) {
        // Approximate coordinates for key nodes in Pakistan
        // Islamabad
        if (nodeName.find("F-6") != std::string::npos) return Models::Location(nodeName, 33.7297, 73.0766);
        if (nodeName.find("F-7") != std::string::npos) return Models::Location(nodeName, 33.7128, 73.0551);
        if (nodeName.find("Blue Area") != std::string::npos) return Models::Location(nodeName, 33.7077, 73.0503);
        if (nodeName.find("Islamabad") != std::string::npos) return Models::Location(nodeName, 33.6844, 73.0479);
        
        // Lahore
        if (nodeName.find("Gulberg") != std::string::npos) return Models::Location(nodeName, 31.5102, 74.3441);
        if (nodeName.find("DHA") != std::string::npos) return Models::Location(nodeName, 31.4697, 74.4504);
        if (nodeName.find("Lahore") != std::string::npos) return Models::Location(nodeName, 31.5204, 74.3587);
        
        // Karachi
        if (nodeName.find("Clifton") != std::string::npos) return Models::Location(nodeName, 24.8138, 67.0315);
        if (nodeName.find("Saddar") != std::string::npos) return Models::Location(nodeName, 24.8615, 67.0261);
        if (nodeName.find("Karachi") != std::string::npos) return Models::Location(nodeName, 24.8607, 67.0011);
        
        // Default fallbacks to prevent jumps to (0,0)
        std::string lowerNode = nodeName;
        std::transform(lowerNode.begin(), lowerNode.end(), lowerNode.begin(), ::tolower);
        if (lowerNode.find("lahore") != std::string::npos) return Models::Location(nodeName, 31.5204, 74.3587);
        if (lowerNode.find("karachi") != std::string::npos) return Models::Location(nodeName, 24.8607, 67.0011);
        
        return Models::Location(nodeName, 33.6844, 73.0479); // Default to Islamabad
    }

};

} // namespace Algorithms
