const mongoose = require('mongoose');

const UserSchema = new mongoose.Schema({
    id: String,
    name: String,
    email: { type: String, unique: true },
    passwordHash: String,
    city: String
});

module.exports = mongoose.model('User', UserSchema);
