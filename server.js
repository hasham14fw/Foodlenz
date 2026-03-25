const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const dotenv = require('dotenv');
const axios = require('axios');
const csv = require('csv-parser');

const User = require('./models/User');
const Restaurant = require('./models/Restaurant');
const MenuItem = require('./models/MenuItem');
const Review = require('./models/Review');
const Location = require('./models/Location');

dotenv.config();

const app = express();
const path = require('path');
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public'), { index: 'signin.html' }));
app.use('/public', express.static(path.join(__dirname, 'public')));

app.get('/api/config', (req, res) => {
    res.json({ 
        googleMapsApiKey: process.env.GOOGLE_MAPS_API_KEY || 'YOUR_GOOGLE_MAPS_API_KEY_HERE',
        weatherApiKey: process.env.WEATHER_API_KEY || '07b7b0dab1cc1653788e9c1890396e7c'
    });
});

const PORT = process.env.PORT || 5050;

const MONGODB_URI = process.env.MONGO_URI || 'mongodb+srv://hasham:hasham@cluster0.eqit4s1.mongodb.net/Foodlenz?appName=Cluster0';

mongoose.connect(MONGODB_URI)
    .then(() => {
        console.log('Connected to MongoDB Cloud');
        seedDatabase();
    })
    .catch(err => console.error('MongoDB connection error:', err));


const CLOUD_BASE = 'https://raw.githubusercontent.com/hasham14fw/csv_files_DS/refs/heads/main/';

async function downloadAndParseCSV(url) {
    const response = await axios.get(url, { responseType: 'stream' });
    return new Promise((resolve, reject) => {
        const results = [];
        response.data.pipe(csv())
            .on('data', (data) => results.push(data))
            .on('end', () => resolve(results))
            .on('error', reject);
    });
}

async function seedDatabase() {
    try {
        const restCount = await Restaurant.countDocuments();
        if (restCount > 0) {
            console.log('Database already seeded. Skipping seed process.');
            return;
        }

        console.log('Seeding database from GitHub CSVs...');

        const users = await downloadAndParseCSV(CLOUD_BASE + 'users.csv');
        await User.insertMany(users.map(u => ({ ...u })));
        
        const rests = await downloadAndParseCSV(CLOUD_BASE + 'restaurants.csv');
        await Restaurant.insertMany(rests.map(r => ({ ...r, rating: parseFloat(r.rating) || 0, lat: parseFloat(r.lat), lng: parseFloat(r.lng) })));

        const reviewsData = await downloadAndParseCSV(CLOUD_BASE + 'reviews.csv');
        await Review.insertMany(reviewsData.map(r => ({
            ...r,
            tasteRating: parseInt(r.tasteRating) || 0,
            ambianceRating: parseInt(r.ambianceRating) || 0,
            overallRating: parseInt(r.overallRating) || 0,
            timestamp: parseInt(r.timestamp) || Math.floor(Date.now() / 1000)
        })));

        try {
            const menus = await downloadAndParseCSV(CLOUD_BASE + 'menu_items.csv');
            if (menus.length > 0) await MenuItem.insertMany(menus.map(m => ({ ...m, price: parseFloat(m.price) || 0 })));
        } catch (e) {
            console.log('Menu items CSV logic skipped or missing');
        }

        const locations = await downloadAndParseCSV(CLOUD_BASE + 'locations.csv');
        await Location.insertMany(locations.map(l => ({ ...l, lat: parseFloat(l.lat), lng: parseFloat(l.lng) })));

        console.log('Database seeding complete!');
    } catch (err) {
        console.error('Error seeding database:', err);
    }
}

