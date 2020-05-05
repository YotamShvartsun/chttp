<template>
    <b-container v-if="!isEditPostPage">
        <b-form @submit="postForm">
            <b-form-group label="Title" label-for="titleInput">
                <b-form-input type="text" id="titleInput" required placeholder="Post title:"
                              v-model="userInput.title"></b-form-input>
            </b-form-group>
            <b-form-group label="Content" label-for="content">
                <b-form-textarea id="content" required placeholder="Post content:"
                                 v-model="userInput.body"></b-form-textarea>
            </b-form-group>
            <b-button type="submit" variant="primary">Post!</b-button>
        </b-form>
    </b-container>
    <b-container v-else>
        <b-form @submit="editForm">
            <b-form-group label="Title" label-for="titleInput">
                <b-form-input type="text" id="titleInput" required :placeholder="title" :value="title"
                              v-model="userInput.title" disabled></b-form-input>
            </b-form-group>
            <b-form-group label="Content" label-for="content">
                <b-form-textarea id="content" :placeholder="ogContent" :value="ogContent"
                                 v-model="userInput.body"></b-form-textarea>
            </b-form-group>
            <b-button type="submit" variant="primary">Post!</b-button>
        </b-form>
    </b-container>
</template>

<script>
    export default {
        name: "NewPostScreen",
        data() {
            return {
                userInput: {
                    title: null,
                    body: null
                },
                error: null
            }
        },
        created(){
          if(this.$props.isEditPostPage){
              this.userInput.body = this.$props.ogContent;
          }
        },
        props: ['isEditPostPage', 'ogContent', 'title', 'postID'],
        methods: {
            async postForm(e) {
                e.preventDefault();
                const resp = await fetch('/api/v1/newPost', {
                    method: 'post',
                    headers: {'content-type': 'application/json'},
                    body: JSON.stringify(this.userInput)
                });
                if (!resp.ok) {
                    this.error = (await resp.json()).error;
                } else {
                    this.$router.push('/');
                }
            },
            async editForm(e){
                e.preventDefault();
                const resp = await fetch(`/api/v1/edit/${this.$props.postID}`, {
                    method: 'post',
                    headers: {'content-type': 'application/json'},
                    body: JSON.stringify({body: this.userInput.body})
                });
                if(!resp.ok){
                    this.error = (await resp.json()).error;
                }else{
                    this.$router.push('/');
                }
            }
        }
    }
</script>

<style scoped>

</style>