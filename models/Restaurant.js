const mongoose = require('mongoose');

const RestaurantSchema = new mongoose.Schema({
    id: String,
    name: String,
    city: String,
    category: String,
    cuisine: String,
    rating: Number,
    logoURL: String,
    lat: Number,
    lng: Number
});

module.exports = mongoose.model('Restaurant', RestaurantSchema);
