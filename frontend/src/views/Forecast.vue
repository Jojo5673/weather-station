<template>
  <VContainer bg-color="surface" fluid>
    <VRow class="my-5" justify="center">
      <VCol cols="12" md="8">
        <h1 class="text-h4 mb-4">7-Day Weather Forecast</h1>

        <div class="d-flex align-center justify-space-between mb-4">
          <div>
            <span class="text-subtitle-1">Location:</span>
            <strong>{{ locationName }}</strong>
          </div>
          <div v-if="loading">Loading latest forecast...</div>
          <div v-if="error" class="text-error">{{ error }}</div>
        </div>

        <v-row v-if="!loading && !error" class="mb-6" dense>
          <v-col v-for="day in forecast" :key="day.date" cols="12" sm="6" md="4">
            <v-card elevation="3" color="surface" class="forecast-card pa-4">
              <div class="forecast-card-head d-flex align-center justify-space-between mb-2">
                <div class="text-subtitle-2 font-weight-medium">{{ day.date }}</div>
                <v-chip color="primary" text-color="on-primary" small>
                  {{ day.precipProbability }}%
                  <v-icon size="16" class="ml-1">mdi-weather-rainy</v-icon>
                </v-chip>
              </div>

              <div class="d-flex align-center mb-2">
                <v-icon :icon="day.icon" size="32" class="mr-2" />
                <span class="text-h6">{{ day.condition }}</span>
              </div>

              <div class="forecast-values d-flex align-center justify-space-between">
                <span><strong>High:</strong> {{ day.tempMax }}°C</span>
                <span><strong>Low:</strong> {{ day.tempMin }}°C</span>
              </div>
            </v-card>
          </v-col>
        </v-row>

        <figure class="highcharts-figure">
          <div id="forecast-chart" style="height: 350px"></div>
        </figure>
      </VCol>
    </VRow>
  </VContainer>
</template>

<script setup>
import { ref, onMounted, watch } from "vue";
import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";

more(Highcharts);
Exporting(Highcharts);

const forecast = ref([]);
const loading = ref(true);
const error = ref(null);
const locationName = ref("Detecting location...");
const forecastChart = ref(null);

const weatherCodeMap = {
  0: "Clear",
  1: "Mainly clear",
  2: "Partly cloudy",
  3: "Overcast",
  45: "Fog",
  48: "Depositing rime fog",
  51: "Light drizzle",
  53: "Moderate drizzle",
  55: "Dense drizzle",
  56: "Freezing drizzle",
  57: "Dense freezing drizzle",
  61: "Slight rain",
  63: "Moderate rain",
  65: "Heavy rain",
  66: "Freezing rain",
  67: "Heavy freezing rain",
  71: "Slight snow",
  73: "Moderate snow",
  75: "Heavy snow",
  77: "Snow grains",
  80: "Slight shower rain",
  81: "Moderate shower rain",
  82: "Violent shower rain",
  85: "Slight shower snow",
  86: "Heavy shower snow",
  95: "Thunderstorm",
  96: "Thunderstorm with slight hail",
  99: "Thunderstorm with heavy hail",
};

const weatherIconMap = {
  0: "mdi-weather-sunny",
  1: "mdi-weather-partly-cloudy",
  2: "mdi-weather-partly-cloudy",
  3: "mdi-weather-cloudy",
  45: "mdi-weather-fog",
  48: "mdi-weather-fog",
  51: "mdi-weather-partially-rainy",
  53: "mdi-weather-rainy",
  55: "mdi-weather-pouring",
  56: "mdi-weather-snowy-rainy",
  57: "mdi-weather-snowy-rainy",
  61: "mdi-weather-rainy",
  63: "mdi-weather-rainy",
  65: "mdi-weather-pouring",
  66: "mdi-weather-snowy-rainy",
  67: "mdi-weather-snowy-rainy",
  71: "mdi-weather-snowy",
  73: "mdi-weather-snowy",
  75: "mdi-weather-snowy-heavy",
  77: "mdi-weather-snowy",
  80: "mdi-weather-pouring",
  81: "mdi-weather-pouring",
  82: "mdi-weather-lightning-rainy",
  85: "mdi-weather-snowy",
  86: "mdi-weather-snowy-heavy",
  95: "mdi-weather-lightning",
  96: "mdi-weather-lightning-rainy",
  99: "mdi-weather-lightning-rainy",
};

