// Restaurant detail page JavaScript

checkAuth();
const restaurantId = getURLParameter('id');

if (!restaurantId) {
    window.location.href = 'search.html';
} else {
    loadRestaurantDetails();
    loadReviews();
}

let currentTab = 'reviews';

function switchTab(tab) {
    // Update button states
    document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
    // Find the button that was clicked
    const clickedBtn = Array.from(document.querySelectorAll('.tab-btn')).find(btn => btn.getAttribute('onclick').includes(tab));
    if (clickedBtn) clickedBtn.classList.add('active');
    
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
                    <div class="restaurant-meta" style="display: flex; gap: 20px; color: var(--text-dim); margin-top: 15px;">
                        <span style="display: flex; align-items: center;"><span class="icon icon-map" style="color: var(--accent); margin-right: 8px;"></span> ${data.city}</span>
                        <span style="display: flex; align-items: center;"><span class="icon icon-star" style="color: #ffd700; margin-right: 8px;"></span> ${data.rating.toFixed(1)}</span>
                        <span>${data.category}</span>
                    </div>
                </div>
            `;
        }
    } catch (error) {
        console.error('Failed to load restaurant:', error);
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
                    <div class="review-ratings" style="display: flex; gap: 15px; margin: 10px 0; font-size: 13px;">
                        <span style="display: flex; align-items: center;">Taste: ${getStarsHTML(review.tasteRating)}</span>
                        <span style="display: flex; align-items: center;">Ambiance: ${getStarsHTML(review.ambianceRating)}</span>
                        <span style="display: flex; align-items: center;">Overall: ${getStarsHTML(review.overallRating)}</span>
                    </div>
                    <p class="review-comment" style="color: var(--text-dim); line-height: 1.5;">${review.comment}</p>
                `;
                list.appendChild(card);
            });
        } else {
            list.innerHTML = '<p class="no-data">No expert reviews yet. Be the first to analyze!</p>';
        }
    } catch (error) {
        console.error('Failed to load reviews:', error);
    }
}

function getStarsHTML(count) {
    let html = '';
    for(let i=0; i<5; i++) {
        html += `<span class="icon icon-star" style="width: 12px; height: 12px; margin-right: 2px; color: ${i < count ? '#ffd700' : 'rgba(255,255,255,0.1)'};"></span>`;
    }
    return html;
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
            messageDiv.textContent = 'Review submitted successfully!';
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
        messageDiv.textContent = 'Failed to submit review. Please try again.';
    }
});
