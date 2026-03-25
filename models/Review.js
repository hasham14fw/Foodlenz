const mongoose = require('mongoose');

const ReviewSchema = new mongoose.Schema({
    id: String,
    userId: String,
    restaurantId: String,
    tasteRating: Number,
    ambianceRating: Number,
    overallRating: Number,
    comment: String,
    timestamp: Number
});

module.exports = mongoose.model('Review', ReviewSchema);
