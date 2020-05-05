<template>
    <div>
        <div v-if="loaded" class="f-container">
            <b-button v-if="isOwnPage" variant="outline-primary" @click="$router.push('/new_post')">Create a new post</b-button>
            <div v-for="post in posts" v-bind:key="post.id" style="padding: 5px">
                <Post :title="post.title" :body="post.body" :display-trash-icon="isOwnPage" :on-trash="DeletePost"
                      :id="post.id"></Post>
            </div>
        </div>
        <div v-else>
            <h4>Loading..</h4>
            <slot></slot>
        </div>
    </div>
</template>

<script>
    import Post from "@/components/Post";

    export default {
        name: "UserPage",
        components: {Post},
        data() {
            return {
                loaded: false,
                posts: [],
                isOwnPage: false,
                username: null
            }
        },
        watch:{
            '$route': 'onCreatedEvent'
        },
        async created() {
            await this.onCreatedEvent();
        },
        methods: {
            async DeletePost(id) {
                const resp = await fetch(`/api/v1/delete/${id}`);
                if (!resp.ok) {
                    alert((await resp.json())['error']);
                } else {
                    this.$router.go(0);
                }
            },
            async onCreatedEvent(){
                this.isOwnPage = parseInt(this.$route.params.user_id) === parseInt(localStorage.getItem('UID'));
                const resp = await fetch(`/api/v1/${this.$route.params.user_id}/posts`);
                if (resp.ok) {
                    this.posts = (await resp.json())['posts'];
                    this.loaded = true;
                }
                else {
                    alert('Something went wrong');
                    this.$router.push('/');
                }
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

    .post-container {
        margin: 0 auto;
        /*line-height: 0;*/
    }

    .trash-icon {
        float: right;
    }
</style>