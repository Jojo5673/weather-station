<template>
  <v-container fluid bg-color="surface" class="pa-4">
    <!-- Date Range Picker -->
    <v-row style="max-width: 1200px" class="mb-4">
      <v-col>
        <v-sheet class="pa-4" color="surface" rounded>
          <v-row align="center" class="gap-3">
            <v-col cols="auto">
              <v-text-field
                v-model="dateRange.start"
                label="Start Date"
                type="date"
                density="compact"
                variant="solo-inverted"
                flat
              ></v-text-field>
            </v-col>
            <v-col cols="auto">
              <v-text-field
                v-model="dateRange.end"
                label="End Date"
                type="date"
                density="compact"
                variant="solo-inverted"
                flat
              ></v-text-field>
            </v-col>
            <v-col cols="auto">
              <v-btn color="primary" @click="loadData" :loading="isLoading">Load Data</v-btn>
            </v-col>
            <v-col cols="auto" class="ml-auto">
              <v-checkbox
                v-model="showInCelsius"
                label="°C / °F"
                hide-details
              ></v-checkbox>
            </v-col>
            <v-col cols="auto">
              <v-checkbox
                v-model="showInMetres"
                label="m / ft"
                hide-details
              ></v-checkbox>
            </v-col>
            <v-col cols="auto">
              <v-checkbox
                v-model="showInPascals"
                label="Pa / atm"
                hide-details
              ></v-checkbox>
            </v-col>
          </v-row>
        </v-sheet>
      </v-col>
    </v-row>

    <!-- Error Message -->
    <v-row v-if="errorMessage" style="max-width: 1200px" class="mb-4">
      <v-col>
        <v-alert type="error" dismissible @click:close="errorMessage = ''">
          {{ errorMessage }}
        </v-alert>
      </v-col>
    </v-row>

    <!-- Tab Navigation -->
    <v-row style="max-width: 1200px" class="mb-4">
      <v-col>
        <v-tabs v-model="activeTab" color="primary">
          <v-tab value="stats">Aggregated Stats</v-tab>
          <v-tab value="trends">Trends</v-tab>
          <v-tab value="distributions">Distributions</v-tab>
          <v-tab value="correlations">Correlations</v-tab>
          <v-tab value="heat-stress">Heat Stress</v-tab>
        </v-tabs>
      </v-col>
    </v-row>

    <!-- AGGREGATED STATS TAB -->
    <v-window v-model="activeTab">
      <v-window-item value="stats" eager>
        <v-row style="max-width: 1200px" class="mb-4">
          <!-- Temperature Card -->
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-blue-lighten-2">Temperature</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="stat-row">
                <span class="stat-label">Min:</span>
                <span class="stat-value">{{ formatTemp(stats.minTemp) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Max:</span>
                <span class="stat-value">{{ formatTemp(stats.maxTemp) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Avg:</span>
                <span class="stat-value">{{ formatTemp(stats.avgTemp) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Range:</span>
                <span class="stat-value">{{ formatTemp(stats.maxTemp - stats.minTemp) }}</span>
              </div>
            </v-card>
          </v-col>

          <!-- Heat Index Card -->
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-pink-lighten-2">Heat Index</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="stat-row">
                <span class="stat-label">Min:</span>
                <span class="stat-value">{{ formatTemp(stats.minHeatIndex) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Max:</span>
                <span class="stat-value">{{ formatTemp(stats.maxHeatIndex) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Avg:</span>
                <span class="stat-value">{{ formatTemp(stats.avgHeatIndex) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Range:</span>
                <span class="stat-value">{{ formatTemp(stats.maxHeatIndex - stats.minHeatIndex) }}</span>
              </div>
            </v-card>
          </v-col>

          <!-- Humidity Card -->
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-cyan-lighten-2">Humidity</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="stat-row">
                <span class="stat-label">Min:</span>
                <span class="stat-value">{{ stats.minHumidity.toFixed(1) }}%</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Max:</span>
                <span class="stat-value">{{ stats.maxHumidity.toFixed(1) }}%</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Avg:</span>
                <span class="stat-value">{{ stats.avgHumidity.toFixed(1) }}%</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Range:</span>
                <span class="stat-value">{{ (stats.maxHumidity - stats.minHumidity).toFixed(1) }}%</span>
              </div>
            </v-card>
          </v-col>

          <!-- Moisture Card -->
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-green-lighten-2">Moisture</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="stat-row">
                <span class="stat-label">Min:</span>
                <span class="stat-value">{{ stats.minMoisture.toFixed(1) }}%</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Max:</span>
                <span class="stat-value">{{ stats.maxMoisture.toFixed(1) }}%</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Avg:</span>
                <span class="stat-value">{{ stats.avgMoisture.toFixed(1) }}%</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Range:</span>
                <span class="stat-value">{{ (stats.maxMoisture - stats.minMoisture).toFixed(1) }}%</span>
              </div>
            </v-card>
          </v-col>

          <!-- Pressure Card -->
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-orange-lighten-2">Pressure</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="stat-row">
                <span class="stat-label">Min:</span>
                <span class="stat-value">{{ formatPressure(stats.minPressure) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Max:</span>
                <span class="stat-value">{{ formatPressure(stats.maxPressure) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Avg:</span>
                <span class="stat-value">{{ formatPressure(stats.avgPressure) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Range:</span>
                <span class="stat-value">{{ formatPressure(stats.maxPressure - stats.minPressure) }}</span>
              </div>
            </v-card>
          </v-col>

          <!-- Altitude Card -->
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-amber-lighten-2">Altitude</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="stat-row">
                <span class="stat-label">Min:</span>
                <span class="stat-value">{{ formatAltitude(stats.minAltitude) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Max:</span>
                <span class="stat-value">{{ formatAltitude(stats.maxAltitude) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Avg:</span>
                <span class="stat-value">{{ formatAltitude(stats.avgAltitude) }}</span>
              </div>
              <div class="stat-row">
                <span class="stat-label">Range:</span>
                <span class="stat-value">{{ formatAltitude(stats.maxAltitude - stats.minAltitude) }}</span>
              </div>
            </v-card>
          </v-col>
        </v-row>
      </v-window-item>

      <!-- TRENDS TAB -->
      <v-window-item value="trends" eager>
        <v-row style="max-width: 1200px" class="mb-4">
          <v-col cols="auto" class="mb-3">
            <v-btn-toggle v-model="trendGranularity">
              <v-btn value="hourly">Hourly</v-btn>
              <v-btn value="daily">Daily</v-btn>
            </v-btn-toggle>
          </v-col>
        </v-row>
        <v-row style="max-width: 1200px">
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="trendTempChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="trendHumidityChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="trendPressureChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="trendAltitudeChart"></div>
            </figure>
          </v-col>
        </v-row>
      </v-window-item>

      <!-- DISTRIBUTIONS TAB -->
      <v-window-item value="distributions" eager>
        <v-row style="max-width: 1200px">
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="distTempChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="distHumidityChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="distMoistureChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="distPressureChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="distAltitudeChart"></div>
            </figure>
          </v-col>
        </v-row>
      </v-window-item>

      <!-- CORRELATIONS TAB -->
      <v-window-item value="correlations" eager>
        <v-row style="max-width: 1200px">
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="scatterTempHumidityChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="scatterHeatIndexTempChart"></div>
            </figure>
          </v-col>
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="scatterPressureAltitudeChart"></div>
            </figure>
          </v-col>
        </v-row>
      </v-window-item>

      <!-- HEAT STRESS TAB -->
      <v-window-item value="heat-stress" eager>
        <v-row style="max-width: 1200px">
          <v-col cols="12" sm="6" md="4">
            <v-card class="pa-4" color="#1e2d3d" flat>
              <v-card-title class="text-red-lighten-2">Heat Stress Events</v-card-title>
              <v-divider class="my-2"></v-divider>
              <div class="text-center">
                <div class="text-h3 text-red-lighten-2 font-weight-bold">{{ heatStressCount }}</div>
                <div class="text-body2 text-grey">Readings where Heat Index > 32°C</div>
              </div>
            </v-card>
          </v-col>
        </v-row>
        <v-row style="max-width: 1200px" class="mt-4">
          <v-col cols="12">
            <figure class="highcharts-figure">
              <div id="heatStressTimelineChart"></div>
            </figure>
          </v-col>
        </v-row>
      </v-window-item>
    </v-window>
  </v-container>
</template>

<script setup>
import { ref, reactive, watch, onMounted, onBeforeUnmount } from "vue";
import { useAppStore } from "@/store/appStore";
import Highcharts from "highcharts";
import Exporting from "highcharts/modules/exporting";
Exporting(Highcharts);

// Store
const AppStore = useAppStore();

// Date range state
const dateRange = reactive({
  start: "",
  end: ""
});

// Chart references
const charts = {};

// UI state
const activeTab = ref("stats");
const trendGranularity = ref("hourly");
const showInCelsius = ref(true);
const showInMetres = ref(true);
const showInPascals = ref(true);
const isLoading = ref(false);
const errorMessage = ref("");

// Data state
const stats = reactive({
  minTemp: 0, maxTemp: 0, avgTemp: 0,
  minHeatIndex: 0, maxHeatIndex: 0, avgHeatIndex: 0,
  minHumidity: 0, maxHumidity: 0, avgHumidity: 0,
  minMoisture: 0, maxMoisture: 0, avgMoisture: 0,
  minPressure: 0, maxPressure: 0, avgPressure: 0,
  minAltitude: 0, maxAltitude: 0, avgAltitude: 0
});

const heatStressCount = ref(0);

// Unit conversion functions
const celsiusToFahrenheit = (celsius) => (celsius * 9/5) + 32;
const metersToFeet = (meters) => meters * 3.28084;
const pascalsToAtm = (pascals) => pascals / 101325;

const formatTemp = (temp) => {
  if (showInCelsius.value) {
    return `${temp.toFixed(1)}°C`;
  } else {
    return `${celsiusToFahrenheit(temp).toFixed(1)}°F`;
  }
};

const formatAltitude = (alt) => {
  if (showInMetres.value) {
    return `${alt.toFixed(2)}m`;
  } else {
    return `${metersToFeet(alt).toFixed(2)}ft`;
  }
};

const formatPressure = (pressure) => {
  if (showInPascals.value) {
    return `${pressure.toFixed(0)}Pa`;
  } else {
    return `${pascalsToAtm(pressure).toFixed(4)}atm`;
  }
};

const getChartColor = (index) => {
  const colors = Highcharts.getOptions().colors;
  return colors[index % colors.length];
};

// Initialize user's date range - default to last 7 days
onMounted(() => {
  const today = new Date();
  const lastWeek = new Date(today.getTime() - 7 * 24 * 60 * 60 * 1000);
  
  dateRange.end = today.toISOString().split('T')[0];
  dateRange.start = lastWeek.toISOString().split('T')[0];
  
  createAllCharts();
});

const createAllCharts = () => {
  // Aggregated Stats Charts (empty, will be filled by createTrendCharts, etc)
  
  // Trend Charts
  createTrendTempChart();
  createTrendHumidityChart();
  createTrendPressureChart();
  createTrendAltitudeChart();
  
  // Distribution Charts
  createDistributionCharts();
  
  // Scatter Charts
  createScatterCharts();
  
  // Heat Stress Chart
  createHeatStressChart();
};

const createTrendTempChart = () => {
  charts.trendTemp = Highcharts.chart("trendTempChart", {
    chart: { zoomType: "x", backgroundColor: "#1e1e1e" },
    title: { text: "Temperature & Heat Index Trend", style: { color: "#FFFFFF" } },
    xAxis: { type: "datetime", labels: { style: { color: "#FFFFFF" } } },
    yAxis: { title: { text: "Temperature (°C)", style: { color: "#FFFFFF" } }, labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { shared: true, backgroundColor: "#444444" },
    series: [
      { name: "Temperature", type: "spline", data: [], color: getChartColor(0) },
      { name: "Heat Index", type: "spline", data: [], color: "#FFC0CB" }
    ]
  });
};

const createTrendHumidityChart = () => {
  charts.trendHumidity = Highcharts.chart("trendHumidityChart", {
    chart: { zoomType: "x", backgroundColor: "#1e1e1e" },
    title: { text: "Humidity & Moisture Trend", style: { color: "#FFFFFF" } },
    xAxis: { type: "datetime", labels: { style: { color: "#FFFFFF" } } },
    yAxis: { title: { text: "Percentage (%)", style: { color: "#FFFFFF" } }, labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { shared: true, backgroundColor: "#444444" },
    series: [
      { name: "Humidity", type: "spline", data: [], color: getChartColor(1) },
      { name: "Moisture", type: "spline", data: [], color: getChartColor(2) }
    ]
  });
};

const createTrendPressureChart = () => {
  charts.trendPressure = Highcharts.chart("trendPressureChart", {
    chart: { zoomType: "x", backgroundColor: "#1e1e1e" },
    title: { text: "Pressure Trend", style: { color: "#FFFFFF" } },
    xAxis: { type: "datetime", labels: { style: { color: "#FFFFFF" } } },
    yAxis: { title: { text: "Pressure (Pa)", style: { color: "#FFFFFF" } }, labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { shared: true, backgroundColor: "#444444" },
    series: [{ name: "Pressure", type: "spline", data: [], color: getChartColor(3) }]
  });
};

const createTrendAltitudeChart = () => {
  charts.trendAltitude = Highcharts.chart("trendAltitudeChart", {
    chart: { zoomType: "x", backgroundColor: "#1e1e1e" },
    title: { text: "Altitude Trend", style: { color: "#FFFFFF" } },
    xAxis: { type: "datetime", labels: { style: { color: "#FFFFFF" } } },
    yAxis: { title: { text: "Altitude (m)", style: { color: "#FFFFFF" } }, labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { shared: true, backgroundColor: "#444444" },
    series: [{ name: "Altitude", type: "spline", data: [], color: getChartColor(4) }]
  });
};

const createDistributionCharts = () => {
  const chartConfig = {
    chart: { backgroundColor: "#1e1e1e" },
    xAxis: { labels: { style: { color: "#FFFFFF" } } },
    yAxis: { title: { text: "Frequency", style: { color: "#FFFFFF" } }, labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { backgroundColor: "#444444" },
    series: [{ type: "column", data: [] }]
  };
  
  charts.distTemp = Highcharts.chart("distTempChart", {
    ...chartConfig,
    title: { text: "Temperature Distribution", style: { color: "#FFFFFF" } },
    xAxis: { ...chartConfig.xAxis, title: { text: "Temperature (°C)", style: { color: "#FFFFFF" } } },
    series: [{ ...chartConfig.series[0], color: getChartColor(0), name: "Temperature" }]
  });
  
  charts.distHumidity = Highcharts.chart("distHumidityChart", {
    ...chartConfig,
    title: { text: "Humidity Distribution", style: { color: "#FFFFFF" } },
    xAxis: { ...chartConfig.xAxis, title: { text: "Humidity (%)", style: { color: "#FFFFFF" } } },
    series: [{ ...chartConfig.series[0], color: getChartColor(1), name: "Humidity" }]
  });
  
  charts.distMoisture = Highcharts.chart("distMoistureChart", {
    ...chartConfig,
    title: { text: "Moisture Distribution", style: { color: "#FFFFFF" } },
    xAxis: { ...chartConfig.xAxis, title: { text: "Moisture (%)", style: { color: "#FFFFFF" } } },
    series: [{ ...chartConfig.series[0], color: getChartColor(2), name: "Moisture" }]
  });
  
  charts.distPressure = Highcharts.chart("distPressureChart", {
    ...chartConfig,
    title: { text: "Pressure Distribution", style: { color: "#FFFFFF" } },
    xAxis: { ...chartConfig.xAxis, title: { text: "Pressure (Pa)", style: { color: "#FFFFFF" } } },
    series: [{ ...chartConfig.series[0], color: getChartColor(3), name: "Pressure" }]
  });
  
  charts.distAltitude = Highcharts.chart("distAltitudeChart", {
    ...chartConfig,
    title: { text: "Altitude Distribution", style: { color: "#FFFFFF" } },
    xAxis: { ...chartConfig.xAxis, title: { text: "Altitude (m)", style: { color: "#FFFFFF" } } },
    series: [{ ...chartConfig.series[0], color: getChartColor(4), name: "Altitude" }]
  });
};

const createScatterCharts = () => {
  const scatterConfig = {
    chart: { type: "scatter", zoomType: "x", backgroundColor: "#1e1e1e" },
    xAxis: { labels: { style: { color: "#FFFFFF" } } },
    yAxis: { labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { backgroundColor: "#444444" },
    plotOptions: {
      scatter: {
        marker: { radius: 4, states: { hover: { enabled: true } } }
      }
    },
    series: [{ name: "Data", data: [] }]
  };
  
  charts.scatterTempHumidity = Highcharts.chart("scatterTempHumidityChart", {
    ...scatterConfig,
    title: { text: "Temperature vs Humidity", style: { color: "#FFFFFF" } },
    xAxis: { ...scatterConfig.xAxis, title: { text: "Temperature (°C)", style: { color: "#FFFFFF" } } },
    yAxis: { ...scatterConfig.yAxis, title: { text: "Humidity (%)", style: { color: "#FFFFFF" } } }
  });
  
  charts.scatterHeatIndexTemp = Highcharts.chart("scatterHeatIndexTempChart", {
    ...scatterConfig,
    title: { text: "Heat Index vs Temperature", style: { color: "#FFFFFF" } },
    xAxis: { ...scatterConfig.xAxis, title: { text: "Temperature (°C)", style: { color: "#FFFFFF" } } },
    yAxis: { ...scatterConfig.yAxis, title: { text: "Heat Index (°C)", style: { color: "#FFFFFF" } } }
  });
  
  charts.scatterPressureAltitude = Highcharts.chart("scatterPressureAltitudeChart", {
    ...scatterConfig,
    title: { text: "Pressure vs Altitude", style: { color: "#FFFFFF" } },
    xAxis: { ...scatterConfig.xAxis, title: { text: "Altitude (m)", style: { color: "#FFFFFF" } } },
    yAxis: { ...scatterConfig.yAxis, title: { text: "Pressure (Pa)", style: { color: "#FFFFFF" } } }
  });
};

const createHeatStressChart = () => {
  charts.heatStress = Highcharts.chart("heatStressTimelineChart", {
    chart: { type: "scatter", backgroundColor: "#1e1e1e" },
    title: { text: "Heat Stress Events Timeline", style: { color: "#FFFFFF" } },
    xAxis: { type: "datetime", labels: { style: { color: "#FFFFFF" } } },
    yAxis: { title: { text: "Heat Index (°C)", style: { color: "#FFFFFF" } }, labels: { style: { color: "#FFFFFF" } } },
    legend: { itemStyle: { color: "#FFFFFF" } },
    tooltip: { backgroundColor: "#444444" },
    plotOptions: {
      scatter: {
        marker: { radius: 6, fillColor: "red" }
      }
    },
    series: [{ name: "Heat Stress Events", data: [] }]
  });
};

// Load data based on date range
const loadData = async () => {
  if (!dateRange.start || !dateRange.end) {
    alert("Please select both start and end dates");
    return;
  }

  isLoading.value = true;
  const startTimestamp = Math.floor(new Date(dateRange.start).getTime() / 1000);
  const endTimestamp = Math.floor(new Date(dateRange.end).getTime() / 1000);

  try {
    // Load stats
    const statsRes = await AppStore.getAggregatedStats(startTimestamp, endTimestamp);
    if (statsRes.status === "found" && statsRes.data && statsRes.data[0]) {
      const d = statsRes.data[0];
      stats.minTemp = d.minTemp || 0;
      stats.maxTemp = d.maxTemp || 0;
      stats.avgTemp = d.avgTemp || 0;
      stats.minHeatIndex = d.minHeatIndex || 0;
      stats.maxHeatIndex = d.maxHeatIndex || 0;
      stats.avgHeatIndex = d.avgHeatIndex || 0;
      stats.minHumidity = d.minHumidity || 0;
      stats.maxHumidity = d.maxHumidity || 0;
      stats.avgHumidity = d.avgHumidity || 0;
      stats.minMoisture = d.minMoisture || 0;
      stats.maxMoisture = d.maxMoisture || 0;
      stats.avgMoisture = d.avgMoisture || 0;
      stats.minPressure = d.minPressure || 0;
      stats.maxPressure = d.maxPressure || 0;
      stats.avgPressure = d.avgPressure || 0;
      stats.minAltitude = d.minAltitude || 0;
      stats.maxAltitude = d.maxAltitude || 0;
      stats.avgAltitude = d.avgAltitude || 0;
    }

    // Load trends
    const trendsRes = await AppStore.getTrendLines(startTimestamp, endTimestamp, trendGranularity.value);
    if (trendsRes.status === "found" && trendsRes.data) {
      const trendPoints = trendsRes.data.map(point => ({
        x: point._id,
        temp: showInCelsius.value ? point.avgTemp : celsiusToFahrenheit(point.avgTemp),
        heatIndex: showInCelsius.value ? point.avgHeatIndex : celsiusToFahrenheit(point.avgHeatIndex),
        humidity: point.avgHumidity,
        moisture: point.avgMoisture,
        pressure: showInPascals.value ? point.avgPressure : pascalsToAtm(point.avgPressure),
        altitude: showInMetres.value ? point.avgAltitude : metersToFeet(point.avgAltitude)
      }));
      
      if (charts.trendTemp && charts.trendTemp.series) {
        charts.trendTemp.series[0].setData(trendPoints.map(p => [p.x, p.temp]));
        charts.trendTemp.series[1].setData(trendPoints.map(p => [p.x, p.heatIndex]));
      }
      if (charts.trendHumidity && charts.trendHumidity.series) {
        charts.trendHumidity.series[0].setData(trendPoints.map(p => [p.x, p.humidity]));
        charts.trendHumidity.series[1].setData(trendPoints.map(p => [p.x, p.moisture]));
      }
      if (charts.trendPressure && charts.trendPressure.series) {
        charts.trendPressure.series[0].setData(trendPoints.map(p => [p.x, p.pressure]));
      }
      if (charts.trendAltitude && charts.trendAltitude.series) {
        charts.trendAltitude.series[0].setData(trendPoints.map(p => [p.x, p.altitude]));
      }
    }

    // Load distributions - separate calls for each variable
    const distTempRes = await AppStore.getFrequencyDistribution("temperature", startTimestamp, endTimestamp);
    if (distTempRes.status === "found" && distTempRes.data && charts.distTemp && charts.distTemp.series) {
      const tempData = distTempRes.data.map(p => [
        showInCelsius.value ? p._id : celsiusToFahrenheit(p._id),
        p.count
      ]);
      charts.distTemp.series[0].setData(tempData);
    }

    const distHumidityRes = await AppStore.getFrequencyDistribution("humidity", startTimestamp, endTimestamp);
    if (distHumidityRes.status === "found" && distHumidityRes.data && charts.distHumidity && charts.distHumidity.series) {
      charts.distHumidity.series[0].setData(distHumidityRes.data.map(p => [p._id, p.count]));
    }

    const distMoistureRes = await AppStore.getFrequencyDistribution("moisture", startTimestamp, endTimestamp);
    if (distMoistureRes.status === "found" && distMoistureRes.data && charts.distMoisture && charts.distMoisture.series) {
      charts.distMoisture.series[0].setData(distMoistureRes.data.map(p => [p._id, p.count]));
    }

    const distPressureRes = await AppStore.getFrequencyDistribution("pressure", startTimestamp, endTimestamp);
    if (distPressureRes.status === "found" && distPressureRes.data && charts.distPressure && charts.distPressure.series) {
      const pressureData = distPressureRes.data.map(p => [
        showInPascals.value ? p._id : pascalsToAtm(p._id),
        p.count
      ]);
      charts.distPressure.series[0].setData(pressureData);
    }

    const distAltitudeRes = await AppStore.getFrequencyDistribution("altitude", startTimestamp, endTimestamp);
    if (distAltitudeRes.status === "found" && distAltitudeRes.data && charts.distAltitude && charts.distAltitude.series) {
      const altitudeData = distAltitudeRes.data.map(p => [
        showInMetres.value ? p._id : metersToFeet(p._id),
        p.count
      ]);
      charts.distAltitude.series[0].setData(altitudeData);
    }

    // Load scatter data
    const scatterRes = await AppStore.getScatterPlotData(startTimestamp, endTimestamp);
    if (scatterRes.status === "found" && scatterRes.data) {
      const data = scatterRes.data;
      if (charts.scatterTempHumidity && charts.scatterTempHumidity.series) {
        charts.scatterTempHumidity.series[0].setData(data.map(p => [
          showInCelsius.value ? p.temperature : celsiusToFahrenheit(p.temperature),
          p.humidity
        ]));
      }
      if (charts.scatterHeatIndexTemp && charts.scatterHeatIndexTemp.series) {
        charts.scatterHeatIndexTemp.series[0].setData(data.map(p => [
          showInCelsius.value ? p.temperature : celsiusToFahrenheit(p.temperature),
          showInCelsius.value ? p.heat_index : celsiusToFahrenheit(p.heat_index)
        ]));
      }
      if (charts.scatterPressureAltitude && charts.scatterPressureAltitude.series) {
        charts.scatterPressureAltitude.series[0].setData(data.map(p => [
          showInMetres.value ? p.altitude : metersToFeet(p.altitude),
          showInPascals.value ? p.pressure : pascalsToAtm(p.pressure)
        ]));
      }
    }

    // Load heat stress events
    const heatRes = await AppStore.getHeatStressEvents(startTimestamp, endTimestamp, 32);
    if (heatRes.status === "found" && heatRes.data) {
      heatStressCount.value = heatRes.data.count;
      const events = heatRes.data.events.map(e => [
        e.timestamp * 1000,
        showInCelsius.value ? e.heat_index : celsiusToFahrenheit(e.heat_index)
      ]);
      if (charts.heatStress && charts.heatStress.series) {
        charts.heatStress.series[0].setData(events);
      }
    }
  } catch (error) {
    console.error("Error loading data:", error);
    errorMessage.value = "Failed to load analysis data. Please check your connection and try again.";
  } finally {
    isLoading.value = false;
  }
};

watch(trendGranularity, () => {
  if (dateRange.start && dateRange.end) {
    loadData();
  }
});

// Watch unit toggles to reload data with new units
watch(showInCelsius, () => {
  if (dateRange.start && dateRange.end) {
    loadData();
  }
});

watch(showInMetres, () => {
  if (dateRange.start && dateRange.end) {
    loadData();
  }
});

watch(showInPascals, () => {
  if (dateRange.start && dateRange.end) {
    loadData();
  }
});
</script>

<style scoped>
.stat-row {
  display: flex;
  justify-content: space-between;
  padding: 8px 0;
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.stat-row:last-child {
  border-bottom: none;
}

.stat-label {
  color: rgba(255, 255, 255, 0.7);
  font-weight: 500;
}

.stat-value {
  color: #fff;
  font-weight: bold;
  font-family: monospace;
}

figure.highcharts-figure {
  border: 2px solid #444;
  border-radius: 4px;
  margin: 16px 0;
}
</style>