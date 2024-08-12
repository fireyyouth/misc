<script>
    export default {
        props: ['prop_messages'],
        updated() {
            this.$refs.view.scrollTop = this.$refs.view.scrollHeight;
        },
        mounted() { // fixme: no effect
            this.$refs.view.scrollTop = this.$refs.view.scrollHeight;
        }
    }
</script>

<template>
    <div class="output" ref="view">
        <div v-for="msg in prop_messages" class="message">
            {{ msg.date }}
            <div class="author_content">
                <div>
                    {{ msg.author }}:
                </div>
                <div class="bubble">
                    {{ msg.content }}
                </div>
            </div>
        </div>
    </div>
</template>

<style scoped>
    .bubble {
        border-radius: 0.5em;
        background-color: aquamarine;
        padding: 0.5em;
    }

    .message {
        padding: 1em;
    }

    .author_content {
        display: flex;
        flex-direction: column;
    }

    .output {
        border: lightblue solid 2px;
        overflow: scroll;
    }
</style>