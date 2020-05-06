<template>
    <div id="posts">
        <b-button v-if="$parent.$data.userID" variant="outline-primary" @click="$router.push('/new_post')">Create a new post</b-button>
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

</style>
<!--{
                    id: 1,
                    title: "Is vue cool? the answer might surprise you - Yotam",
                    body: 'hell ya!',
                }-->