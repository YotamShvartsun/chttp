import Vue from 'vue';
import VueRouter from 'vue-router';
// import HomeScreen from "@/components/HomeScreen";
// import LoginScreen from "@/components/LoginScreen";
// import SignupPage from "@/components/SignupPage";
// import NewPostScreen from "@/components/NewPostScreen";
// import UserPage from "@/components/UserPage";

Vue.use(VueRouter);

const router = new VueRouter({
    routes: [
        {
            path: '/', name: 'Home', component() {
                return import('@/components/HomeScreen')
            }
        },
        {
            path: '/login', name: 'Login', component() {
                return /*webpackChunkName: 'login'*/ import('@/components/LoginScreen')
            }
        },
        {
            path: '/signup', name: 'signup', component() {
                return import('@/components/SignupPage')
            }
        },
        {
            path: '/new_post', name: 'newPost', component() {
                return import('@/components/NewPostScreen')
            }, props: true
        },
        {
            path: '/:user_id/posts', name: 'posts', component() {
                return import('@/components/UserPage')
            }, canReuse: false
        },
    ]
});

export default router;