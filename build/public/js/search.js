// Search page JavaScript

checkAuth();

// Load initial data
loadAllRestaurants();
loadRecommendedRestaurants();

// Search functionality
function performSearch() {
    const query = document.getElementById('search-input').value;
    const cuisine = document.getElementById('cuisine-filter').value;
    const city = document.getElementById('city-filter').value;
    const rating = document.getElementById('rating-filter').value;
    
    searchRestaurants(query, cuisine, city, rating);
}

async function searchRestaurants(query = '', cuisine = '', city = '', rating = '0') {
    const resultsGrid = document.getElementById('results-grid');
    const heading = document.getElementById('results-heading');
    
    try {
        resultsGrid.innerHTML = '<p class="loading-text">Searching...</p>';
        
        const params = new URLSearchParams();
        if (query) params.append('q', query);
        if (cuisine) params.append('cuisine', cuisine);
        if (city) params.append('city', city);
        if (rating && rating !== '0') params.append('rating', rating);
        
        const data = await apiRequest(`/api/restaurants/search?${params.toString()}`);
        
        if (data && data.restaurants && data.restaurants.length > 0) {
            heading.textContent = `Found ${data.restaurants.length} Restaurant${data.restaurants.length > 1 ? 's' : ''}`;
            displayRestaurants(data.restaurants, resultsGrid);
        } else {
            heading.textContent = 'No Results Found';
            resultsGrid.innerHTML = '<p class="no-results">No restaurants match your criteria. Try different filters!</p>';
        }
    } catch (error) {
        console.error('Search failed:', error);
        resultsGrid.innerHTML = '<p class="error-text">Failed to search. Please try again.</p>';
    }
}

async function loadAllRestaurants() {
    searchRestaurants();
}

async function loadRecommendedRestaurants() {
    const grid = document.getElementById('recommended-grid');
    
    try {
        const data = await apiRequest('/api/restaurants/recommended');
        
        if (data && data.restaurants && data.restaurants.length > 0) {
            displayRestaurants(data.restaurants.slice(0, 6), grid);
        } else {
            grid.innerHTML = '<p class="info-text">Start reviewing restaurants to get personalized recommendations!</p>';
        }
    } catch (error) {
        console.error('Failed to load recommendations:', error);
        document.getElementById('recommended-section').style.display = 'none';
    }
}

function displayRestaurants(restaurants, container) {
    container.innerHTML = '';
    
    restaurants.forEach(restaurant => {
        const card = document.createElement('div');
        card.className = 'search-restaurant-card';
        card.onclick = () => goToRestaurant(restaurant.id);
        
        card.innerHTML = `
            <img src="${restaurant.logoURL}" alt="${restaurant.name}" 
                 onerror="this.src='https://via.placeholder.com/250x180/4ECDC4/FFFFFF?text=${encodeURIComponent(restaurant.cuisine)}'">
            <div class="card-info">
                <h3>${restaurant.name}</h3>
                <p class="cuisine-tag">${restaurant.cuisine}</p>
                <p class="location"><span class="pin-icon"></span> ${restaurant.city}</p>
                <p class="category">${restaurant.category}</p>
                <div class="card-footer">
                    <span class="rating-badge"><span class="star-icon"></span> ${restaurant.rating.toFixed(1)}</span>
                    <button class="view-btn">View Details →</button>
                </div>
            </div>
        `;
        
        container.appendChild(card);
    });
}
