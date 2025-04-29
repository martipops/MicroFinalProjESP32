<template>
  <div class="flex flex-col items-center justify-center h-screen w-screen">
    <TrashCan :fillPercentage="trashFillLevel ?? 0" />
    <div class="flex mb-2 gap-12">
      <div>
        <label class="mr-4 text-lg">Refresh Interval (ms):</label>
        <input
          type="number"
          v-model="refreshInterval"
          class="p-2 border border-gray-300 rounded"
          placeholder="Set refresh interval (ms)"
          @change="saveSettings"
        />
      </div>
      <div>
        <label class="mr-4 text-lg">Can Height (cm):</label>
        <input
          type="number"
          v-model="canHeight"
          class="p-2 border border-gray-300 rounded"
          placeholder="Set can height (cm)"
          @change="saveSettings"
        />
        <button
          class="ml-4 p-2 bg-blue-500 text-white rounded"
          @click="measureCan"
        >
          Measure Can
        </button>
      </div>
    </div>

  </div>
</template>

<script lang="ts">
import { defineComponent } from 'vue';
import TrashCan from './components/TrashCan.vue';

export default defineComponent({
  name: 'App',
  components: {
    TrashCan
  },
  data() {
    return {
      loraMessages: ['112,112,112,112','113,113,13,113'] as string[],
      refreshInterval: 1000,
      timer: null as any,
      canHeight: 100
    };
  },
  
  methods: {
    async fetchMessages() {
      try {
        const response = await fetch('/read');
        if (!response.ok) {
          throw new Error(`HTTP error: ${response.status}`);
        }
        const data = await response.json();
        if (data.messages){
          this.loraMessages = data.messages
        }
        console.log('Messages updated:', this.loraMessages);
        // await fetch('/clear');
      } catch (error) {
        console.error('Error fetching messages:', error);
      }
    },
    
    startPolling() {
      // Clear any existing timer
      if (this.timer) {
        clearInterval(this.timer);
      }
      
      // Start a new timer with the current interval
      this.timer = setInterval(() => this.fetchMessages(), this.refreshInterval);
      console.log(`Polling started with ${this.refreshInterval}ms interval`);
    },
    
    updateInterval() {
      this.startPolling();
      this.saveSettings();
    },
    
    saveSettings() {
      localStorage.setItem('refreshInterval', this.refreshInterval.toString());
      localStorage.setItem('canHeight', this.canHeight.toString());
      console.log('Settings saved to localStorage');
      
      // Also update polling if refresh interval was changed
      if (this.timer) {
        this.startPolling();
      }
    },
    
    loadSettings() {
      const savedRefreshInterval = localStorage.getItem('refreshInterval');
      const savedCanHeight = localStorage.getItem('canHeight');
      
      if (savedRefreshInterval) {
        this.refreshInterval = parseInt(savedRefreshInterval);
      }
      
      if (savedCanHeight) {
        this.canHeight = parseInt(savedCanHeight);
      }
      
      console.log('Settings loaded from localStorage', { 
        refreshInterval: this.refreshInterval, 
        canHeight: this.canHeight 
      });
    },

    measureCan() {
      this.canHeight = this.measuredLevel;
    }
  },

  computed: {
    trashFillLevel() {
      const fillLevel = ((this.canHeight - this.measuredLevel) / this.canHeight) * 100;
      return Math.min(100, Math.max(0, Math.round(fillLevel)));
    },
    measuredLevel() {
      if (this.loraMessages.length > 0) {
        return parseInt(this.loraMessages[0].split(',')[2]) as number;
      }
      return 0;
    }
  },
  
  mounted() {
    // Load saved settings first
    this.loadSettings();
    
    // Initial fetch
    this.fetchMessages();
    
    // Start continuous polling
    this.startPolling();
  },
  
  beforeDestroy() {
    if (this.timer) {
      clearInterval(this.timer);
    }
  }
});
</script>