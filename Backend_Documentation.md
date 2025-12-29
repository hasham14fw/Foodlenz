# FoodLens AI - Backend & DSA Documentation

This document explains the technical architecture, algorithms, and Data Structures and Algorithms (DSA) concepts used in the FoodLens AI backend.

---

## 1. Login & Signup System

**Goal:** Securely manage user accounts and quick access.

- **DSA Used:** `CustomHashTable` (Hash Table)
- **Why:**
  - **Signup:** We need to check if an email already exists. Searching a list of 1000 users one-by-one is slow ($O(N)$). A Hash Table allows us to check existence instantly ($O(1)$).
  - **Login:** We need to retrieve user details (like password hash) using their email. Hash Maps provide instant lookups.
- **Key Files:** `src/database/csv_database.h`, `src/routes.cpp`

---

## 2. Chatbot Logic

**Goal:** Understand user natural language (e.g., "I want Italian food in Islamabad") and manage conversation flow.

- **DSA Concept 1: Trie (Prefix Tree)**

  - **Used For:** **Keyword Extraction**.
  - **How:** All known keywords (Cuisines, Cities, Greetings) are stored in a Trie. When a user sends a message, the system scans it against the Trie.
  - **Benefit:** Checks the message against thousands of keywords in a single pass ($O(L)$ time, where $L$ is message length), making it extremely fast compared to looping through lists.
  - **File:** `src/dsa/trie.h`

- **DSA Concept 2: Finite State Machine (FSA)**

  - **Used For:** **Conversation Flow**.
  - **How:** The bot uses "States" (`GREETING` $\to$ `ASK_CUISINE` $\to$ `ASK_CITY` $\to$ `COMPLETED`).
  - **Benefit:** Ensures the bot remembers context. If it asked for a city, it knows the next input is likely a city name.
  - **File:** `src/routes.cpp`

- **DSA Concept 3: Dynamic Array / Vector**
  - **Used For:** **Session Management**.
  - **How:** Stores active user sessions in a list to track history for multiple users.

---

## 3. Search System (Restaurants)

**Goal:** Allow users to filter restaurants by Name, Cuisine, or Rating.

- **DSA Concept 1: Binary Search Tree (BST)**

  - **Used For:** **Search by Name** (Prefix Search).
  - **How:** Restaurants are organized alphabetically in a tree.
  - **Benefit:** Allows fast "starts with" searching (e.g., typing "McD" efficiently finds "McDonalds").
  - **File:** `src/dsa/bst.h`

- **DSA Concept 2: AVL Tree (Self-Balancing Tree)**
  - **Used For:** **Filter by Cuisine**.
  - **How:** Each node represents a Cuisine Category (e.g., "Italian") and contains a list of restaurants.
  - **Why AVL?:** Cuisines often come in sorted clusters. A normal BST would become skewed (slow). AVL guarantees almost perfect balance ($O(\log N)$) for fast category lookups.
  - **File:** `src/dsa/avl_tree.h`

---

## 4. Map & Routing System

**Goal:** Calculate the shortest path and travel time between two locations.

- **DSA Concept 1: Weighted Graph**

  - **Used For:** **Map Representation**.
  - **structure:**
    - **Nodes (Vertices):** Locations (City Centers, Restaurants).
    - **Edges:** Roads.
    - **Weights:** Distances (km).
  - **File:** `src/dsa/graph.h`

- **DSA Concept 2: Dijkstra's Algorithm**
  - **Used For:** **Shortest Path Calculation**.
  - **How:** It explores the Graph starting from the user's location, always expanding the "cheapest" (shortest) path first, until it reaches the destination.
  - **Time Calculation:** `Time = Distance * 3` (Assumption: 3 mins to travel 1 km).
  - **File:** `src/algorithms/route_finder.h`

---

## 5. Trending vs. Recommended

**Goal:** Show popular content (Trends) vs Personalized content (Recommendations).

### A. Trending Restaurants (Global)

- **DSA Used:** **Max Heap**
- **Implementation:** `DynamicArray` (Array-based Heap).
- **Why:** A Max Heap keeps the highest-rated item at the root. Retrieving the "Top 6" is instant and efficient ($O(K \log N)$). Array implementation allows $O(1)$ mathematical traversal (Parent/Child indices).

### B. Recommended Restaurants (Personalized)

- **Algorithm:** **Content-Based Filtering**.
- **How:**
  1.  Analyzes your **Past Reviews**.
  2.  Calculates a **Score** for every restaurant based on:
      - Cuisine Match (40%)
      - Rating (30%)
      - Category Match (20%)
      - Diversity (10%)
  3.  **Sorts** the list by Score.

---

## 6. DSA Summary Table

| Feature                | Data Structure               | Reason                                    |
| :--------------------- | :--------------------------- | :---------------------------------------- |
| **Login / Signup**     | **Hash Table**               | $O(1)$ fast lookup for users/email.       |
| **Search (Name)**      | **Binary Search Tree (BST)** | Efficient prefix matching.                |
| **Search (Cuisine)**   | **AVL Tree**                 | Handles skewed category data efficiently. |
| **Trending**           | **Max Heap (Array)**         | access to highest-rated items.            |
| **Chatbot (Keywords)** | **Trie**                     | Instant keyword detection in text.        |
| **Chatbot (Flow)**     | **State Machine**            | Logical conversation progression.         |
| **Maps**               | **Graph + Dijkstra**         | Standard mathematical model for routing.  |
| **Sliders/History**    | **Linked Lists**             | Circular for sliders, Doubly for history. |
