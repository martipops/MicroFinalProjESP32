<template>
  <div>
    <h1>LoRa Messages</h1>
    <ul>
      <li v-for="(message, index) in messages" :key="index">{{ message }}</li>
    </ul>
    <div v-if="loading">Loading...</div>
    <div v-if="error">{{ error }}</div>
  </div>
</template>

<script lang="ts">

const REFRESH_INTERVAL = 1000 // 2 seconds

export default {
  name: 'App',
  data() {
    return {
      messages: [] as string[],
      loading: false,
      error: null as string | null,
      timer: undefined as number | undefined
    }
  },
  mounted() {
    this.fetchMessages()
    // Check for new messages every 2 seconds
    this.timer = setInterval(this.fetchMessages, REFRESH_INTERVAL)
  },
  
  beforeUnmount() {
    clearInterval(this.timer)
  },
  
  methods: {
    async fetchMessages() {
      this.loading = true
      try {
        // Get messages
        const response = await fetch('/read')
        const data = await response.json()
        
        // If we got messages, save them and clear the buffer
        if (data.messages && data.messages.length > 0) {
          this.messages = [...this.messages, ...data.messages]
          fetch('/clear') // No need to await this
        }
      } catch (err) {
        this.error = "Failed to load messages"
        console.error(err)
      } finally {
        this.loading = false
      }
    }
  }
}
</script>

<style scoped>
ul {
  list-style-type: none;
  padding: 0;
}
li {
  padding: 10px;
  margin: 5px;
  background-color: #f5f5f5;
  border-left: 4px solid #42b983;
}
</style>