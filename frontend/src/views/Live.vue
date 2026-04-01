<template>
  <v-container bg-color="surface" fluid align="center">
    <!--First Row-->
    <v-row style="max-width: 1200px">
      <!--First Column-->
      <v-col cols="9">
        <figure class="highcharts-figure">
          <div id="container"></div>
        </figure>
      </v-col>
      <!--Second Column-->
      <v-col cols="3">
        <!--First Card-->
        <v-card
          class="mb-5"
          max-width="500"
          color="primaryContainer"
          subtitle="Temperature"
        >
          <v-card-item>
            <span class="text-h3 text-onPrimaryContainer">{{
              temperature
            }}</span>
          </v-card-item>
        </v-card>
        <!--Second Card-->
        <v-card
          class="mb-5"
          max-width="500"
          color="tertiaryContainer"
          subtitle="Heat Index (Feels like)"
        >
          <v-card-item>
            <span class="text-h3 text-onTertiaryContainer">{{
              heatIndex
            }}</span>
          </v-card-item>
        </v-card>
        <!--Third Card-->
        <v-card
          class="mb-5"
          max-width="500"
          color="secondaryContainer"
          subtitle="Humidity"
        >
          <v-card-item>
            <span class="text-h3 text-onSecondaryContainer">{{
              humidity
            }}</span>
          </v-card-item>
        </v-card>
      </v-col>
    </v-row>

    <!--Second Row-->
    <v-row style="max-width: 1200px" justify="start">
      <v-col cols="9">
        <figure class="highcharts-figure">
          <div id="container1"></div>
        </figure>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import {
  ref,
  reactive,
  watch,
  onMounted,
  onBeforeUnmount,
  computed,
} from "vue";
import { useRoute, useRouter } from "vue-router";
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { useAppStore } from "@/store/appStore";
import { storeToRefs } from "pinia";

// Highcharts, Load the exporting module and Initialize exporting module.
import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";
Exporting(Highcharts);
more(Highcharts);

// VARIABLES
const router = useRouter();
const route = useRoute();
const Mqtt = useMqttStore();
const AppStore = useAppStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);
const tempHiChart = ref(null); // Chart object
const humidityChart = ref(null); // Chart object
const points = ref(10); // Specify the quantity of points to be shown on the live graph simultaneously.
const shift = ref(false); // Delete a point from the left side and append a new point to the right side of the graph.

const CreateCharts = async () => {
  // TEMPERATURE CHART
  tempHiChart.value = Highcharts.chart("container", {
    chart: { zoomType: "x", backgroundColor: "#1e1e1e" },
    title: {
      text: "Temperature Analysis (Live)",
      align: "left",
      style: { color: "#FFFFFF" },
    },
    yAxis: {
      title: {
        text: "°C",
        style: { color: "#FFFFFF" },
      },
      labels: { format: "{value} °C", style: { color: "#FFFFFF" } },
      gridLineColor: "#444444",
    },
    xAxis: {
      type: "datetime",
      title: { text: "", style: { color: "#FFFFFF" } },
      labels: { style: { color: "#FFFFFF" } },
      gridLineColor: "#444444",
    },
    tooltip: {
      shared: true,
      backgroundColor: "#2b2b2b",
      style: { color: "#FFFFFF" },
    },
    legend: { itemStyle: { color: "#FFFFFF" } },
    series: [
      {
        name: "Temperature",
        type: "spline",
        data: [],
        turboThreshold: 0,
        color: "#FF9999",
        lineWidth: 2,
      },
      {
        name: "Heat Index",
        type: "spline",
        data: [],
        turboThreshold: 0,
        color: "#FFC0CB",
        lineWidth: 2,
      },
    ],
  });

  // HUMIDITY CHART
  humidityChart.value = Highcharts.chart("container1", {
    chart: {
      zoomType: "x",
      backgroundColor: "#1e1e1e",
    },
    title: {
      text: "Humidity Analysis (Live)",
      align: "left",
      style: { color: "#FFFFFF" },
    },
    yAxis: {
      title: {
        text: "Humidity (%)",
        style: { color: "#FFFFFF" },
      },
      labels: {
        format: "{value}%",
        style: { color: "#FFFFFF" },
      },
      gridLineColor: "#444444",
    },
    xAxis: {
      type: "datetime",
      title: {
        text: "",
        style: { color: "#FFFFFF" },
      },
      labels: { style: { color: "#FFFFFF" } },
      gridLineColor: "#444444",
    },
    tooltip: {
      shared: true,
      backgroundColor: "#2b2b2b",
      style: { color: "#FFFFFF" },
    },
    legend: {
      itemStyle: { color: "#FFFFFF" },
    },
    series: [
      {
        name: "Humidity",
        type: "spline",
        data: [],
        turboThreshold: 0,
        color: "#bfa7ff",
        lineWidth: 2,
      },
    ],
  });
};

const updateLineCharts = async () => {
  if (!!start.value && !!end.value) {
    // Convert output from Textfield components to 10 digit timestamps
    let startDate = new Date(start.value).getTime() / 1000;
    let endDate = new Date(end.value).getTime() / 1000;
    // Fetch data from backend
    const data = await AppStore.getAllInRange(startDate, endDate);
    // Create arrays for each plot
    let temperature = [];
    let heatindex = [];
    let humidity = [];
    // Iterate through data variable and transform object to format recognized by highcharts
    data.forEach((row) => {
      temperature.push({
        x: row.timestamp * 1000,
        y: parseFloat(row.temperature.toFixed(2)),
      });
      heatindex.push({
        x: row.timestamp * 1000,
        y: parseFloat(row.heatindex.toFixed(2)),
      });
      humidity.push({
        x: row.timestamp * 1000,
        y: parseFloat(row.humidity.toFixed(2)),
      });
    });
    // Add data to Temperature and Heat Index chart
    tempHiChart.value.series[0].setData(temperature);
    tempHiChart.value.series[1].setData(heatindex);
    humidityChart.value.series[0].setData(humidity);
  }
};

// COMPUTED PROPERTIES
const temperature = computed(() => {
  if (!!payload.value) {
    return `${payload.value.temperature.toFixed(2)} °C`;
  }
});

const heatIndex = computed(() => {
  if (!!payload.value) {
    return `${payload.value.heatindex.toFixed(2)} °C`;
  }
});

const humidity = computed(() => {
  if (!!payload.value) {
    return `${payload.value.humidity.toFixed(2)} %`;
  }
});

// FUNCTIONS
onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  CreateCharts();
  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620172690");
    //Mqtt.subscribe("topic2");
  }, 3000);
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubscribeAll();
});

// WATCHERS
watch(payload, (data) => {
  if (points.value > 0) {
    points.value--;
  } else {
    shift.value = true;
  }
  tempHiChart.value.series[0].addPoint(
    { y: parseFloat(data.temperature.toFixed(2)), x: data.timestamp * 1000 },
    true,
    shift.value,
  );
  tempHiChart.value.series[1].addPoint(
    { y: parseFloat(data.heatindex.toFixed(2)), x: data.timestamp * 1000 },
    true,
    shift.value,
  );
  humidityChart.value.series[0].addPoint(
    { y: parseFloat(data.humidity.toFixed(2)), x: data.timestamp * 1000 },
    true,
    shift.value,
  );
});
</script>

<style scoped>
/** CSS STYLE HERE */
Figure {
  border: 2px solid black;
}
</style>