import requests
import json

API_URL = "http://localhost:8080/action/insertMany"

sample_restaurants = [
    {
        "id": "r001", 
        "name": "La Bella Italia", 
        "city": "Islamabad", 
        "category": "Fine Dining", 
        "cuisine": "Italian", 
        "rating": 4.7, 
        "logoURL": "https://img.freepik.com/free-photo/italian-food-background_1220-4318.jpg", 
        "latitude": 33.6844, 
        "longitude": 73.0479
    },
    {
        "id": "r002", 
        "name": "Sushi World", 
        "city": "Islamabad", 
        "category": "Fine Dining", 
        "Japanese": 4.8,
        "cuisine": "Japanese",
        "rating": 4.8,
        "logoURL": "https://img.freepik.com/free-photo/sushi-set-table_140725-5024.jpg", 
        "latitude": 33.7077, 
        "longitude": 73.0478
    },
    {
        "id": "r003", 
        "name": "Spice Garden", 
        "city": "Lahore", 
        "category": "Casual Dining", 
        "cuisine": "Pakistani", 
        "rating": 4.5, 
        "logoURL": "https://img.freepik.com/free-photo/chicken-karahi-pakistani-indian-food_1205-11026.jpg", 
        "latitude": 31.5204, 
        "longitude": 74.3587
    },
    {
        "id": "r004", 
        "name": "Burger Lab", 
        "city": "Karachi", 
        "category": "Fast Food", 
        "cuisine": "American", 
        "rating": 4.4, 
        "logoURL": "https://img.freepik.com/free-photo/front-view-burger-stand_141793-15542.jpg", 
        "latitude": 24.8607, 
        "longitude": 67.0011
    },
    {
        "id": "r005", 
        "name": "Tandoori Nights", 
        "city": "Islamabad", 
        "category": "Casual Dining", 
        "cuisine": "Pakistani", 
        "rating": 4.6, 
        "logoURL": "https://img.freepik.com/free-photo/chicken-tikka-skewers_1220-4493.jpg", 
        "latitude": 33.69, 
        "longitude": 73.05
    },
    {
        "id": "r006", 
        "name": "Wok This Way", 
        "city": "Lahore", 
        "category": "Fast Food", 
        "cuisine": "Chinese", 
        "rating": 4.3, 
        "logoURL": "https://img.freepik.com/free-photo/chinese-food-table_140725-2426.jpg", 
        "latitude": 31.55, 
        "longitude": 74.33
    }
]

payload = {
    "collection": "restaurants",
    "documents": sample_restaurants
}

try:
    print("Seeding data...")
    response = requests.post(API_URL, json=payload)
    if response.status_code == 200:
        print("Successfully seeded data!")
        print(response.json())
    else:
        print(f"Failed to seed data: {response.status_code}")
        print(response.text)
except Exception as e:
    print(f"Error: {e}")
