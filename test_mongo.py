import os
from pymongo import MongoClient
from pymongo.server_api import ServerApi

# URI provided by user
uri = "mongodb+srv://foodlenz:foodlenz@cluster0.eqit4s1.mongodb.net/?appName=Cluster0"

# Create a new client and connect to the server
client = MongoClient(uri, server_api=ServerApi('1'))

try:
    client.admin.command('ping')
    print("Pinged your deployment. You successfully connected to MongoDB!")
    
    print("\nAvailable Databases:")
    for db in client.list_database_names():
        print(f" - {db}")
        
except Exception as e:
    print(f"Connection failed: {e}")
