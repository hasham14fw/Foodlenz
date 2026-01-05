// Weather.js
const apiBase = "http://localhost:3000/api/weather?city=";

// Show current date
const dateElem = document.getElementById('current-date');
function updateDate() {
    const now = new Date();
    dateElem.innerText = now.toLocaleDateString() + " | " + now.toLocaleTimeString();
}
updateDate();
setInterval(updateDate, 60000);

// Weather Animation
function updateAnimation(weather) {
    const anim = document.getElementById('weather-animation');
    weather = weather.toLowerCase();

    if (weather.includes("rain")) {
        anim.innerText = "🌧️";
        anim.className = "weather-animation rain";
    } else if (weather.includes("cloud")) {
        anim.innerText = "⛅";
        anim.className = "weather-animation cloud";
    } else if (weather.includes("snow")) {
        anim.innerText = "❄️";
        anim.className = "weather-animation snow";
    } else {
        anim.innerText = "☀️";
        anim.className = "weather-animation sun";
    }
}

// Fetch Weather
async function getWeather() {
    const city = document.getElementById('city-input').value || "Islamabad";

    try {
        const response = await fetch(apiBase + city);
        // Check for HTTP errors (e.g., 404, 500)
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();

        console.log(data); // debug

        document.getElementById('temp').innerText = data.temp + "°C";
        document.getElementById('description').innerText = data.description;
        // Convert Unix timestamp to readable time
        document.getElementById('sunrise').innerText = new Date(data.sunrise * 1000).toLocaleTimeString();
        document.getElementById('sunset').innerText = new Date(data.sunset * 1000).toLocaleTimeString();

        updateAnimation(data.description);
    } catch (err) {
        console.error(err);
        alert("Weather could not load. Server may be offline or API key invalid.");
    }
}

// Load default weather on page load
getWeather();