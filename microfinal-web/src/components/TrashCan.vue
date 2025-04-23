

<template>
  <div class="flex flex-col items-center justify-center w-full h-screen bg-gradient-to-b">
    <div :class="['trash-can-container relative', containerClass]">
      <!-- Status message -->
      <div class="absolute -top-36 left-0 right-0 text-center">
        <span class="text-4xl font-bold" :class="{
          'text-green-600': fillPercentage < 30,
          'text-yellow-600': fillPercentage >= 30 && fillPercentage < 60,
          'text-orange-600': fillPercentage >= 60 && fillPercentage < 80,
          'text-red-600': fillPercentage >= 80
        }">{{ statusMessage }}</span>
      </div>
      
      <!-- Percentage display -->
      <div class="absolute -bottom-24 left-0 right-0 text-center">
        <span class="text-6xl font-bold text-gray-700">{{ fillPercentage }}%</span>
      </div>
      
      <!-- Trash can -->
      <div class="relative w-64 h-96">
        <!-- Trash can lid -->
        <div class="absolute -top-8 -left-6 w-[300px] h-12 bg-gray-700 rounded-md shadow-lg z-20"></div>
        
        <!-- Trash can handle -->
        <div class="absolute -top-16 left-1/2 transform -translate-x-1/2 w-16 h-8 border-6 border-gray-700 rounded-t-full"></div>
        
        <!-- Trash can body -->
        <div class="absolute top-4 left-0 w-full h-full border-8 border-gray-700 border-t-0 rounded-b-2xl overflow-hidden bg-white shadow-2xl z-10">
          <!-- Fill pattern - small diagonal lines -->
          <div class="absolute inset-0 opacity-10">
            <div class="w-full h-full bg-stripe"></div>
          </div>
          
          <!-- Trash fill - dynamic height based on fillPercentage -->
          <div 
            :class="['absolute bottom-0 w-full transition-all duration-700 ease-in-out', fillColor]"
            :style="{ height: `${fillPercentage}%` }">
            <!-- Some garbage icons that show up when filling -->
            <div v-if="fillPercentage > 10" class="absolute top-1/4 left-1/4 text-3xl opacity-40">🍕</div>
            <div v-if="fillPercentage > 20" class="absolute top-1/3 right-1/4 text-2xl opacity-30">📄</div>
            <div v-if="fillPercentage > 40" class="absolute top-1/2 left-1/3 text-3xl opacity-50">🥫</div>
            <div v-if="fillPercentage > 60" class="absolute top-3/4 right-1/3 text-2xl opacity-40">🍎</div>
            <div v-if="fillPercentage > 80" class="absolute bottom-1/4 left-1/2 text-3xl opacity-60">📦</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent } from 'vue';


export default defineComponent({
  name: 'TrashCan',
  props: {
    fillPercentage: {
      type: Number,
      required: true,
      validator: (value: number) => value >= 0 && value <= 100
    }
  },
  computed: {
    // Dynamically calculate color based on fill level
    fillColor() {
      if (this.fillPercentage < 30) return 'bg-green-500';
      if (this.fillPercentage < 60) return 'bg-yellow-500';
      if (this.fillPercentage < 80) return 'bg-orange-500';
      return 'bg-red-500';
    },
    
    // Compute status message based on fill level
    statusMessage() {
      if (this.fillPercentage < 30) return 'Looking good!';
      if (this.fillPercentage < 60) return 'Getting full...';
      if (this.fillPercentage < 80) return 'Nearly full!';
      return 'Time to empty!';
    },
    
    // Add a slight shake animation at high levels
    containerClass() {
      return this.fillPercentage >= 90 ? 'animate-shake' : '';
    }
  }
});
</script>

<style>
.border-6 {
  border-width: 6px;
}

.border-8 {
  border-width: 8px;
}

.bg-stripe {
  background: repeating-linear-gradient(
    45deg,
    rgba(0, 0, 0, 0.05),
    rgba(0, 0, 0, 0.05) 10px,
    transparent 10px,
    transparent 20px
  );
}

@keyframes shake {
  0% { transform: translateX(0); }
  25% { transform: translateX(-5px); }
  50% { transform: translateX(5px); }
  75% { transform: translateX(-5px); }
  100% { transform: translateX(0); }
}

.animate-shake {
  animation: shake 0.5s ease-in-out infinite;
}
</style>