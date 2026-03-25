# FoodLenz

FoodLenz is a high-performance restaurant discovery and routing application. It utilizes a hybrid architecture combining a Node.js web server with a C++ backend powered by custom data structures and algorithms.

## Overview

The application allows users to search for restaurants, get personalized recommendations based on their preferences, find the shortest routes to dining locations, and interact with an AI-powered chatbot. 

The project is designed as a demonstration of advanced Data Structures and Algorithms (DSA) including Tries, Graphs, Heaps, and Balanced Trees.

## Features

- User Authentication: Secure login and signup system using Hash Tables.
- Advanced Search: Search by name, cuisine, and rating using BST and AVL Trees.
- Smart Recommendations: Personalized suggestions based on user history and ratings.
- Map and Routing: Real-time shortest path calculation using Dijkstra's algorithm on a weighted graph.
- Interactive Chatbot: Natural language processing for restaurant queries using Tries.
- Trending Section: Instant access to high-rated establishments using Max Heaps.

## Technology Stack

- Frontend: HTML5, CSS3, Vanilla JavaScript.
- Web Backend: Node.js, Express.js.
- Algorithm Backend: C++17, Crow (Web Framework).
- Database: MongoDB (via Mongoose) and GitHub-hosted CSV files.
- Build System: CMake.

## Getting Started

### Prerequisites

- Node.js and npm.
- C++ Compiler (GCC, Clang, or MSVC) with C++17 support.
- CMake (for building the C++ backend).
- MongoDB connection string (set in .env).

### Installation

1. Install Node.js dependencies:
   ```bash
   npm install
   ```

2. Build the C++ backend:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

3. Configure Environment:
   Create a .env file based on the provided template with your MongoDB URI and API keys.

4. Run the application:
   Execute the start_project.bat file or run:
   ```bash
   node server.js
   ```

## Project Structure

- models/: Database schemas for user and restaurant data.
- public/: Frontend assets (HTML, CSS, JS).
- src/: C++ source code.
  - dsa/: Custom data structures (AVL Tree, Graph, Trie, etc.).
  - algorithms/: Specialized algorithms (Route Finder, Recommendation Engine).
  - database/: C++ database connectors for CSV and Cloud storage.
- CMakeLists.txt: Build configuration for the C++ backend.
- server.js: Entry point for the Node.js application.

## License

This project is developed for educational purposes as part of a Data Structures and Algorithms curriculum.
