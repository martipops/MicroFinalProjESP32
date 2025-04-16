<template>
  <div class="blend">
    <JSCharting :style="chartStyle" :options="chartOptions"></JSCharting>
  </div>
</template>

<script lang="ts">
import type { JSCChartConfig } from 'jscharting';
import JSCharting from 'jscharting-vue';

export default {
  name: 'GraphDisplay',
  components: {
    JSCharting
  },
  props: {
    modelValue: {
      type: Array,
      default: () => [12, 15, 20, 25, 30]
    },
    width: {
      type: String,
      default: () => '100%'
    },
    height: {
      type: String,
      default: () => '30rem'
    },
    title: {
      type: String,
      default: () => 'Sensor Readings'
    }
  },
  data() {
    return {
      chartOptions: {
        type: 'line',
        title: { label: { text: this.title } },
        legend: {
          visible: false,
        },
        xAxis: {
          visible: false,
          label: { text: 'Time' }
        },
        yAxis: {
          label: { text: 'Value' }
        },
        series: [
          {
            name: 'Readings',
            points: this.formattedPoints
          }
        ]
      } as JSCChartConfig
    };
  },
  methods: {
    add() {
      const newmodelValue = this.modelValue
      newmodelValue.push(123)
      this.$emit('update:modelValue', newmodelValue)
    }
  },
  computed: {
    formattedPoints() {
      return this.modelValue.map((value, index) => {
        return {
          id: `p${index}`,
          x: index,
          y: value
        };
      });
    },
    chartStyle(){
      return `width: ${this.width} ; height:${this.height}`;
    } 
  },
  watch: {
    formattedPoints: {
      handler(newPoints) {
        this.chartOptions.series = [
          {
            name: 'Readings',
            points: newPoints
          }
        ];
      },
      deep: true
    }
  },
  mounted() {
    if (this.chartOptions.series) {
      this.chartOptions.series[0].points = this.formattedPoints;
    }
  }
};
</script>

<style scoped>

.blend {
  mix-blend-mode: screen;
  filter: invert(1);
  width: 100%;
}

</style>