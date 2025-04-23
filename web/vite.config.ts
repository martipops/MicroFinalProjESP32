import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import { viteSingleFile } from 'vite-plugin-singlefile'

const SINGLE_FILE_CONFIG = { 
  removeViteModuleLoader: true 
}

// https://vite.dev/config/
export default defineConfig({
  plugins: [vue(), viteSingleFile(SINGLE_FILE_CONFIG)],
  build: {
    outDir: 'dist'
  },
})