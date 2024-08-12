<script>
    export default {
        data() {
            return {
                'username': 'frank',
                'password': 'password',
                'error_msg': ''
            }
        },
        emits: ['authSuccess'],
        methods: {
            async login() {
                console.log('clicked')
                let auth_rsp = await fetch('/auth', {
                    method: 'POST',
                    headers: {
                        "Content-Type": "application/json",
                    },
                    body: JSON.stringify({
                        'username': this.username,
                        'password': this.password,
                    })
                })
                if (auth_rsp.status != 200) {
                    this.error_msg = await auth_rsp.text()
                    this.$refs.fail_dialog.showModal()
                } else {
                    this.$emit('authSuccess')
                }
            }
        }
    }
</script>

<template>
    <div class="login-form">
        <dialog ref="fail_dialog">
            {{ error_msg }}
            <form method="dialog">
                <button>ok</button>
            </form>
        </dialog>
        <label>
            <span>username</span>
            <input v-model="username">
        </label>
        <label>
            <span>password</span>
            <input type="password" v-model="password">
        </label>
        <button @click="login">
            login
        </button>
    </div>
</template>

<style scoped>
    .login-form {
        display: flex;
        flex-direction: column;
    }
</style>