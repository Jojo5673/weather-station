<template>
  <VContainer fluid>
    <VRow class="py-6" justify="space-between" align="center">
      <VCol cols="12" md="8">
        <h1 class="text-h4 mb-2">Live Station Metrics</h1>
        <p class="text-subtitle-2 text-secondary">Real-time stream from MQTT topic <strong>620172690</strong>.</p>
      </VCol>
      <VCol cols="12" md="4" class="d-flex justify-end">
        <VCard flat class="unit-card px-3 py-2">
          <div class="d-flex align-center gap-2">
            <span class="font-weight-medium">Temperature unit</span>
            <VBtnGroup>
              <VBtn :variant="tempUnit === 'C' ? 'tonal' : 'text'" @click="tempUnit = 'C'">°C</VBtn>
              <VBtn :variant="tempUnit === 'F' ? 'tonal' : 'text'" @click="tempUnit = 'F'">°F</VBtn>
            </VBtnGroup>
          </div>
          <div class="d-flex align-center gap-2 mt-2">
            <span class="font-weight-medium">Altitude unit</span>
            <VBtnGroup>
              <VBtn :variant="altUnit === 'm' ? 'tonal' : 'text'" @click="altUnit = 'm'">m</VBtn>
              <VBtn :variant="altUnit === 'ft' ? 'tonal' : 'text'" @click="altUnit = 'ft'">ft</VBtn>
            </VBtnGroup>
          </div>
          <div class="d-flex align-center gap-2 mt-2">
            <span class="font-weight-medium">Pressure unit</span>
            <VBtnGroup>
              <VBtn :variant="pressUnit === 'Pa' ? 'tonal' : 'text'" @click="pressUnit = 'Pa'">Pa</VBtn>
              <VBtn :variant="pressUnit === 'atm' ? 'tonal' : 'text'" @click="pressUnit = 'atm'">atm</VBtn>
            </VBtnGroup>
          </div>
        </VCard>
      </VCol>
    </VRow>

    <VRow class="mb-4">
      <VCol cols="12">
        <VAlert
          v-if="!hasLiveData"
          type="warning"
          variant="tonal"
          icon="mdi-cloud-question"
          class="mb-4"
        >
          No live data received yet. Waiting for MQTT payload on <strong>620172690</strong>...
        </VAlert>
      </VCol>
    </VRow>

    <VRow class="mb-4" align="stretch">
      <VCol cols="12" sm="6" md="4" v-for="tile in tiles" :key="tile.label">
        <VCard class="tile-card" flat>
          <VCardTitle class="justify-space-between">
            <div>
              <span>{{ tile.label }}</span>
              <div class="text-caption text-secondary">{{ tile.subtitle }}</div>
            </div>
            <VIcon size="28">{{ tile.icon }}</VIcon>
          </VCardTitle>
          <VCardText class="text-h5 font-weight-bold">{{ tile.value }}</VCardText>
        </VCard>
      </VCol>
    </VRow>

    <VRow class="mb-6" dense>
      <VCol cols="12" md="6">
        <figure class="live-chart">
          <div id="chart-temp" class="chart-box"></div>
        </figure>
      </VCol>
      <VCol cols="12" md="6">
        <figure class="live-chart">
          <div id="chart-humidity" class="chart-box"></div>
        </figure>
      </VCol>
    </VRow>

    <VRow class="mb-6" dense>
      <VCol cols="12" md="6">
        <figure class="live-chart">
          <div id="chart-pressure" class="chart-box"></div>
        </figure>
      </VCol>
      <VCol cols="12" md="6">
        <figure class="live-chart">
          <div id="chart-altitude" class="chart-box"></div>
        </figure>
      </VCol>
    </VRow>
  </VContainer>
</template>

<script setup>
import { ref, computed, watch, onMounted, onBeforeUnmount } from "vue";
import { useMqttStore } from "@/store/mqttStore";
import { storeToRefs } from "pinia";
import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";

more(Highcharts);
Exporting(Highcharts);

const Mqtt = useMqttStore();
const { payload } = storeToRefs(Mqtt);

const tempUnit = ref("C");
const altUnit = ref("m");
const pressUnit = ref("Pa");

const liveHistory = ref([]);
const lastTimestamp = computed(() => {
  if (!liveHistory.value.length) return null;
  return liveHistory.value[liveHistory.value.length - 1].timestamp;
});

const hasLiveData = computed(() => {
  if (!lastTimestamp.value) return false;
  const seconds = Date.now() / 1000 - lastTimestamp.value;
  return seconds < 45;
});

const lastSeenAgo = computed(() => {
  if (!lastTimestamp.value) return "--";
  return Math.floor(Date.now() / 1000 - lastTimestamp.value);
});

const temperatureValue = computed(() => {
  if (!liveHistory.value.length) return "--";
  const value = liveHistory.value[liveHistory.value.length - 1].temperature;
  if (tempUnit.value === "F") return `${cToF(value).toFixed(1)} °F`;
  return `${value.toFixed(1)} °C`;
});

