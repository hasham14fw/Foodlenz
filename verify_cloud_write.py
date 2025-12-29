import requests
import json
import base64
import time
import os
from dotenv import load_dotenv

# Load env to get token
load_dotenv()
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
REPO_OWNER = "hasham14fw"
REPO_NAME = "csv_files_DS"
BRANCH = "main"

BASE_URL = "http://localhost:3000"

def log(msg):
    print(f"[VERIFY] {msg}")

def get_file_content(filename):
    url = f"https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/contents/{filename}?ref={BRANCH}"
    headers = {"Authorization": f"token {GITHUB_TOKEN}"}
    resp = requests.get(url, headers=headers)
    if resp.status_code == 200:
        content = base64.b64decode(resp.json()["content"]).decode("utf-8")
        return content
    else:
        log(f"Failed to fetch {filename} from GitHub: {resp.status_code}")
        return None

def test_signup_and_verify():
    # 1. Signup via Backend
    user_email = f"verified_user_{int(time.time())}@example.com"
    log(f"Signing up {user_email} via Backend...")
    
    payload = {
        "name": "Verified User",
        "email": user_email,
        "password": "password123",
        "city": "Islamabad"
    }
    
    try:
        resp = requests.post(f"{BASE_URL}/api/signup", json=payload)
        if resp.status_code != 200:
            log(f"Backend Signup Failed: {resp.text}")
            return False
        
        log("Backend returned Success. Checking GitHub...")
        
        # 2. Check GitHub
        time.sleep(2) # Give it a second to propagate if needed (though API is usually immediate)
        content = get_file_content("users.csv")
        if content and user_email in content:
            log("SUCCESS: User found in GitHub CSV file!")
            return True
        else:
            log("FAILURE: User NOT found in GitHub CSV file.")
            log(f"Current Content Length: {len(content) if content else 0}")
            return False
            
    except Exception as e:
        log(f"Exception: {e}")
        return False

if __name__ == "__main__":
    if not GITHUB_TOKEN:
        log("Error: GITHUB_TOKEN not found in .env")
        exit(1)
        
    log(f"Using Token: {GITHUB_TOKEN[:4]}...{GITHUB_TOKEN[-4:]}")
    log("Waiting 5s for server to start...")
    time.sleep(5)
    test_signup_and_verify()
