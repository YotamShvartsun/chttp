<template>
    <div class="f-container">
        <div style="padding-bottom: 10px;" class="f-container">
            <b-button v-if="$parent.$data.userID" variant="outline-primary" @click="$router.push('/new_post')">Create a
                new post
            </b-button>
        </div>
        <div v-for="post in posts" v-bind:key="post.id">
            <Post :title="post.title" :body="post.body"></Post>
            <br>
        </div>

    </div>
</template>

<script>
    import Post from "@/components/Post";

    export default {
        name: "HomeScreen",
        components: {Post},
        data() {
            return {
                posts: []
            }
        },
        async created() {
            const resp = await fetch('/api/v1/posts');
            if (!resp.ok) {
                alert('Something went wrong!');
                console.error(await resp.json());
            } else {
                const data = (await resp.json());
                this.posts = data.posts;
            }
        }
    }
</script>

<style scoped>
    .f-container {
        display: flex;
        flex-flow: row wrap;
    }

    .f-container > * {
        flex: 1 100%;
    }
</style>
