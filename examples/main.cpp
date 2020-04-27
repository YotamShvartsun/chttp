#include <iostream>
#include <chttp/CHttp.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <json.hpp>


struct blogPostJSON {
    int id{};
    std::string title;
    std::string body;
};

void to_json(nlohmann::json& j, const blogPostJSON& p) {
    j = nlohmann::json{{"title", p.title}, {"body", p.body}, {"id", p.id}};
}

void from_json(const nlohmann::json& j, blogPostJSON& p) {
    j.at("id").get_to(p.id);
    j.at("title").get_to(p.title);
    j.at("body").get_to(p.body);
}

void CreateTables(SQLite::Database &db) {
    db.exec("CREATE TABLE IF NOT EXISTS users(id integer primary key, username char(20), password text)");
    db.exec("CREATE TABLE IF NOT EXISTS posts(id integer primary key, publisher_id integer,post_title text, post_content TEXT, foreign key(publisher_id) references users(id))");
}

void Signup(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
//    if(req->) TODO write a function to return one header
    nlohmann::json body = nlohmann::json::parse(dynamic_cast<PostRequest *>(req.get())->GetBody());
    std::string username = body["username"].get<std::string>();
    std::string password = body["password"].get<std::string>();
    nlohmann::json responseBody;
    try {
        SQLite::Database db("exampleBlogDB.db");
        SQLite::Statement insertNewUser(db, "insert into users(username, password) values (?,?)");
        insertNewUser.bind(1, username);
        insertNewUser.bind(2, password);
        while (insertNewUser.executeStep()) {
            responseBody["id"] = db.getLastInsertRowid();
        }
        resp->AddCookie("Auth", responseBody.dump());
    } catch (std::exception &e) {
        responseBody["error"] = "Unable to create a new user!";
        resp->SetStatus(Unauthorized);
    }
    resp->Header("Content-Type", "application/json");
    resp->Raw(responseBody.dump());
}

void usersMe(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
    resp->Header("Content-Type", "application/json");
    CookieJar jar(req->GetHeaders()["cookie"]);
    resp->Raw(jar.GetCookieValue("Auth"));
}

void Logout(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
    resp->RemoveCookie("Auth");
    resp->Raw("ok");
}

void Login(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
    nlohmann::json body = nlohmann::json::parse(dynamic_cast<PostRequest *>(req.get())->GetBody());
    nlohmann::json responseBody;
    responseBody["error"] = "Invalid credentials";
    resp->SetStatus(Unauthorized);
    std::string username = body["username"].get<std::string>();
    std::string password = body["password"].get<std::string>();

    try {
        SQLite::Database db("exampleBlogDB.db");
        SQLite::Statement stmt(db, "select id from users where username = ? and password = ?");
        stmt.bind(1, username);
        stmt.bind(2, password);
        while (stmt.executeStep()) {
            if (responseBody.contains("error")) {
                responseBody.clear();
            }
            responseBody["UID"] = std::stoi(stmt.getColumn("id"));
        }
        if (!responseBody.contains("error")) {
            resp->SetStatus(OK);
            resp->AddCookie("Auth", responseBody.dump());
        }
    } catch (...) {
        resp->SetStatus(Internal_Server_Error);
        responseBody["error"] = "Something went wrong";
    }
    resp->Header("Content-Type", "application/json");
    resp->Raw(responseBody.dump());
}

void CreatePost(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
    CookieJar cookies(req->GetHeaders()["cookie"]);
    nlohmann::json respBody;
    if (cookies.IsInJar("Auth")) {
        try {
            nlohmann::json cookieBody = nlohmann::json::parse(cookies.GetCookieValue("Auth"));
            SQLite::Database db("exampleBlogDB.db", SQLite::OPEN_READWRITE);
            SQLite::Statement isUser(db, "select count(*) as count from users where id=?");
            bool isLoggedIn = false;
            int tmp = cookieBody["UID"].get<int>();
            isUser.bind(1, tmp);
            while (isUser.executeStep()) {
                tmp = isUser.getColumn("count");
                if (tmp != 0) {
                    isLoggedIn = true;
                }
            }
            if (isLoggedIn) {
                SQLite::Statement insertPost(db,
                                             "insert into posts(publisher_id, post_title, post_content) values (?,?,?)");
                tmp = cookieBody["UID"].get<int>();
                insertPost.bind(1, tmp);
                nlohmann::json requestBody = nlohmann::json::parse(dynamic_cast<PostRequest *>(req.get())->GetBody());
                std::string requestTmp = requestBody["title"].get<std::string>();
                insertPost.bind(2, requestTmp);
                requestTmp = requestBody["body"].get<std::string>();
                insertPost.bind(3, requestTmp);
                insertPost.exec();
                respBody["id"] = db.getLastInsertRowid();
            } else {
                respBody["error"] = "You are not allowed to do that";
                resp->SetStatus(Unauthorized);
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            respBody["error"] = "Something went wrong!";
            resp->SetStatus(Internal_Server_Error);
        }
    } else {
        respBody["error"] = "You are not allowed to do that";
        resp->SetStatus(Unauthorized);
    }
    resp->Header("Content-Type", "application/json");
    resp->Raw(respBody.dump());
}

void AllPosts(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp){
    nlohmann::json responseBody;
    try{
        SQLite::Database db("exampleBlogDB.db");
        SQLite::Statement allPosts(db, "select posts.post_content, posts.post_title, posts.id, users.username from posts join users on publisher_id=users.id");
        nlohmann::json postsJSON = nlohmann::json::array();
        blogPostJSON postTmp;
        std::string titleTmp;
        std::vector<blogPostJSON> posts;
        while(allPosts.executeStep()){
            postTmp.body = allPosts.getColumn(0).getString();
            titleTmp = allPosts.getColumn(1).getString();
            titleTmp += " - " + allPosts.getColumn(3).getString();
            postTmp.title = titleTmp;
            postTmp.id = allPosts.getColumn(2).getInt();
            posts.push_back(postTmp);
        }
        postsJSON = nlohmann::json(posts);
        responseBody["posts"] = postsJSON;
    }catch(std::exception &e){
        std::cerr << e.what() << std::endl;
        resp->SetStatus(Not_Found);
        responseBody["error"] = "Unable to fetch posts";
    }
    resp->Header("Content-Type", "application/json");
    resp->Raw(responseBody.dump());
}


int main(int argc, char **argv) {
    int port = 8081;
    if (argc == 2) {
        port = std::stoi(argv[1]);
    }
    {
        SQLite::Database dbHandler("exampleBlogDB.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        CreateTables(dbHandler);
    }
    Server server;
    server.Post("/api/v1/signup", Signup, {});
    server.Post("/api/v1/login", Login, {});
    server.Post("/api/v1/newPost", CreatePost, {});
    server.Get("/api/v1/users/me", usersMe, {});
    server.Get("/api/v1/logout", Logout, {});
    server.Get("/api/v1/posts", AllPosts, {});
    server.Run(port);
}