const heatIndexValue = computed(() => {
  if (!liveHistory.value.length) return "--";
  const value = liveHistory.value[liveHistory.value.length - 1].heat_index;
  if (tempUnit.value === "F") return `${cToF(value).toFixed(1)} °F`;
  return `${value.toFixed(1)} °C`;
});

const humidityValue = computed(() => {
  if (!liveHistory.value.length) return "--";
  return `${liveHistory.value[liveHistory.value.length - 1].humidity.toFixed(1)} %`;
});

const moistureValue = computed(() => {
  if (!liveHistory.value.length) return "--";
  return `${liveHistory.value[liveHistory.value.length - 1].moisture.toFixed(1)} %`;
});

const pressureValue = computed(() => {
  if (!liveHistory.value.length) return "--";
  const value = liveHistory.value[liveHistory.value.length - 1].pressure;
  if (pressUnit.value === "atm") return `${(value / 101325).toFixed(3)} atm`;
  return `${value.toFixed(0)} Pa`;
});

const altitudeValue = computed(() => {
  if (!liveHistory.value.length) return "--";
  const value = liveHistory.value[liveHistory.value.length - 1].altitude;
  if (altUnit.value === "ft") return `${(value * 3.28084).toFixed(1)} ft`;
  return `${value.toFixed(1)} m`;
});

const tiles = computed(() => [
  { label: "Temperature", subtitle: `Unit: ${tempUnit.value}`, icon: "mdi-temperature-celsius", value: temperatureValue.value },
  { label: "Heat Index", subtitle: `Unit: ${tempUnit.value}`, icon: "mdi-thermometer", value: heatIndexValue.value },
  { label: "Humidity", subtitle: "%", icon: "mdi-water-percent", value: humidityValue.value },
  { label: "Soil Moisture", subtitle: "%", icon: "mdi-leaf", value: moistureValue.value },
  { label: "Pressure", subtitle: `Unit: ${pressUnit.value}`, icon: "mdi-gauge", value: pressureValue.value },
  { label: "Altitude", subtitle: `Unit: ${altUnit.value}`, icon: "mdi-altimeter", value: altitudeValue.value },
]);

const tempChart = ref(null);
const pressureChart = ref(null);
const altitudeChart = ref(null);
const humChart = ref(null);

const cToF = (c) => (c * 9) / 5 + 32;
const mToFt = (m) => m * 3.28084;
const paToAtm = (p) => p / 101325;

const EST_OFFSET_MS = -5 * 60 * 60 * 1000;
const toEST = (ts) => ts * 1000 + EST_OFFSET_MS;

const aggregatedSeries = () => {
  const tempSeries = liveHistory.value.map((item) => [toEST(item.timestamp), (tempUnit.value === "F" ? cToF(item.temperature) : item.temperature)]);
  const hiSeries = liveHistory.value.map((item) => [toEST(item.timestamp), (tempUnit.value === "F" ? cToF(item.heat_index) : item.heat_index)]);
  const pressureSeries = liveHistory.value.map((item) => [toEST(item.timestamp), (pressUnit.value === "atm" ? paToAtm(item.pressure) : item.pressure)]);
  const altitudeSeries = liveHistory.value.map((item) => [toEST(item.timestamp), (altUnit.value === "ft" ? mToFt(item.altitude) : item.altitude)]);
  const humiditySeries = liveHistory.value.map((item) => [toEST(item.timestamp), item.humidity]);
  const moistureSeries = liveHistory.value.map((item) => [toEST(item.timestamp), item.moisture]);
  return { tempSeries, hiSeries, pressureSeries, altitudeSeries, humiditySeries, moistureSeries };
};

