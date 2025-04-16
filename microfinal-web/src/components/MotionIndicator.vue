<template>
  <div class="container">
    <h3>Motion</h3>
    <div class="circle" :class="getMotionColor">
    </div>
  </div>
</template>

<script lang="ts">

export default {
  name: 'MotionIndicator',
  props: {
    motion: {
      type: Array as () => boolean[],
      default: [false]
    }
  },
  computed: {
    getMotionColor() {
      const g = this.motion.some((m) => m);
      return g ? 'on' : 'off';
    }
  }
}


</script>

<style scoped>

.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 2rem;
}

.circle {
  width: 100px;
  height: 100px;
  border-radius: 100%;
  border: 2px solid white;
}

.off {
  background-color: black;
}

.on {
  background-color: red;
}

</style>