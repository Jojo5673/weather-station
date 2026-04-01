import { defineStore } from "pinia";

export const useAppStore = defineStore(
  "app",
  () => {
    /*  
    The composition API way of defining a Pinia store
    ref() s become state properties
    computed() s become getters
    function() s become actions  
    */

    // STATES

    // ACTIONS
    const getAllInRange = async (start, end) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/climo/get/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "found") {
              // console.log(data["data"] )
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("getAllInRange returned no data");
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
        }
      } catch (err) {
        console.error("getAllInRange error: ", err.message);
      }
      return [];
    };

    const getTemperatureMMAR = async (start, end) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/mmar/temperature/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "found") {
              // console.log(data["data"] )
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("getTemperatureMMAR returned no data");
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
        }
      } catch (err) {
        console.error("getTemperatureMMAR error: ", err.message);
      }
      return [];
    };

    const getHumidityMMAR = async (start, end) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/mmar/humidity/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "found") {
              // console.log(data["data"] )
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("getHumidityMMAR returned no data");
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
        }
      } catch (err) {
        console.error("getHumidityMMAR error: ", err.message);
      }
      return [];
    };

    const getFreqDistro = async (variable, start, end) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/frequency/${variable}/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "found") {
              // console.log(data["data"] )
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("getFreqDistro returned no data");
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
        }
      } catch (err) {
        console.error("getFreqDistro error: ", err.message);
      }
      return [];
    };

    const updateUnits = async (units) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/station/units`;
      try {
        const response = await fetch(URL, { method: "POST", headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(units), signal: signal });
        if (response.ok) {
          const data = await response.json();
          return data;
        } else {
          const data = await response.text();
          console.log(data);
          return { status: 'error', message: data };
        }
      } catch (err) {
        console.error("updateUnits error: ", err.message);
        return { status: 'error', message: err.message };
      }
    };

    const getAggregatedStats = async (start, end) => {
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => controller.abort(), 60000);
      const URL = `/api/analysis/stats/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          return data;
        } else {
          const data = await response.text();
          console.log(data);
          return { status: 'error', message: data };
        }
      } catch (err) {
        console.error("getAggregatedStats error: ", err.message);
        return { status: 'error', message: err.message };
      }
    };

    const getTrendLines = async (start, end, granularity) => {
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => controller.abort(), 60000);
      const URL = `/api/analysis/trends/${start}/${end}/${granularity}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          return data;
        } else {
          const data = await response.text();
          console.log(data);
          return { status: 'error', message: data };
        }
      } catch (err) {
        console.error("getTrendLines error: ", err.message);
        return { status: 'error', message: err.message };
      }
    };

    const getFrequencyDistribution = async (variable, start, end) => {
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => controller.abort(), 60000);
      const URL = `/api/analysis/frequency/${variable}/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          return data;
        } else {
          const data = await response.text();
          console.log(data);
          return { status: 'error', message: data };
        }
      } catch (err) {
        console.error("getFrequencyDistribution error: ", err.message);
        return { status: 'error', message: err.message };
      }
    };

    const getScatterPlotData = async (start, end) => {
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => controller.abort(), 60000);
      const URL = `/api/analysis/scatter/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          return data;
        } else {
          const data = await response.text();
          console.log(data);
          return { status: 'error', message: data };
        }
      } catch (err) {
        console.error("getScatterPlotData error: ", err.message);
        return { status: 'error', message: err.message };
      }
    };

    const getHeatStressEvents = async (start, end, threshold = 32) => {
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => controller.abort(), 60000);
      const URL = `/api/analysis/heat-stress/${start}/${end}/${threshold}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          return data;
        } else {
          const data = await response.text();
          console.log(data);
          return { status: 'error', message: data };
        }
      } catch (err) {
        console.error("getHeatStressEvents error: ", err.message);
        return { status: 'error', message: err.message };
      }
    };

    return {
      // EXPORTS
      getAllInRange,
      getTemperatureMMAR,
      getHumidityMMAR,
      getFreqDistro,
      updateUnits,
      getAggregatedStats,
      getTrendLines,
      getFrequencyDistribution,
      getScatterPlotData,
      getHeatStressEvents,
    };
  },
  { persist: true },
);