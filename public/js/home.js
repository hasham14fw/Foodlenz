// Home page JavaScript

checkAuth();

// Load trending restaurants
loadTrendingRestaurants();
loadRecommendedRestaurants();

// Load trending restaurants from API
async function loadTrendingRestaurants() {
    const container = document.getElementById('trending-restaurants');
    try {
        const data = await apiRequest('/api/restaurants/trending');
        if (data && data.restaurants && data.restaurants.length > 0) {
            displayToGrid(data.restaurants.slice(0, 3), container);
        } else {
            container.innerHTML = '<p style="text-align: center; padding: 20px; color: var(--text-dim);">No trending restaurants found.</p>';
        }
    } catch (error) {
        console.error('Failed to load trending restaurants:', error);
        container.innerHTML = '<p style="text-align: center; padding: 20px; color: var(--primary);">Failed to load trends.</p>';
    }
}

// Load recommended restaurants
async function loadRecommendedRestaurants() {
    const container = document.getElementById('recommended-restaurants');
    try {
        const data = await apiRequest('/api/restaurants/recommended');
        if (data && data.restaurants && data.restaurants.length > 0) {
            displayToGrid(data.restaurants.slice(0, 3), container);
        } else {
            container.innerHTML = '<p style="text-align: center; padding: 20px; color: var(--text-dim);">Analyzing your preferences for deep picks...</p>';
        }
    } catch (error) {
        console.error('Failed to load recommended restaurants:', error);
    }
}

function displayToGrid(restaurants, container) {
    container.innerHTML = '';
    restaurants.forEach(restaurant => {
        const card = document.createElement('div');
        card.className = 'restaurant-card animate-in';
        card.onclick = () => goToRestaurant(restaurant.id);
        
        card.innerHTML = `
            <img src="${restaurant.logoURL}" alt="${restaurant.name}" 
                 onerror="this.src='https://via.placeholder.com/200x150/4ECDC4/FFFFFF?text=Restaurant'">
            <div class="card-content">
                 <h3>${restaurant.name}</h3>
                <p class="cuisine">
                    <span class="icon icon-map" style="color: var(--accent); margin-right: 5px; width: 14px; height: 14px;"></span>
                    ${restaurant.cuisine} • ${restaurant.city}
                </p>
                <p class="rating">
                    <span class="icon icon-star" style="color: #ffd700; margin-right: 5px; width: 14px; height: 14px;"></span>
                    ${restaurant.rating.toFixed(1)}
                </p>
                <button class="explore-btn">Explore Selection →</button>
            </div>
        `;
        
        container.appendChild(card);
    });
}
