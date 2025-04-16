<template>
  <div class="app">
    <div class="center">
      <h1>Invisible Intrusion Detector</h1>
    </div>
    <MotionIndicator :motion="motion"/>
    <div class="graphs">
      <Graph v-model="gas" title="Gas Levels"/>
      <Graph v-model="sound" title="Sound Levels"/>
    </div>
    <div class="interval-control">
      <input 
      type="number" 
      v-model="newInterval" 
      min="50"
      placeholder="Interval (ms)"
      />
      <button @click="updateInterval">Update Interval</button>
      <span v-if="intervalStatus" :class="intervalStatus.success ? 'success' : 'error'">
        {{ intervalStatus.message }}
      </span>
    </div>
  </div>
</template>

<script lang="ts">

import Graph from './components/Graph.vue';
import MotionIndicator from './components/MotionIndicator.vue';

export type SensorData = {
  motion: boolean[];
  sound: number[];
  gas: number[];
}
const DATA_INTERVAL = 100;

export default {
  name: 'App',
  components: {
    Graph,
    MotionIndicator
  },
  setup() {
    // Setup logic can go here
  },
  data() {
    return {
      sensorData: null as SensorData | null,
      dataInterval: DATA_INTERVAL,
      newInterval: DATA_INTERVAL,
      intervalStatus: null as {success: boolean, message: string} | null,
      intervalTimer: null as number | null,
    }
  },
  computed: {
    motion: {
      get() {
        return this.sensorData?.motion || [false,false,false,false,false];
      },
      set(value: boolean[]) {
        if (this.sensorData) {
          this.sensorData.motion = value;
        }
      }
    },
    gas: {
      get() {
        return this.sensorData?.gas || [0, 0, 0, 0, 0];
      },
      set(value: number[]) {
        if (this.sensorData) {
          this.sensorData.gas = value;
        }
      }
    },
    sound: {
      get() {
        return this.sensorData?.sound || [0, 0, 0, 0, 0];
      },
      set(value: number[]) {
        if (this.sensorData) {
          this.sensorData.sound = value;
        }
      }
    }
  },
  methods: {
    fetchSensorData() {
      // Fetch sensor data from the server
      fetch('/read')
        .then(response => response.json())
        .then(data => {
          this.sensorData = data;
        })
        .catch(error => {
          console.error('Error fetching  ensor data:', error);
        });
    },
    fetchSensorDataTemp() {
      for (let i = 0; i < 30; i++) {
        if (this.sensorData && this.sensorData?.gas && this.sensorData?.sound) {
          this.sensorData.sound[i] = Math.floor(Math.random() * 100);
          this.sensorData.gas[i] = Math.floor(Math.random() * 100);
        }
      }
    },
    updateInterval() {
      const interval = parseInt(this.newInterval.toString());
      if (isNaN(interval) || interval < 50) {
        this.intervalStatus = { success: false, message: 'Interval must be at least 50ms' };
        return;
      }
      
      // Update the interval on the ESP32
      fetch('/api/interval', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: `value=${interval}`
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          this.dataInterval = interval;
          this.intervalStatus = { success: true, message: `Interval updated to ${interval}ms` };
          
          // Reset the polling interval
          if (this.intervalTimer) {
            clearInterval(this.intervalTimer);
          }
          this.intervalTimer = setInterval(() => {
            this.fetchSensorData();
          }, this.dataInterval);
        } else {
          this.intervalStatus = { success: false, message: data.message || 'Failed to update interval' };
        }
      })
      .catch(error => {
        console.error('Error updating interval:', error);
        this.intervalStatus = { success: false, message: 'Error updating interval' };
      });
    }
  },
  mounted() {
    // every x seconds, fetch the sensor data
    this.sensorData = {
      motion: [false, false, false, false, false],
      sound: [0, 0, 0, 0, 0],
      gas: [0, 0, 0, 0, 0]
    }
    this.fetchSensorData();
    this.intervalTimer = setInterval(() => {
      this.fetchSensorData();
    }, this.dataInterval);
  },
  beforeUnmount() {
    // Clear the interval when component is destroyed
    if (this.intervalTimer) {
      clearInterval(this.intervalTimer);
    }
  }
}
</script>

<style scoped>

.mr-4 {
  margin-bottom: 4rem;
}

.center {
  display: flex;
  justify-content: center;
  justify-items: center;
}

.app {
  padding: 2rem;
}

.graphs {
  display: flex;
  gap: 1rem;
  justify-content: space-between;
}

.interval-control {
  margin: 20px 0;
  display: flex;
  align-items: center;
  gap: 10px;
}

input {
  padding: 8px;
  border: 1px solid #ccc;
  border-radius: 4px;
  width: 150px;
}

button {
  padding: 8px 16px;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background-color: #45a049;
}

.success {
  color: #4CAF50;
}

.error {
  color: #f44336;
}
</style>