import requests
import time
import sys
import json

BASE_URL = "http://localhost:3000"

def log(msg):
    print(f"[TEST] {msg}")

def test_fetch_restaurants():
    log("Fetching trending restaurants (verifies Cloud Read)...")
    try:
        resp = requests.get(f"{BASE_URL}/api/restaurants/trending")
        if resp.status_code == 200:
            data = resp.json()
            restaurants = data.get("restaurants", [])
            log(f"Success! Found {len(restaurants)} restaurants.")
            if len(restaurants) > 0:
                log(f"Sample: {restaurants[0]['name']} ({restaurants[0]['cuisine']})")
            return True
        else:
            log(f"Failed. Status: {resp.status_code}, Body: {resp.text}")
            return False
    except Exception as e:
        log(f"Exception: {e}")
        return False

def test_signup_login():
    log("Testing Signup and Login (verifies Local Write/Memory)...")
    user_email = f"testuser_{int(time.time())}@example.com"
    payload = {
        "name": "Test User",
        "email": user_email,
        "password": "password123",
        "city": "Islamabad"
    }
    
    # Signup
    try:
        log(f"Signing up {user_email}...")
        resp = requests.post(f"{BASE_URL}/api/signup", json=payload)
        if resp.status_code != 200:
            log(f"Signup Failed: {resp.text}")
            return False
        
        data = resp.json()
        token = data.get("token")
        log("Signup Successful. Token received.")
        
        # Login
        log("Logging in...")
        login_payload = {
            "email": user_email,
            "password": "password123"
        }
        resp = requests.post(f"{BASE_URL}/api/login", json=login_payload)
        if resp.status_code == 200:
            log("Login Successful.")
            return token
        else:
            log(f"Login Failed: {resp.text}")
            return None
            
    except Exception as e:
        log(f"Exception: {e}")
        return None

def test_create_review(token, user_id):
    log("Testing Review Creation (verifies Cloud Write)...")
    # Use the first restaurant found
    try:
        resp = requests.get(f"{BASE_URL}/api/restaurants/trending")
        if resp.status_code != 200: return False
        restaurants = resp.json().get("restaurants", [])
        if not restaurants: return False
        
        rest_id = restaurants[0]["id"]
        
        payload = {
            "tasteRating": 5,
            "ambianceRating": 4,
            "overallRating": 5,
            "comment": f"Amazing food! Automated test review {int(time.time())}"
        }
        
        headers = {"Authorization": f"Bearer {token}"}
        log(f"Submitting review for {restaurants[0]['name']}...")
        
        resp = requests.post(f"{BASE_URL}/api/restaurants/{rest_id}/reviews", json=payload, headers=headers)
        if resp.status_code == 201:
            log("Review Created Successfully! Check server logs for 'Successfully pushed to GitHub'.")
            return True
        elif resp.status_code == 409:
             log("User already reviewed this restaurant (Expected if run multiple times via same user).")
             return True
        else:
            log(f"Review Creation Failed: {resp.text}")
            return False
            
    except Exception as e:
        log(f"Exception: {e}")
        return False

def main():
    log("Waiting for server to be ready...")
    time.sleep(2) # Give server a moment if just started
    
    if not test_fetch_restaurants():
        sys.exit(1)
        
    token = test_signup_login()
    if not token:
        sys.exit(1)
        
    if not test_create_review(token, "userId_placeholder"):
        sys.exit(1)
        
    log("All verification tests passed!")

if __name__ == "__main__":
    main()
