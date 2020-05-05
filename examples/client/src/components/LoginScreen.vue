<template>
    <div id="login-form">
        <b-form @submit="doLogin">
            <b-form-group id="username_input_group" label="Username:" label-for="username">
                <b-form-input id="username" v-model="input.username" type="text" required
                              placeholder="Enter username:"></b-form-input>
            </b-form-group>
            <b-form-group id="password_input_group" label="Password:" label-for="password">
                <b-form-input id="password" v-model="input.password" type="password" required
                              placeholder="Enter password"></b-form-input>
            </b-form-group>
            <b-button type="submit" variant="primary">Submit</b-button>
        </b-form>
        <p>Don't have an account? <b-link @click="$router.push('/signup')">Sign up!</b-link> </p>
    </div>
</template>

<script>
    export default {
        name: "LoginScreen",
        data() {
            return {
                input: {
                    username: null,
                    password: null
                },
                errorMessage: null
            }
        },
        methods: {
            async doLogin(e) {
                e.preventDefault();
                const resp = await fetch('/api/v1/login', {
                    method: 'post', headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify(this.input)
                });
                if (!resp.ok) {
                    this.errorMessage = (await resp.json()).error;
                } else {
                    this.$emit('userUpdated', (await resp.json()).UID);
                    this.$router.push('/');
                }
            }
        }
    }
</script>

<style scoped>
#login-form {
    padding-left: 30%;
    padding-right: 30%;
    align-content: center;
}
</style>