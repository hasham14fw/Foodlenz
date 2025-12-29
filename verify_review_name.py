import requests
import json
import base64
import time
from dotenv import load_dotenv
import os

load_dotenv()
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
REPO_OWNER = "hasham14fw"
REPO_NAME = "csv_files_DS"
BRANCH = "main"
BASE_URL = "http://localhost:3000"

def get_file_content(filename):
    url = f"https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/contents/{filename}?ref={BRANCH}"
    headers = {"Authorization": f"token {GITHUB_TOKEN}"}
    resp = requests.get(url, headers=headers)
    if resp.status_code == 200:
        return base64.b64decode(resp.json()["content"]).decode("utf-8")
    return None

def verify():
    # 1. Signup
    timestamp = int(time.time())
    user_name = f"Reviewer_{timestamp}"
    user_email = f"reviewer_{timestamp}@example.com"
    
    print(f"Signing up {user_name}...")
    signup_resp = requests.post(f"{BASE_URL}/api/signup", json={
        "name": user_name,
        "email": user_email,
        "password": "password",
        "city": "Islamabad"
    })
    
    if signup_resp.status_code != 200:
        print("Signup failed")
        return
        
    # Get ID from signup response or fetch user list? 
    # Signup response returns {id, name, ...} usually or just success.
    # Looking at csv_database.cpp, createUser returns the User object.
    # Crow route sends the User object back as JSON.
    user_data = signup_resp.json()
    user_id = user_data.get("id")
    print(f"User ID: {user_id}")
    
    # 2. Add Review
    print("Adding review...")
    review_resp = requests.post(f"{BASE_URL}/api/reviews", json={
        "userId": user_id,
        "restaurantId": "r001", # Assuming r001 exists
        "tasteRating": 5,
        "ambianceRating": 5,
        "overallRating": 5,
        "comment": "Verified Review Name"
    })
    
    if review_resp.status_code != 200:
        print(f"Review failed: {review_resp.text}")
        return

    print("Review added. Checking GitHub...")
    time.sleep(2)
    
    content = get_file_content("reviews.csv")
    if content:
        # Check if user_name is in content
        if user_name in content:
            print("SUCCESS: User Name found in reviews.csv")
            # Also check the specific line structure if possible, but presence is good enough for now
        else:
            print("FAILURE: User Name NOT found in reviews.csv")
            print("Content snippet:")
            print(content[-200:])
    else:
        print("Failed to fetch reviews.csv")

if __name__ == "__main__":
    time.sleep(5) # Wait for server
    verify()
