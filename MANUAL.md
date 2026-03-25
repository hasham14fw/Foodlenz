# FoodLenz User and Developer Manual

## User Manual

### 1. Registration and Authentication
- Visit the registration page to create an account.
- Input your name, email, password, and city.
- The system uses a hash table to ensure instant registration and lookup.
- Log in using your email and password to access personalized features.

### 2. Finding Restaurants
- Use the search bar on the Home page or Search page.
- Search by restaurant name for quick lookup via Binary Search Tree.
- Use the filters to narrow results by cuisine (e.g., Italian, Chinese, Pakistani).
- The filters utilize a self-balancing AVL Tree to ensure rapid processing of large restaurant datasets.

### 3. Personalized Experience
- The Home page features a Trending section showing restaurants with the highest aggregate ratings using a Max Heap algorithm.
- The Recommendations sub-section analyzes your past reviews and dining history to suggest restaurants that match your profile using content-based filtering.

### 4. Interactive Chatbot
- Navigate to the Chatbot page for natural language assistance.
- You can ask the AI questions like "Where can I find pizza in Lahore?" or "What's the best Pakistani food around?"
- The bot employs a Trie-based keyword extraction and a State Machine to guide you through the selection process.

### 5. Routing and Distance
- The Map Route page allows you to calculate the shortest path from any city center to a chosen restaurant.
- This feature is powered by Dijkstra's algorithm running on a weighted graph of the city's key points.

## Developer Manual

### System Architecture
The application is a multi-tier platform combining Node.js for external web APIs and C++ for internal data-intensive algorithms.

- Entry Point: `server.js` starts the Node.js server.
- Algorithm Layer: Compiled from the `src/` directory.
- Database: Managed through MongoDB and synchronized with GitHub CSV repositories.

### Building the Project
1. Compile the C++ modules using the CMake build system.
2. Ensure you have the `crow_all.h` header in the include path.
3. Configure the environment variables in the `.env` file (MONGO_URI, GITHUB_TOKEN).

### Data Synchronization
- To update the seed data, modify the CSV files in the GitHub repository mentioned in the configurations.
- Any new reviews or registrations are automatically pushed to the cloud-based CSV store via the C++ backend.

### Technical Support
Consult the `Backend_Documentation.md` for detailed mathematical and algorithmic implementations of the data structure used in the project.
