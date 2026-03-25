// Search page JavaScript
checkAuth();

let allFoundRestaurants = [];
let displayedCount = 0;
const ITEMS_PER_PAGE = 3;

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
    const showMoreContainer = document.getElementById('show-more-container');
    
    try {
        resultsGrid.innerHTML = '<p class="loading-text">Mining the best options for you...</p>';
        showMoreContainer.style.display = 'none';
        
        const params = new URLSearchParams();
        if (query) params.append('q', query);
        if (cuisine) params.append('cuisine', cuisine);
        if (city) params.append('city', city);
        if (rating && rating !== '0') params.append('rating', rating);
        
        const data = await apiRequest(`/api/restaurants/search?${params.toString()}`);
        
        if (data && data.restaurants && data.restaurants.length > 0) {
            allFoundRestaurants = data.restaurants;
            displayedCount = 0;
            resultsGrid.innerHTML = '';
            heading.textContent = `Found ${allFoundRestaurants.length} Elite Establishments`;
            showMoreResults();
        } else {
            heading.textContent = 'No Matches Found';
            resultsGrid.innerHTML = '<p class="no-results" style="grid-column: 1/-1; text-align: center; padding: 40px; color: var(--text-dim);">No restaurants match your elite criteria. Try adjusting your search.</p>';
        }
    } catch (error) {
        console.error('Search failed:', error);
        resultsGrid.innerHTML = '<p class="error-text">Failed to retrieve data. Please try again.</p>';
    }
}

function showMoreResults() {
    const resultsGrid = document.getElementById('results-grid');
    const showMoreContainer = document.getElementById('show-more-container');
    
    const nextBatch = allFoundRestaurants.slice(displayedCount, displayedCount + ITEMS_PER_PAGE);
    displayRestaurants(nextBatch, resultsGrid, false);
    displayedCount += nextBatch.length;
    
    if (displayedCount < allFoundRestaurants.length) {
        showMoreContainer.style.display = 'flex';
    } else {
        showMoreContainer.style.display = 'none';
    }
}

// Hook up show more button
document.getElementById('show-more-btn').onclick = showMoreResults;

async function loadAllRestaurants() {
    searchRestaurants();
}

async function loadRecommendedRestaurants() {
    const grid = document.getElementById('recommended-grid');
    try {
        const data = await apiRequest('/api/restaurants/recommended');
        if (data && data.restaurants && data.restaurants.length > 0) {
            displayRestaurants(data.restaurants.slice(0, 3), grid, true);
        }
    } catch (error) {
        console.error('Failed to load recommendations:', error);
    }
}

function displayRestaurants(restaurants, container, clear = true) {
    if (clear) container.innerHTML = '';
    
    restaurants.forEach(restaurant => {
        const card = document.createElement('div');
        card.className = 'restaurant-card';
        card.style.animation = 'fadeIn 0.5s forwards';
        card.onclick = () => goToRestaurant(restaurant.id);
        
        card.innerHTML = `
            <img src="${restaurant.logoURL}" alt="${restaurant.name}" 
                 onerror="this.src='https://via.placeholder.com/250x180/4ECDC4/FFFFFF?text=${encodeURIComponent(restaurant.cuisine)}'">
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
                <button class="explore-btn" style="margin-top: 15px; width: 100%;">Explore Selection →</button>
            </div>
        `;
        
        container.appendChild(card);
    });
}
