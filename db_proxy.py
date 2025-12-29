from flask import Flask, request, jsonify
from pymongo import MongoClient
import os
import json
from bson import json_util

app = Flask(__name__)

# Connection string from user (hardcoded for simplicity in proxy, or read from env)
MONGO_URI = "mongodb+srv://foodlenz:foodlenz@cluster0.eqit4s1.mongodb.net/?appName=Cluster0"
DB_NAME = "foodlenz"

client = MongoClient(MONGO_URI)
db = client[DB_NAME]

def parse_json(data):
    return json.loads(json_util.dumps(data))

@app.route('/action/findOne', methods=['POST'])
def find_one():
    try:
        data = request.json
        collection_name = data['collection']
        filter_doc = data.get('filter', {})
        
        # Handle simple replacements for ID if needed, but for now pass through
        # Note: Data API uses string IDs, Pymongo might use ObjectId. 
        # We rely on our C++ code sending string IDs which we stored as strings in DB? 
        # If we stored as ObjectIds, we might need conversion.
        # Based on previous mock/code, we treat IDs as strings.
        
        result = db[collection_name].find_one(filter_doc)
        if result:
            return jsonify({"document": parse_json(result)})
        else:
            return jsonify({"document": None})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/action/find', methods=['POST'])
def find():
    try:
        data = request.json
        collection_name = data['collection']
        filter_doc = data.get('filter', {})
        sort_doc = data.get('sort', None)
        limit_val = data.get('limit', 0)
        
        cursor = db[collection_name].find(filter_doc)
        
        if sort_doc:
            # Pymongo sort expects list of tuples, Data API sends dict
            # e.g. {"rating": -1} -> [("rating", -1)]
            sort_list = []
            for k, v in sort_doc.items():
                sort_list.append((k, int(v)))
            cursor = cursor.sort(sort_list)
            
        if limit_val > 0:
            cursor = cursor.limit(limit_val)
            
        documents = list(cursor)
        return jsonify({"documents": parse_json(documents)})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/action/insertOne', methods=['POST'])
def insert_one():
    try:
        data = request.json
        collection_name = data['collection']
        document = data['document']
        
        result = db[collection_name].insert_one(document)
        return jsonify({"insertedId": str(result.inserted_id)})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/action/insertMany', methods=['POST'])
def insert_many():
    try:
        data = request.json
        collection_name = data['collection']
        documents = data['documents']
        
        result = db[collection_name].insert_many(documents)
        return jsonify({"insertedIds": [str(id) for id in result.inserted_ids]})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    print("Starting DB Proxy on port 8080...")
    app.run(port=8080, debug=True)