const createCharts = () => {
  tempChart.value = Highcharts.chart("chart-temp", {
    chart: { type: "spline", backgroundColor: "#111a2b" },
    title: { text: "Temperature / Heat Index", style: { color: "#fff" } },
    xAxis: { type: "datetime", labels: { style: { color: "#f1f5f9" } } },
    yAxis: { title: { text: tempUnit.value === "F" ? "°F" : "°C", style: { color: "#f1f5f9" } }, labels: { style: { color: "#f1f5f9" } } },
    tooltip: { shared: true, backgroundColor: "#1b2434", style: { color: "#ffffff" } },
    legend: { itemStyle: { color: "#fff" } },
    series: [
      { name: "Temperature", data: [], color: "#ff7a59" },
      { name: "Heat Index", data: [], color: "#ffbb54" },
    ],
  });

  pressureChart.value = Highcharts.chart("chart-pressure", {
    chart: { type: "spline", backgroundColor: "#111a2b" },
    time: { timezone: "America/New_York" },
    title: { text: "Pressure", style: { color: "#fff" } },
    xAxis: { type: "datetime", labels: { style: { color: "#f1f5f9" } } },
    yAxis: { title: { text: pressUnit.value === "Pa" ? "Pa" : "atm", style: { color: "#f1f5f9" } }, labels: { style: { color: "#f1f5f9" } } },
    tooltip: { shared: true, backgroundColor: "#1b2434", style: { color: "#ffffff" } },
    legend: { itemStyle: { color: "#fff" } },
    series: [
      { name: "Pressure", data: [], color: "#66b2ff" },
    ],
  });

  altitudeChart.value = Highcharts.chart("chart-altitude", {
    chart: { type: "spline", backgroundColor: "#111a2b" },
    time: { timezone: "America/New_York" },
    title: { text: "Altitude", style: { color: "#fff" } },
    xAxis: { type: "datetime", labels: { style: { color: "#f1f5f9" } } },
    yAxis: { title: { text: altUnit.value === "ft" ? "ft" : "m", style: { color: "#f1f5f9" } }, labels: { style: { color: "#f1f5f9" } } },
    tooltip: { shared: true, backgroundColor: "#1b2434", style: { color: "#ffffff" } },
    legend: { itemStyle: { color: "#fff" } },
    series: [
      { name: "Altitude", data: [], color: "#9dff6b" },
    ],
  });

  humChart.value = Highcharts.chart("chart-humidity", {
    chart: { type: "spline", backgroundColor: "#111a2b" },
    time: { timezone: "America/New_York" },
    title: { text: "Humidity / Moisture", style: { color: "#fff" } },
    xAxis: { type: "datetime", labels: { style: { color: "#f1f5f9" } } },
    yAxis: { title: { text: "%", style: { color: "#f1f5f9" } }, labels: { style: { color: "#f1f5f9" } } },
    tooltip: { shared: true, backgroundColor: "#1b2434", style: { color: "#ffffff" } },
    legend: { itemStyle: { color: "#fff" } },
    series: [
      { name: "Humidity", data: [], color: "#58a6ff" },
      { name: "Moisture", data: [], color: "#90e0ef" },
    ],
  });
};

const refreshChartData = () => {
  const { tempSeries, hiSeries, pressureSeries, altitudeSeries, humiditySeries, moistureSeries } = aggregatedSeries();

  tempChart.value?.series[0].setData(tempSeries, false);
  tempChart.value?.series[1].setData(hiSeries, false);
  tempChart.value?.yAxis[0].setTitle({ text: tempUnit.value === "F" ? "°F" : "°C" });
  tempChart.value?.redraw();


  pressureChart.value?.series[0].setData(pressureSeries, false);
  pressureChart.value?.yAxis[0].setTitle({ text: pressUnit.value === "Pa" ? "Pa" : "atm" });
  pressureChart.value?.redraw();

  altitudeChart.value?.series[0].setData(altitudeSeries, false);
  altitudeChart.value?.yAxis[0].setTitle({ text: altUnit.value === "ft" ? "ft" : "m" });
  altitudeChart.value?.redraw();

  humChart.value?.series[0].setData(humiditySeries, false);
  humChart.value?.series[1].setData(moistureSeries, false);
  humChart.value?.redraw();
};

watch(payload, (newPayload) => {
  if (!newPayload?.timestamp) return;

  const entry = {
    timestamp: Number(newPayload.timestamp),
    temperature: Number(newPayload.temperature ?? 0),
    heat_index: Number(newPayload.heat_index ?? newPayload.heatindex ?? 0),
    humidity: Number(newPayload.humidity ?? 0),
    moisture: Number(newPayload.moisture ?? 0),
    pressure: Number(newPayload.pressure ?? 0),
    altitude: Number(newPayload.altitude ?? 0),
  };

  liveHistory.value.push(entry);
  if (liveHistory.value.length > 120) liveHistory.value.shift();

  refreshChartData();
}, { deep: true });

watch([tempUnit, altUnit, pressUnit], refreshChartData);

onMounted(() => {
  createCharts();
  Mqtt.connect();
  setTimeout(() => {
    Mqtt.subscribe("620172690");
  }, 1000);
});

onBeforeUnmount(() => {
  Mqtt.unsubcribeAll();
});
</script>

<style scoped>
.tile-card {
  margin-bottom: 12px;
  color: #e8ecf5;
  background: rgba(17, 26, 40, 0.85);
  border: 1px solid rgba(255, 255, 255, 0.1);
  backdrop-filter: blur(8px);
  min-height: 110px;
}

.unit-card {
  background: rgba(20, 30, 50, 0.9);
  border: 1px solid rgba(110, 140, 170, 0.2);
  color: #e8ecf5;
}

.chart-box {
  min-height: 300px;
  width: 100%;
  border-radius: 14px;
  border: 1px solid rgba(180, 210, 255, 0.3);
  background-color: #101b2c;
}

.live-chart {
  margin: 0;
}

.text-secondary {
  color: #aac3e1;
}
</style>