const openMeteoFetch = async (lat, lon) => {
  const url = `https://api.open-meteo.com/v1/forecast?latitude=${lat}&longitude=${lon}&daily=temperature_2m_max,temperature_2m_min,precipitation_probability_mean,weathercode&timezone=auto`;
  const res = await fetch(url);
  if (!res.ok) throw new Error("Forecast API failed");
  return await res.json();
};

const loadForecast = async (lat, lon) => {
  loading.value = true;
  error.value = null;
  try {
    const data = await openMeteoFetch(lat, lon);
    locationName.value = `${data.latitude.toFixed(2)}, ${data.longitude.toFixed(2)}`;

    const days = data.daily.time.map((date, index) => {
      const code = data.daily.weathercode[index];
      return {
        date: new Date(date).toLocaleDateString(undefined, { weekday: "short", month: "short", day: "numeric" }),
        tempMax: Math.round(data.daily.temperature_2m_max[index] * 10) / 10,
        tempMin: Math.round(data.daily.temperature_2m_min[index] * 10) / 10,
        precipProbability: Math.round(data.daily.precipitation_probability_mean[index]),
        condition: weatherCodeMap[code] || "Unknown",
        icon: weatherIconMap[code] || "mdi-weather-partly-cloudy",
      };
    });

    forecast.value = days;
    createForecastChart(days);
  } catch (err) {
    error.value = "Unable to load forecast data. " + err.message;
  } finally {
    loading.value = false;
  }
};

const createForecastChart = (days) => {
  const dates = days.map((d) => d.date);
  const maxTemps = days.map((d) => d.tempMax);
  const minTemps = days.map((d) => d.tempMin);

  forecastChart.value = Highcharts.chart("forecast-chart", {
    chart: { type: "line", backgroundColor: "#1e1e1e" },
    title: { text: "7-Day High/Low Temperature", style: { color: "#fff" } },
    xAxis: { categories: dates, labels: { style: { color: "#fff" } } },
    yAxis: { title: { text: "°C", style: { color: "#fff" } }, labels: { style: { color: "#fff" } } },
    legend: { itemStyle: { color: "#fff" } },
    series: [
      { name: "High", data: maxTemps, color: "#FF9999" },
      { name: "Low", data: minTemps, color: "#66CCFF" },
    ],
    tooltip: { shared: true, backgroundColor: "#2b2b2b", style: { color: "#fff" } },
    credits: { enabled: false },
    plotOptions: {
      line: { marker: { enabled: true, radius: 3 } },
    },
  });
};

onMounted(() => {
  if (navigator.geolocation) {
    navigator.geolocation.getCurrentPosition(
      (position) => {
        loadForecast(position.coords.latitude, position.coords.longitude);
      },
      () => {
        // fallback location (New York City)
        locationName.value = "Default location Kingston, Jamaica";
        loadForecast(18.0179, 76.8099);
      },
      { timeout: 5000 },
    );
  } else {
    locationName.value = "Default location Kingston, Jamaica";
    loadForecast(18.0179, 76.8099);
  }
});
</script>

<style scoped>
.highcharts-figure {
  min-width: 310px;
  max-width: 100%;
  margin: 1em auto;
}

.forecast-card {
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.12);
  background: linear-gradient(145deg, rgba(10, 17, 34, 0.95), rgba(16, 24, 40, 0.9));
  backdrop-filter: blur(8px);
}

.forecast-card-head {
  border-bottom: 1px solid rgba(255, 255, 255, 0.08);
  padding-bottom: 8px;
}

.forecast-values span {
  color: #d1d9f5;
}

.text-error {
  color: #ffc1c1;
}
</style>
