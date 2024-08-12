<script>
    import ChatDialog from './ChatDialog.vue'
    export default {
        data() {
            return {
                current_contact: 'public',
                event_source: null,
                message_history: {
                    'public': [],
                    'boss': [
                        'hello',
                        'hi',
                        'nice to meet you',
                        'i am find',
                        'how is your day',
                    ],
                    'friend': [
                        'good mordning',
                        'how have you been'
                    ],
                    'coworker': [
                        'how about a drink later ?'
                    ]
                }
            }
        },
        components: {
            ChatDialog
        },
        methods: {
            set_current_contact(s) {
                this.current_contact = s
            }
        }
    }
</script>

<template>
        <div class="chat-panel">
            <ul class="contact-list">
                <li v-for="name in Object.keys(message_history)" @click="set_current_contact(name)">
                    <span :class=" (name == current_contact)? 'active-contact': 'contact' ">
                        {{ name }}
                    </span>
                </li>
            </ul>
            <keep-alive class="flex-item-chat-dialog">
                <chat-dialog
                    :prop_messages="message_history[current_contact]"
                    :prop_name="current_contact"
                    :key="current_contact">
                </chat-dialog>
            </keep-alive>
        </div>
    </template>

<style scoped>
    .chat-panel {
        width: 100%;
        height: 100%;
        display: flex;
        align-items: stretch;
    }
    .contact-list {
        padding: 0;
        list-style-type: none;
    }
    .contact {
        border: 1px solid lightgray;
        display: inline-block;
        width: 100%;
    }
    .contact:hover {
        background-color: lightgray;
    }
    .active-contact {
        background-color: lightskyblue;
        display: inline-block;
        width: 100%;
    }
    .flex-item-chat-dialog {
        flex-grow: 1;
    }
</style>