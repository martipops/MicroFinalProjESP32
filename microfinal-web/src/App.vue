<template>
  <div class="app">
    <div class="center">
      <h1>Invisible Intrusion Detector</h1>
    </div>
    <MotionIndicator :motion="motion"/>
    <div class="graphs">
      <Graph 
        v-model="gas" 
        title="Gas Levels" 
        :threshold="gasThreshold"
        class="above"
        :class="isGasAlertActive ? `above` : `below`" 
      />
      <Graph 
        v-model="sound" 
        title="Sound Levels" 
        :threshold="soundThreshold"
        :class="isSoundAlertActive ? `above` : `below`" 
      />
    </div>
    
    <div class="controls">
      <button @click="toggleSettings" class="settings-toggle">
        {{ showSettings ? 'Hide Settings' : 'Show Settings' }}
      </button>
      <button @click="downloadCSV" class="download-btn">Download CSV</button>
    </div>
    
    <div v-if="showSettings" class="settings-panel">
      <h3>Settings</h3>
      <div class="settings-group">
        <div class="setting-item">
          <label>Data Interval (ms):</label>
          <input 
            type="number" 
            v-model="newInterval" 
            min="50"
            placeholder="Interval (ms)"
          />
          <button @click="updateInterval">Update</button>
          <span v-if="intervalStatus" :class="intervalStatus.success ? 'success' : 'error'">
            {{ intervalStatus.message }}
          </span>
        </div>
        
        <div class="setting-item">
          <label>Gas Threshold:</label>
          <input 
            type="number" 
            v-model.number="gasThreshold" 
            min="0"
            max="100"
          />
        </div>
        
        <div class="setting-item">
          <label>Sound Threshold:</label>
          <input 
            type="number" 
            v-model.number="soundThreshold" 
            min="0"
            max="100"
          />
        </div>
      </div>
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
      gasThreshold: 1720,
      soundThreshold: 150,
      showSettings: false,
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
    },
    isGasAlertActive() {
      if (!this.sensorData?.gas || this.sensorData.gas.length === 0) return false;
      const g=  Math.max(...this.sensorData.gas) > this.gasThreshold;
      console.log(g)
      return g
    },
    isSoundAlertActive() {
      if (!this.sensorData?.sound || this.sensorData.sound.length === 0) return false;
      return Math.max(...this.sensorData.sound) > this.soundThreshold;
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
          console.error('Error fetching sensor data:', error);
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
    downloadCSV() {
      if (!this.sensorData) return;
      
      // Create CSV header
      let csv = 'Index,Motion,Gas,Sound\n';
      
      // Add each row of data
      const length = Math.max(
        this.sensorData.motion.length,
        this.sensorData.gas.length,
        this.sensorData.sound.length
      );
      
      for (let i = 0; i < length; i++) {
        const motion = this.sensorData.motion[i] ? 1 : 0;
        const gas = this.sensorData.gas[i] || 0;
        const sound = this.sensorData.sound[i] || 0;
        csv += `${i},${motion},${gas},${sound}\n`;
      }
      
      // Create download link
      const blob = new Blob([csv], { type: 'text/csv' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `sensor-data-${new Date().toISOString().slice(0,19).replace(/:/g,'-')}.csv`;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
    },
    toggleSettings() {
      this.showSettings = !this.showSettings;
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
  margin-bottom: 1.5rem;
}

.controls {
  margin-top: 20px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.interval-control {
  display: flex;
  align-items: center;
  gap: 10px;
}

input {
  padding: 8px;
  border: 1px solid #ccc;
  border-radius: 4px;
  width: 120px;
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

.download-btn {
  background-color: #2196F3;
}

.download-btn:hover {
  background-color: #0b7dda;
}

.settings-toggle {
  background-color: #607d8b;
}

.settings-toggle:hover {
  background-color: #455a64;
}

.settings-panel {
  margin-top: 20px;
  padding: 15px;
  border-radius: 4px;
}

.settings-group {
  display: flex;
  flex-direction: column;
  gap: 15px;
}

.setting-item {
  display: flex;
  align-items: center;
  gap: 10px;
}

.setting-item label {
  min-width: 140px;
  font-weight: bold;
}

.success {
  color: #4CAF50;
}

.error {
  color: #f44336;
}

.above {
  border: 2px solid #00f7ff;
}

.below {
  border: 2px solid #d75edb;
}

</style>