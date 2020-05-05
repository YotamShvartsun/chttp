<template>
    <div class="post-container">
        <h4>{{title}}</h4>
        <b-icon v-if="displayTrashIcon" icon="trash" @click="showMessageBox(title)" style="float: right"
                :id="'delete-'+id"></b-icon>
        <b-icon v-if="displayTrashIcon" icon="trash"
                @click="$router.push({name: 'newPost', params: {isEditPostPage: true, ogContent: body, title: title, postID: id}})"
                style="float: right"></b-icon>

        <span style="white-space: pre">{{body}}</span>
        <b-tooltip :target="'delete-'+id" triggers="hover">Delete this post</b-tooltip>
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
        border: 1px #b5c7d1 solid;
        border-radius: 5px;
        padding: 15px;
        margin: auto;
    }
</style>