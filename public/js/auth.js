// Authentication page JavaScript

// Login form handling
const loginForm = document.getElementById('loginForm');
if (loginForm) {
    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;
        const errorDiv = document.getElementById('error-message');
        
        errorDiv.textContent = '';
		
        try {
            const response = await fetch(`${API_BASE}/api/login`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ email, password })
            });
            
            if (!response.ok) {
                const error = await response.json();
                throw new Error(error.error || 'Login failed');
            }
            
            const data = await response.json();
            
            // Save token and user
            setAuthToken(data.token);
            setUser(data.user);
            
            // Redirect to home
            window.location.href = 'index.html';
            
        } catch (error) {
            errorDiv.textContent = error.message;
        }
    });
}

// Signup form handling
const signupForm = document.getElementById('signupForm');
if (signupForm) {
    signupForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const name = document.getElementById('name').value;
        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;
        const city = document.getElementById('city').value;
        const errorDiv = document.getElementById('error-message');
        
        errorDiv.textContent = '';
        
        // Validation
        if (password.length < 6) {
            errorDiv.textContent = 'Password must be at least 6 characters';
            return;
        }
        
        try {
            const response = await fetch(`${API_BASE}/api/signup`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ name, email, password, city })
            });
            
            if (!response.ok) {
                const error = await response.json();
                throw new Error(error.error || 'Signup failed');
            }
            
            const data = await response.json();
            
            // Save token and user
            setAuthToken(data.token);
            setUser(data.user);
            
            // Redirect to home
            window.location.href = 'index.html';
            
        } catch (error) {
            errorDiv.textContent = error.message;
        }
    });
}
