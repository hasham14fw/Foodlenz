const mongoose = require('mongoose');

const MenuItemSchema = new mongoose.Schema({
    id: String,
    restaurantId: String,
    itemName: String,
    price: Number,
    imageURL: String
});

module.exports = mongoose.model('MenuItem', MenuItemSchema);
