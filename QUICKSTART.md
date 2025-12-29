# Quick Start Guide - FOODLENZ Restaurant Finder

## Problem: "login.html not found"

This happens because the server needs the `public` folder in the build directory.

## Quick Fix (Run these commands):

### Option 1: Copy Public Folder Manually

```batch
# From project root directory
robocopy public build\public /E
```

### Option 2: Use the provided script

```batch
copy_public.bat
```

### Option 3: Run from Project Root

Instead of running from build folder, run from project root:

```batch
# Don't do: cd build && server.exe
# Instead, run from project root:
build\server.exe
```

## Full Setup Steps

1. **Copy public files** (choose one method above)

2. **Start the server**:
   ```batch
   cd "d:\Project DS Final"
   build\server.exe
   ```
3. **Open browser**:
   - Go to: `http://localhost:3000/login.html`
   - Or: `http://localhost:3000/signup.html`

## Verify Server is Running

You should see:

```
=== Restaurant Finder API Server ===
Database initialized with 15 restaurants
Server starting on port 3000...
```

## File Structure Check

Make sure you have:

```
d:\Project DS Final\
├── build\
│   ├── server.exe  (your compiled server)
│   └── public\     (MUST BE HERE!)
│       ├── index.html
│       ├── login.html
│       ├── signup.html
│       ├── search.html
│       ├── restaurant.html
│       ├── chatbot.html
│       ├── route.html
│       ├── css\
│       └── js\
└── public\         (original files)
```

## Test the Pages

1. **Login**: http://localhost:3000/login.html
2. **Signup**: http://localhost:3000/signup.html
3. **Home**: http://localhost:3000/index.html (after login)
4. **Search**: http://localhost:3000/search.html
5. **Chatbot**: http://localhost:3000/chatbot.html
6. **Route**: http://localhost:3000/route.html

## If Still Not Working

Check:

1. Server is running (console shows port 3000)
2. No other program using port 3000
3. File `build\public\login.html` exists
4. Typing correct URL with `/` before filename

## Test Login Credentials

Create new account at signup, or use:

- Email: alice@example.com
- Password: alice123 (hash_alice123 in system)
