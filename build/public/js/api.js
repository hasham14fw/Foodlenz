// Common API utilities and authentication helpers

const API_BASE = 'http://localhost:3001';

// Authentication functions
function getAuthToken() {
    return localStorage.getItem('authToken');
}

function setAuthToken(token) {
    localStorage.setItem('authToken', token);
}

function removeAuthToken() {
    localStorage.removeItem('authToken');
    localStorage.removeItem('user');
}

function getUser() {
    const userStr = localStorage.getItem('user');
    return userStr ? JSON.parse(userStr) : null;
}

function setUser(user) {
    localStorage.setItem('user', JSON.stringify(user));
}

// API request wrapper with authentication
async function apiRequest(endpoint, options = {}) {
    const url = `${API_BASE}${endpoint}`;
    const token = getAuthToken();
    
    const headers = {
        'Content-Type': 'application/json',
        ...options.headers
    };
    
    if (token) {
        headers['Authorization'] = `Bearer ${token}`;
    }
    
    try {
        const response = await fetch(url, {
            ...options,
            headers
        });
        
        if (!response.ok) {
            if (response.status === 401) {
                // Unauthorized - redirect to login
                logout();
                return null;
            }
            const error = await response.json();
            throw new Error(error.error || 'Request failed');
        }
        
        return await response.json();
    } catch (error) {
        console.error('API Request failed:', error);
        throw error;
    }
}

// Logout function
function logout() {
    removeAuthToken();
    window.location.href = 'signin.html';
}

// Check authentication
function checkAuth() {
    const token = getAuthToken();
    const user = getUser();
    
    if (!token || !user) {
        // Not authenticated
        if (!window.location.pathname.includes('signin.html') && 
            !window.location.pathname.includes('signup.html')) {
            window.location.href = 'signin.html';
        }
        return false;
    }
    return true;
}

// Format date
function formatDate(timestamp) {
    const date = new Date(timestamp * 1000);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString();
}

// Show loading spinner
function showLoading(elementId) {
    const elem = document.getElementById(elementId);
    if (elem) {
        elem.innerHTML = '<div class="loading-spinner">Loading...</div>';
    }
}

// Show error message
function showError(elementId, message) {
    const elem = document.getElementById(elementId);
    if (elem) {
        elem.innerHTML = `<div class="error-message">${message}</div>`;
    }
}

// Create restaurant card HTML
function createRestaurantCard(restaurant) {
    return `
        <div class="restaurant-card" onclick="goToRestaurant('${restaurant.id}')">
            <img src="${restaurant.logoURL}" alt="${restaurant.name}" onerror="this.src='https://via.placeholder.com/200x150/4ECDC4/FFFFFF?text=Restaurant'">
            <div class="card-content">
                <h3>${restaurant.name}</h3>
                <p class="cuisine">${restaurant.cuisine} • ${restaurant.city}</p>
                <p class="rating"><span class="star-icon"></span> ${restaurant.rating.toFixed(1) || 'N/A'}</p>
                <p class="category">${restaurant.category}</p>
                <button class="explore-btn">Explore</button>
            </div>
        </div>
    `;
}

function goToRestaurant(id) {
    window.location.href = `restaurant.html?id=${id}`;
}

// Get URL parameter
function getURLParameter(name) {
    const urlParams = new URLSearchParams(window.location.search);
    return urlParams.get(name);
}
