<template>
    <div class="post-container">
        <h4>{{title}}</h4>
        <b-icon v-if="displayTrashIcon" icon="trash" @click="showMessageBox(title)" style="float: right"
                :id="'delete-'+id"></b-icon>
        <b-icon v-if="displayTrashIcon" icon="pen"
                @click="$router.push({name: 'newPost', params: {isEditPostPage: true, ogContent: body, title: title, postID: id}})"
                style="float: right" :id="'edit-'+id"></b-icon>

        <p style="white-space: pre-wrap; overflow: hidden; overflow-wrap: break-word">{{body}}</p>
        <b-tooltip :target="'delete-'+id" triggers="hover">Delete this post</b-tooltip>
        <b-tooltip :target="'edit-'+id" triggers="hover">Edit this post</b-tooltip>
    </div>

</template>

<script>
    export default {
        name: "Post",
        props: ['title', 'body', 'displayTrashIcon', 'onTrash', 'id'],
        methods: {
            showMessageBox(postTitle) {
                this.$bvModal.msgBoxConfirm(`Are you sure you want to delete '${postTitle}'?`, {
                    okTitle: 'Yes',
                    cancelTitle: 'No'
                }).then(status => {
                    if (status) {
                        this.onTrash(this.id);
                    }
                })
            }
        }
    }
</script>

<style scoped>
    .post-container {
        display: block;
        border: 0.08rem #b5c7d1 solid;
        border-radius: 0.5rem;
        padding: 1rem;
        /*margin: auto;*/
    }
</style>