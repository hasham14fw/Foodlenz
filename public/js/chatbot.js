// Chatbot page JavaScript

checkAuth();

const chatMessages = document.getElementById('chat-messages');
const chatInput = document.getElementById('chat-input');

function handleKeyPress(event) {
    if (event.key === 'Enter') {
        sendMessage();
    }
}

async function sendMessage() {
    const message = chatInput.value.trim();
    
    if (!message) return;
    
    // Add user message
    addMessage(message, 'user');
    chatInput.value = '';
    
    // Show typing indicator
    addTypingIndicator();
    
    try {
        const response = await fetch(`${API_BASE}/api/chatbot`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${getAuthToken()}`
            },
            body: JSON.stringify({ message })
        });
        
        removeTypingIndicator();
        
        if (response.ok) {
            const data = await response.json();
            
            // Add bot response
            addMessage(data.message, 'bot');
            
            // Add restaurant suggestions if any
            if (data.restaurants && data.restaurants.length > 0) {
                addRestaurantSuggestions(data.restaurants);
            }
        } else {
            addMessage('Sorry, I encountered an error. Please try again!', 'bot');
        }
    } catch (error) {
        removeTypingIndicator();
        console.error('Chatbot error:', error);
        addMessage('Sorry, something went wrong. Please check your connection.', 'bot');
    }
}

function addMessage(text, sender) {
    const messageDiv = document.createElement('div');
    messageDiv.className = `message ${sender}-message`;
    messageDiv.innerHTML = `<p>${text}</p>`;
    chatMessages.appendChild(messageDiv);
    scrollToBottom();
}

function addRestaurantSuggestions(restaurants) {
    const suggestionsDiv = document.createElement('div');
    suggestionsDiv.className = 'restaurant-suggestions';
    
    restaurants.forEach(restaurant => {
        const card = document.createElement('div');
        card.className = 'suggestion-card';
        card.onclick = () => goToRestaurant(restaurant.id);
        
        card.innerHTML = `
            <img src="${restaurant.logoURL}" alt="${restaurant.name}">
            <div class="suggestion-info">
                <h4>${restaurant.name}</h4>
                <p>${restaurant.cuisine} • ${restaurant.city}</p>
                <p class="rating"><span class="star-icon"></span> ${restaurant.rating.toFixed(1)}</p>
            </div>
        `;
        
        suggestionsDiv.appendChild(card);
    });
    
    chatMessages.appendChild(suggestionsDiv);
    scrollToBottom();
}

function addTypingIndicator() {
    const typing = document.createElement('div');
    typing.className = 'message bot-message typing-indicator';
    typing.id = 'typing';
    typing.innerHTML = '<p><span></span><span></span><span></span></p>';
    chatMessages.appendChild(typing);
    scrollToBottom();
}

function removeTypingIndicator() {
    const typing = document.getElementById('typing');
    if (typing) {
        typing.remove();
    }
}

function scrollToBottom() {
    chatMessages.scrollTop = chatMessages.scrollHeight;
}
