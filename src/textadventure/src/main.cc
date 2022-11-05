#include <JanGeschenk/Webgame.hpp>
#include <exception>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <webgame/webgame.h>

#ifdef _COMPILE_FOR_SERVER_
httplib::SSLServer srv(
    "/etc/letsencrypt/live/kava-i.de-0001/cert.pem",
    "/etc/letsencrypt/live/kava-i.de-0001/privkey.pem"
); 
#else
httplib::Server srv;
#endif

/**
 * Function load a (txt-)file from disc.
 * @param[in] path (path to file to load)
 * @return return file as string
 */
std::string GetPage(std::string path) {
  //Read loginpage and send
  std::ifstream read(path);
  if (!read) {
    std::cout << "Wrong file passed: " << path << std::endl;
    return "";
  }
  std::string page( (std::istreambuf_iterator<char>(read) ),
                    std::istreambuf_iterator<char>()     );
  //Delete file-end marker
  page.pop_back();
  return page;
}

/**
 * Function to load an image from disc.
 * @param[in] path (to images)
 * @param[in] iamge (name of image to load)
 * @return Return image as string.
 */
std::string GetImage(std::string path, std::string image) {
  path = path + "images/" + image + ".jpg";
  std::ifstream f(path, std::ios::in|std::ios::binary|std::ios::ate);    
  if (!f.is_open()) {
    std::cout << "Image could not be found: " << path << std::endl;    
    return "";
  }
  FILE* file_stream = fopen(path.c_str(), "rb");    
  std::vector<char> buffer;    
  fseek(file_stream, 0, SEEK_END);    
  long length = ftell(file_stream);    
  rewind(file_stream);    
  buffer.resize(length);    
  length = fread(&buffer[0], 1, length, file_stream);    
      
  std::string s(buffer.begin(), buffer.end());    
  return s;
}

/**
* Function to load an image from disc.
* @param[in] path (to images)
* @param[in] iamge (name of image to load)
* @return Return image as string.
*/
std::string GetSound(std::string path, std::string sound) {
 path = path + sound + ".mp3"; std::ifstream f(path,
     std::ios::in|std::ios::binary|std::ios::ate);    
 if (!f.is_open()) {
   std::cout << "Audio could not be found: " << path << std::endl;    
   return "";
 }
 FILE* file_stream = fopen(path.c_str(), "rb");    
 std::vector<char> buffer;    
 fseek(file_stream, 0, SEEK_END);    
 long length = ftell(file_stream);    
 rewind(file_stream);    
 buffer.resize(length);    
 length = fread(&buffer[0], 1, length, file_stream);    
     
 std::string s(buffer.begin(), buffer.end());    
 std::cout << "Audio loaded, size: " << s.size() << std::endl;
 return s;
}

std::string GetWorldId(std::string creator, std::string name) {
  return creator + "/" + name;
}


/**
 * main loop
 */
int main(int x, char **argc) {
  int port = 4489;
  if (x > 1)
    port = std::stoi(argc[1]);
  std::map<std::string, std::shared_ptr<Webgame<WebserverGame, CGame>>> webgames;
  std::shared_mutex mutex_webgames;
  std::cout << "Starting txtad server on port: " << port << std::endl;

  //Open httpserver to serve main-page
  std::thread t1([&]() {
    // Serve media: images
    srv.Get("/(.*)/(.*)/(.*).jpg", [&](const httplib::Request& req, httplib::Response& resp)  {
        std::cout << "get background: " << std::endl;
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        std::cout << "get background: " << game_id << std::endl;
        // If game not found return 404
        if (webgames.count(game_id) == 0) {
          resp.status = 404;
          return;
        }
        // Otherwise load audio at game-path.
        std::string path = webgames.at(game_id)->game_->path();
        resp.set_content(GetImage(path, req.matches[3]), "image/jpg"); 
    });
    // Serve media: audio
    srv.Get("/(.*)/(.*)/(.*).mp3", [&](const httplib::Request& req, httplib::Response& resp)  {
        std::cout << "get music: " << std::endl;
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        std::cout << "get music: " << game_id << std::endl;
        // If game not found return 404
        if (webgames.count(game_id) == 0) {
          resp.status = 404;
          return;
        }
        // Otherwise load audio at game-path.
        std::string path = webgames.at(game_id)->game_->path();
        resp.set_content(GetSound(path, req.matches[3]), "audio/mp3"); 
    });

   
    // Create new game 
    srv.Post("/api/create/", [&](const httplib::Request& req, httplib::Response& resp) {
        std::shared_lock ul_mtx(mutex_webgames);
        nlohmann::json json = nlohmann::json::parse(req.body); 
        std::string path = json["path"].get<std::string>();
        std::string name = json["name"].get<std::string>();
        std::string creator = json["creator"].get<std::string>();
        int port = json["port"].get<int>();
        std::string game_id = GetWorldId(creator, name);
        std::cout << "create: " << game_id << std::endl;
        // Game already running
        if (webgames.count(game_id) > 0) {
          resp.status = 409;
        }
        else {
          try {
            // Create new webgame with game based on given path:
            webgames[game_id] = std::make_shared<Webgame<WebserverGame, CGame>>(
                std::make_shared<CGame>(path), port);
            // set port and run: 
            std::thread ng([&]() { webgames[game_id]->run(); });
            ng.detach();
            resp.status = 200;
            std::cout << "create: " << game_id << " success on port " << port << std::endl;
          } catch (std::exception& e) {
            std::cout << "create: " << game_id << " failed: " << e.what() << std::endl;
          }
        }
    });
    
    // Close running game
    srv.Get("/api/close/(.*)/(.*)", [&](const httplib::Request& req, httplib::Response& resp) {
        std::shared_lock ul_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        std::cout << "[close] game_id: " << game_id << std::endl;
        if (webgames.count(game_id) == 0) {
          resp.status = 404;
          return;
        }
        // Stop and remove game:
        webgames.at(game_id)->stop();
        webgames.erase(game_id);
    });
    
    // Check running game
    srv.Get("/api/running/(.*)/(.*)", [&](const httplib::Request& req, httplib::Response& resp) {
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        std::cout << "[running] game_id: " << game_id << std::endl;
        if (webgames.count(game_id) == 0) {
          resp.status = 404;
          return;
        }
        resp.status = 200;
    });
    
    // Function to serve main app
    srv.Get("/(.*)/(.*)", [&](const httplib::Request& req, httplib::Response& resp)  {
      std::shared_lock sl_mtx(mutex_webgames);
      std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
      std::cout << "[main] game_id: " << game_id << std::endl;
      // Get game
      if (webgames.count(game_id) == 0) {
        resp.status = 404;
        return;
      }
      auto cg = webgames.at(game_id)->game_;
      auto port = webgames.at(game_id)->port_;
      inja::Environment env;
      inja::Template temp = env.parse_template("index.html");
      resp.set_content(env.render(temp, { {"port",port}, {"music", cg->get_music()}, 
            {"image", cg->get_background_image()} }), "text/html"); });
 
    //Start main loop.
    srv.listen("0.0.0.0", port);
    return 0;
  });

  t1.join(); 
}
