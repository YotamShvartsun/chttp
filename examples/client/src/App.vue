<template>
    <div id="app">
        <b-navbar variant="dark" type="dark" sticky>
            <b-navbar-brand href="/">CHttp</b-navbar-brand>
            <b-collapse is-nav>
                <b-navbar-nav class="ml-auto" v-if="userID">
                    <b-nav-item @click="logOut" class="text-light">Logout</b-nav-item>
                    <b-nav-item @click="goToUserPage">Profile</b-nav-item>
                </b-navbar-nav>
                <b-navbar-nav class="ml-auto" v-else>
                    <b-nav-item @click="$router.push('/login')">Login</b-nav-item>
                </b-navbar-nav>
            </b-collapse>
        </b-navbar>
        <div style="padding: 1.5em">
            <router-view @userUpdated="onUserIDUpdate"></router-view>
        </div>
    </div>
</template>

<script>
    export default {
        name: 'App',
        data() {
            return {
                userID: null,
            }
        },
        async created() {
            let res = await fetch('/api/v1/users/me');
            if (res.ok) {
                const respJSON = await res.json();
                this.userID = respJSON.UID;
            }
        },
        methods: {
            onUserIDUpdate(UID) {
                this.userID = UID;
                localStorage.setItem('UID', UID);
            },
            async logOut(e) {
                e.preventDefault();
                await fetch('/api/v1/logout');
                localStorage.removeItem('UID');
                this.$router.go(0);
            },
            goToUserPage() {
                if (this.userID) {
                    this.$router.push(`/${this.userID}/posts`);
                }
            }
        }
    }
</script>

<style>

    #top * {
        margin-left: 15px;
    }
</style>
