// Home page JavaScript

// Check authentication
if (!checkAuth()) {
    // Redirects to login if not authenticated (handled in checkAuth)
} else {
    // Display user greeting
    const user = getUser();
    const topGreeting = document.getElementById('top-user-greeting');
    if (topGreeting && user) {
        topGreeting.textContent = `Welcome, ${user.name}!`;
    }
    
    // Show logout buttons
    const logoutBtn = document.getElementById('logout-btn');
    const logoutBtnMobile = document.getElementById('logout-btn-mobile');
    if (logoutBtn) {
        logoutBtn.style.display = 'inline-block';
        logoutBtn.onclick = logout;
    }
    if (logoutBtnMobile) {
        logoutBtnMobile.style.display = 'block';
        logoutBtnMobile.onclick = logout;
    }
    
    // Load trending restaurants
    loadTrendingRestaurants();
}

// Load trending restaurants from API
async function loadTrendingRestaurants() {
    const container = document.getElementById('trending-restaurants');
    
    try {
        showLoading('trending-restaurants');
        
        const data = await apiRequest('/api/restaurants/trending');
        
        if (data && data.restaurants && data.restaurants.length > 0) {
            container.innerHTML = '';
            data.restaurants.forEach(restaurant => {
                const card = document.createElement('div');
                card.className = 'restaurant-card';
                card.onclick = () => goToRestaurant(restaurant.id);
                
                card.innerHTML = `
                    <img src="${restaurant.logoURL}" alt="${restaurant.name}" 
                         onerror="this.src='https://via.placeholder.com/200x150/4ECDC4/FFFFFF?text=Restaurant'">
                    <div class="card-content">
                         <h3>${restaurant.name}</h3>
                        <p class="cuisine">${restaurant.cuisine} • ${restaurant.city}</p>
                        <p class="rating">⭐ ${restaurant.rating.toFixed(1) || 'N/A'}</p>
                        <p class="category">${restaurant.category}</p>
                        <button class="explore-btn">Explore</button>
                    </div>
                `;
                
                container.appendChild(card);
            });
        } else {
            container.innerHTML = '<p style="text-align: center; padding: 20px; color: #666;">No trending restaurants available.</p>';
        }
    } catch (error) {
        console.error('Failed to load trending restaurants:', error);
        container.innerHTML = '<p style="text-align: center; padding: 20px; color: #d32f2f;">Failed to load restaurants. Please try again later.</p>';
    }
}

// Initialize animations for restaurant cards
document.addEventListener('DOMContentLoaded', () => {
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('animate-show');
            }
        });
    });
    
    setTimeout(() => {
        const cards = document.querySelectorAll('.restaurant-card');
        cards.forEach(card => {
            observer.observe(card);
        });
    }, 1000);
});

// Carousel Scrolling Logic
const scrollLeftBtn = document.getElementById('scroll-left');
const scrollRightBtn = document.getElementById('scroll-right');
const trendingList = document.getElementById('trending-restaurants');

if (scrollLeftBtn && scrollRightBtn && trendingList) {
    scrollLeftBtn.addEventListener('click', () => {
        const scrollAmount = trendingList.clientWidth;
        trendingList.scrollBy({ left: -scrollAmount, behavior: 'smooth' });
    });

    scrollRightBtn.addEventListener('click', () => {
        const scrollAmount = trendingList.clientWidth;
        trendingList.scrollBy({ left: scrollAmount, behavior: 'smooth' });
    });
}