app.post('/api/login', async (req, res) => {
    try {
        const { email, password } = req.body;
        const user = await User.findOne({ email });
        if (!user) return res.status(401).json({ error: 'User not found' });
        
        res.json({ token: 'fake-jwt-token-' + user.id, user: { id: user.id, name: user.name, email: user.email } });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.post('/api/signup', async (req, res) => {
    try {
        const { name, email, password, city } = req.body;
        const exists = await User.findOne({ email });
        if (exists) return res.status(400).json({ error: 'Email already registered' });
        
        const newUser = new User({
            id: 'u_' + Date.now(),
            name, email, passwordHash: 'hash_' + password, city
        });
        await newUser.save();
        
        res.json({ token: 'fake-jwt-token-' + newUser.id, user: { id: newUser.id, name: newUser.name, email: newUser.email } });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/weather', async (req, res) => {
    const city = req.query.city || 'Islamabad';
    try {
        const apiKey = process.env.WEATHER_API_KEY || '07b7b0dab1cc1653788e9c1890396e7c';
        const url = `http://api.openweathermap.org/data/2.5/weather?q=${encodeURIComponent(city)}&appid=${apiKey}&units=metric`;
        const response = await axios.get(url);
        const data = response.data;
        res.json({
            city: data.name,
            temp: Math.round(data.main.temp),
            description: data.weather[0].main,
            sunrise: data.sys.sunrise,
            sunset: data.sys.sunset
        });
    } catch (error) {
        res.json({
            city,
            temp: Math.floor(Math.random() * 15) + 15,
            description: 'Sunny',
            sunrise: Math.floor(Date.now() / 1000) - 3600 * 5,
            sunset: Math.floor(Date.now() / 1000) + 3600 * 5
        });
    }
});

app.get('/api/restaurants/search', async (req, res) => {
    try {
        const { query, cuisine, city, minRating } = req.query;
        let filter = {};
        if (query) filter.name = new RegExp(query, 'i');
        if (cuisine) filter.cuisine = new RegExp(cuisine, 'i');
        if (city && city !== 'All Cities') filter.city = new RegExp(city, 'i');
        if (minRating) filter.rating = { $gte: parseFloat(minRating) };

        const restaurants = await Restaurant.find(filter);
        res.json({ restaurants });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/restaurants/recommended', async (req, res) => {
    try {
        const restaurants = await Restaurant.find().sort({ rating: -1 }).limit(5);
        res.json({ restaurants });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/restaurants/trending', async (req, res) => {
    try {
        const restaurants = await Restaurant.find().sort({ rating: -1 }).limit(6);
        res.json({ restaurants });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/restaurants/:id', async (req, res) => {
    try {
        const restaurant = await Restaurant.findOne({ id: req.params.id });
        if (!restaurant) return res.status(404).json({ error: 'Not found' });
        res.json(restaurant);
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/restaurants/:id/menu', async (req, res) => {
    try {
        const menuItems = await MenuItem.find({ restaurantId: req.params.id });
        res.json({ menuItems });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/restaurants/:id/reviews', async (req, res) => {
    try {
        const reviews = await Review.find({ restaurantId: req.params.id }).sort({ timestamp: -1 });
        const enrichedReviews = [];
        for (let r of reviews) {
            const u = await User.findOne({ id: r.userId });
            enrichedReviews.push({ ...r.toObject(), userName: u ? u.name : 'Anonymous' });
        }
        res.json({ reviews: enrichedReviews });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.post('/api/restaurants/:id/reviews', async (req, res) => {
    try {
        const authHeader = req.headers.authorization || '';
        const token = authHeader.replace('Bearer ', '');
        let userId = 'u001';
        if (token.startsWith('fake-jwt-token-')) {
            userId = token.replace('fake-jwt-token-', '');
        }

        const { tasteRating, ambianceRating, overallRating, comment } = req.body;
        
        const existing = await Review.findOne({ userId, restaurantId: req.params.id });
        if (existing) {
             return res.status(400).json({ error: 'You have already reviewed this restaurant.' });
        }

        const newReview = new Review({
            id: 'rev_' + Date.now(),
            userId,
            restaurantId: req.params.id,
            tasteRating, ambianceRating, overallRating, comment,
            timestamp: Math.floor(Date.now() / 1000)
        });

        await newReview.save();
        res.json({ success: true, review: newReview });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.get('/api/locations', async (req, res) => {
    try {
        const locations = await Location.find();
        res.json({ locations });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.post('/api/route', async (req, res) => {
    try {
        const { startLocation, restaurantId, startLat, startLng } = req.body;
        
        const rest = await Restaurant.findOne({ id: restaurantId });
        let start = null;
        
        if (startLat && startLng) {
            start = { name: "Pinned Location", lat: startLat, lng: startLng };
        } else {
            start = await Location.findOne({ name: new RegExp(startLocation, 'i') });
        }
        
        if (!rest || !start) {
            return res.json({ found: false });
        }
        
        const R = 6371;
        const dLat = (rest.lat - start.lat) * Math.PI / 180;
        const dLon = (rest.lng - start.lng) * Math.PI / 180;
        const lat1 = start.lat * Math.PI / 180;
        const lat2 = rest.lat * Math.PI / 180;

        const a = Math.sin(dLat/2)*Math.sin(dLat/2) + Math.sin(dLon/2)*Math.sin(dLon/2) * Math.cos(lat1)*Math.cos(lat2);
        const distance = R * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
        
        const path = [
            { name: start.name, lat: start.lat, lng: start.lng },
            { name: "City Path", lat: (start.lat + rest.lat)/2, lng: (start.lng + rest.lng)/2 },
            { name: rest.name, lat: rest.lat, lng: rest.lng }
        ];

        res.json({
            found: true,
            path: path,
            distance: distance,
            estimatedMinutes: Math.round(distance * 3),
            graph: null
        });
    } catch (error) {
        res.status(500).json({ error: 'Server error' });
    }
});

app.post('/api/chatbot', (req, res) => {
    const { message } = req.body;
    let reply = "I am FoodLens AI. How can I help you find a restaurant?";
    if (message.toLowerCase().includes('pizza') || message.toLowerCase().includes('italian')) {
        reply = "I recommend La Bella Italia in Islamabad for great Italian food!";
    } else if (message.toLowerCase().includes('lahore')) {
        reply = "Spice Garden in Lahore has an excellent rating of 4.5. You should try it!";
    }
    setTimeout(() => {
        res.json({ response: reply });
    }, 1000);
});

app.listen(PORT, () => {
    console.log(`Node.js Backend Server running on port ${PORT}`);
});
