// Restaurant detail page JavaScript

checkAuth();
const restaurantId = getURLParameter('id');

if (!restaurantId) {
    window.location.href = 'search.html';
} else {
    loadRestaurantDetails();
    loadMenu();
    loadReviews();
}

let currentTab = 'menu';

function switchTab(tab) {
    // Update button states
    document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
    event.target.classList.add('active');
    
    // Update content visibility
    document.querySelectorAll('.tab-content').forEach(content => content.classList.remove('active'));
    document.getElementById(`${tab}-tab`).classList.add('active');
    
    currentTab = tab;
}

async function loadRestaurantDetails() {
    try {
        const data = await apiRequest(`/api/restaurants/${restaurantId}`);
        
        if (data) {
            const header = document.getElementById('restaurant-header');
            header.innerHTML = `
                <img src="${data.logoURL}" alt="${data.name}" class="restaurant-image">
                <div class="restaurant-info">
                    <h1>${data.name}</h1>
                    <p class="cuisine-badge">${data.cuisine}</p>
                    <p class="restaurant-meta">
                        <span>📍 ${data.city}</span>
                        <span>⭐ ${data.rating.toFixed(1)}</span>
                        <span>${data.category}</span>
                    </p>
                </div>
            `;
        }
    } catch (error) {
        console.error('Failed to load restaurant:', error);
    }
}

async function loadMenu() {
    const grid = document.getElementById('menu-grid');
    
    try {
        const data = await apiRequest(`/api/restaurants/${restaurantId}/menu`);
        
        if (data && data.menuItems && data.menuItems.length > 0) {
            grid.innerHTML = '';
            data.menuItems.forEach(item => {
                const card = document.createElement('div');
                card.className = 'menu-item-card';
                card.innerHTML = `
                    <img src="${item.imageURL}" alt="${item.itemName}">
                    <div class="item-info">
                        <h3>${item.itemName}</h3>
                        <p class="price">Rs. ${item.price}</p>
                    </div>
                `;
                grid.appendChild(card);
            });
        } else {
            grid.innerHTML = '<p class="no-data">No menu items available.</p>';
        }
    } catch (error) {
        console.error('Failed to load menu:', error);
        grid.innerHTML = '<p class="error-text">Failed to load menu.</p>';
    }
}

async function loadReviews() {
    const list = document.getElementById('reviews-list');
    
    try {
        const data = await apiRequest(`/api/restaurants/${restaurantId}/reviews`);
        
        if (data && data.reviews && data.reviews.length > 0) {
            list.innerHTML = '';
            data.reviews.forEach(review => {
                const card = document.createElement('div');
                card.className = 'review-card';
                card.innerHTML = `
                    <div class="review-header">
                        <strong>${review.userName || 'Anonymous'}</strong>
                        <span class="review-date">${formatDate(review.timestamp)}</span>
                    </div>
                    <div class="review-ratings">
                        <span>Taste: ${Array(review.tasteRating).fill('⭐').join('')}</span>
                        <span>Ambiance: ${Array(review.ambianceRating).fill('⭐').join('')}</span>
                        <span>Overall: ${Array(review.overallRating).fill('⭐').join('')}</span>
                    </div>
                    <p class="review-comment">${review.comment}</p>
                `;
                list.appendChild(card);
            });
        } else {
            list.innerHTML = '<p class="no-data">No reviews yet. Be the first to review!</p>';
        }
    } catch (error) {
        console.error('Failed to load reviews:', error);
        list.innerHTML = '<p class="error-text">Failed to load reviews.</p>';
    }
}

// Opinion form submission
document.getElementById('opinion-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const taste = document.querySelector('input[name="taste"]:checked')?.value;
    const ambiance = document.querySelector('input[name="ambiance"]:checked')?.value;
    const overall = document.querySelector('input[name="overall"]:checked')?.value;
    const comment = document.getElementById('comment').value;
    const messageDiv = document.getElementById('form-message');
    
    if (!taste || !ambiance || !overall) {
        messageDiv.className = 'error-message';
        messageDiv.textContent = 'Please provide all ratings';
        return;
    }
    
    try {
        const response = await fetch(`${API_BASE}/api/restaurants/${restaurantId}/reviews`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${getAuthToken()}`
            },
            body: JSON.stringify({
                tasteRating: parseInt(taste),
                ambianceRating: parseInt(ambiance),
                overallRating: parseInt(overall),
                comment: comment
            })
        });
        
        if (response.ok) {
            messageDiv.className = 'success-message';
            messageDiv.textContent = '✅ Review submitted successfully!';
            document.getElementById('opinion-form').reset();
            setTimeout(() => {
                switchTab('reviews');
                loadReviews();
            }, 1500);
        } else {
            throw new Error('Submission failed');
        }
    } catch (error) {
        messageDiv.className = 'error-message';
        if (typeof error.message === 'string' && error.message.includes('already reviewed')) {
             messageDiv.textContent = 'You have already reviewed this restaurant.';
        } else {
             // Try to parse response error if possible, otherwise generic
             // In this block we don't have the response object directly unless we threw with it
             // Let's rely on the fact that existing code doesn't parse body on error, so we need to improve that
             messageDiv.textContent = 'Failed to submit review. ' + (error.message || 'Please try again.');
        }
    }
});
