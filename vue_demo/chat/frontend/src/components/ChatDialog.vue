<script>
    import OutputView from './OutputView.vue'

    class Message {
        constructor(author, date, content) {
            this.date = date
            this.author = author
            this.content = content
        }
    }

    export default {
        props: ['prop_messages', 'prop_name'],
        data() {
            return {
                event_source: this.init_event_source(),
                messages: this.init_messages(),
                stage: ''
            }
        },
        components: {
            OutputView
        },
        methods: {
            init_event_source() {
                if (this.prop_name == 'public') {
                    let event_source = new EventSource('/message_events')
                    let this_component = this;
                    event_source.onmessage = (e) => {
                        let message_json = JSON.parse(e.data)
                        let message = new Message(message_json.username, (new Date()).toUTCString(), message_json.content)
                        this_component.messages.push(message);
                    }
                    event_source.onerror = (e) => {
                        let message = new Message('[system]', (new Date()).toUTCString(), e.type)
                        this_component.messages.push(message);
                    }
                    return event_source
                } else {
                    return null
                }
            },
            init_messages() {
                return this.prop_messages.map((s) => new Message(this.prop_name, (new Date()).toUTCString(), s))
            },
            async submit() {
                if (this.prop_name == 'public') {
                    let rsp = await fetch('/send_message', {
                        method: 'POST',
                        headers: {
                            "Content-Type": "application/json",
                        },
                        body: JSON.stringify({
                            'content': this.stage
                        })
                    })
                    this.stage = ''
                    if (rsp.status != 200) {
                        let body = await rsp.text()
                        this.messages.push(new Message('[system]', (new Date()).toUTCString(), `${rsp.status}:${body}`));
                    }
                } else {
                    this.messages.push(new Message('me', (new Date()).toUTCString(), this.stage));
                    this.stage = ''
                }
            }
        }
    }
</script>

<template>
    <div class="chat-dialog">
        <output-view class="output-view" :prop_messages="messages"></output-view>
        <div class="input">
            <textarea v-model="stage" class="stage" @keyup.enter="submit"></textarea>
            <button @click="submit">send</button>
        </div>
    </div>
</template>

<style scoped>
    .chat-dialog {
        display: flex;
        flex-direction: column;
        justify-content: end;
    }
    .output-view {
        flex-grow: 1;
    }
    .input {
        display: flex;
        align-items: stretch;
    }
    .stage {
        flex-grow: 1;
    }
</style>