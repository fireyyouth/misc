
<script>
  import LoginForm from './components/LoginForm.vue'
  import ChatPanel from './components/ChatPanel.vue'
  export default {
    data() {
      return {
        current_panel: 'LoginForm'
      }
    },
    methods: {
      set_chat_panel() {
          console.log("setting chat panel")
          this.current_panel = 'ChatPanel'
      }
    },
    components: {
      LoginForm,
      ChatPanel
    }
  }
</script>

<template>
  <div class="app">
    <login-form v-if="current_panel == 'LoginForm'" @auth-success="set_chat_panel"></login-form>
    <chat-panel v-else></chat-panel>
  </div>
</template>

<style scoped>
  .app {
      box-sizing: border-box;
      width: 100%;
      height: 100%;
      padding: 0.5em;
  }
</style>
