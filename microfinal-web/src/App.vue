<template>
  <div>
    <pre>
      {{ sensorData }}
    </pre>
  </div>
  <Graph v-model="testModel"/>
</template>


<script lang="ts">
import Graph from './components/Graph.vue';



export interface SensorData {
  motion: boolean;
  sound: number;
  gas: number;
}

const DATA_INTERVAL = 1000;

export default {
  name: 'App',
  components: {
    Graph
  },
  setup() {
    // Setup logic can go here
  },
  data() {
    return {
      sensorData: null as SensorData | null,
      testModel: [12, 15, 20, 25, 30]
    }
  },
  methods: {
    fetchSensorData() {
      // Fetch sensor data from the server
      fetch('/sensor-data')
        .then(response => response.json())
        .then(data => {
          this.sensorData = data;
        })
        .catch(error => {
          console.error('Error fetching sensor data:', error);
        });
    },
    fetchSensorDataTemp() {
      this.sensorData = {
        motion: Math.random() > 0.5,
        sound: Math.floor(Math.random() * 100),
        gas: Math.floor(Math.random() * 100)
      };
      
    }
  },
  mounted() {
    // every x seconds, fetch the sensor data
    setInterval(() => {
      this.fetchSensorDataTemp();
    }, DATA_INTERVAL);
  }
}
</script>

<style scoped>
</style>