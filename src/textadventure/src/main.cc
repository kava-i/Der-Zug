#include "objects/player.h"
#include "objects/room.h"
#include "spdlog/common.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "tools/gramma.h"
#include <JanGeschenk/Webgame.hpp>
#include <cstdlib>
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
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include <webgame/webgame.h>
#include <dotenv.h>

# define LOGGER "general"

#ifdef _HTTPS_
httplib::SSLServer srv(
		std::getenv("CERT_FILE"),
		std::getenv("CERT_PRIV_KEY")
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
    spdlog::get(LOGGER)->error("Wrong file passed: {}", path);
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
    spdlog::get(LOGGER)->error("Image could not be found: {}", path);
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
  path = path + "sounds/" + sound + ".mp3"; std::ifstream f(path,
     std::ios::in|std::ios::binary|std::ios::ate);    
  if (!f.is_open()) {
    spdlog::get(LOGGER)->error("Audio could not be found: {}", path);
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
  spdlog::get(LOGGER)->debug("Audio loaded, size: {}", s.size());
  return s;
}

std::string GetWorldId(std::string creator, std::string name) {
  return creator + "/" + name;
}

int GetNewPort(const std::map<std::string, std::shared_ptr<Webgame<WebserverGame, CGame>>> &games) {
  std::set<int> ports = {};
  for (const auto& it : games) 
    ports.insert(it.second->port_);
  int port = 9000;
  for (unsigned int i=0; i<9500; i++) {
    if (ports.count(port+i) == 0)
      return port+i;
  }
  return 0xFFFF;
}

/**
 * main loop
 */
int main(int x, char **argc) {
	dotenv::init();

  //Initialize contexts
  Context::initializeHanlders();
  Context::initializeTemplates();
  //Initialize objects
  CDState::initializeFunctions();
  CItem::initializeFunctions();
 
  std::string log_level = "info";
  int port = 4489;
  if (x > 1)
    log_level = argc[1];
  if (x > 2)
    port = std::stoi(argc[2]);
  std::cout << "Using log_level: " << log_level << " and port " << port << std::endl;
  std::map<std::string, std::shared_ptr<Webgame<WebserverGame, CGame>>> webgames;
  std::shared_mutex mutex_webgames;

  // setup logger 
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_st>());
  sinks.push_back(std::make_shared<spdlog::sinks::daily_file_format_sink_st>("logs/logfile.txt", 23, 59));
  auto logger = std::make_shared<spdlog::logger>(LOGGER, begin(sinks), end(sinks));
  spdlog::register_logger(logger);
  spdlog::flush_on(spdlog::level::warn);
  spdlog::flush_every(std::chrono::seconds(10));
  if (log_level == "error")
    spdlog::set_level(spdlog::level::err);
  else if (log_level == "warn")
    spdlog::set_level(spdlog::level::warn);
  else if (log_level == "info") {
    spdlog::set_level(spdlog::level::info);
  }
  else if (log_level == "debug") {
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);
  }

  // load gramma 
  CGramma gramma = CGramma({"dictionary.txt", "EIG.txt"});
  CPlayer::set_gramma(std::make_shared<CGramma>(gramma));
  CRoom::set_gramma(std::make_shared<CGramma>(gramma));

  spdlog::get(LOGGER)->info("Starting txtad server on port: {}", port);

  //Open httpserver to serve main-page
  std::thread t1([&]() {
    // Serve media: images
    srv.Get("/(.*)/(.*)/(.*).jpg", [&](const httplib::Request& req, httplib::Response& resp)  {
        spdlog::get(LOGGER)->debug("[get-background]");
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        spdlog::get(LOGGER)->debug("[get-background] game-id: {}", game_id);
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
        spdlog::get(LOGGER)->info("[get-audio]");
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        spdlog::get(LOGGER)->debug("[get-audio] game-id: {}", game_id);
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
        spdlog::get(LOGGER)->debug("[create]");
        std::shared_lock ul_mtx(mutex_webgames);
        nlohmann::json json = nlohmann::json::parse(req.body); 
        std::string path = json["path"].get<std::string>();
        if (path.back() != '/') 
          path.append("/");
        std::string name = json["name"].get<std::string>();
        std::string creator = json["creator"].get<std::string>();
        int port = GetNewPort(webgames);
        if (port == 0xFFFF) {
            resp.status = 401;
            spdlog::get(LOGGER)->error("[create] failed creating game {} to many worlds!");
            std::cout << "[create] failed creating game {} to many worlds!" << std::endl;
        }
        std::string game_id = GetWorldId(creator, name);
        spdlog::get(LOGGER)->debug("[create] game-id: {}", game_id);
        // Game already running
        if (webgames.count(game_id) > 0) {
          resp.status = 409;
        }
        else {
          try {
            // Create new webgame with game based on given path:
            std::shared_ptr<CGame> game = std::make_shared<CGame>(path);
            if (game->SetupGame()) {
              std::shared_ptr<Webgame<WebserverGame, CGame>> web_game =
                std::make_shared<Webgame<WebserverGame, CGame>>(game, port);
              webgames[game_id] = web_game;
              // set port and run: 
              std::thread ng([web_game]() { 
                  spdlog::get(LOGGER)->info("[run] game valid? {}", (web_game != nullptr));
                  if (web_game)
                    web_game->run(); 
              });
              ng.detach();
              resp.status = 200;
              spdlog::get(LOGGER)->debug("[create] created game {} on port {}", game_id, port);
            }
            else {
              resp.status = 400;
              spdlog::get(LOGGER)->error("[create] failed creating game {} invalid world!", game_id);

            }
          } catch (std::exception& e) {
            spdlog::get(LOGGER)->error("[create] failed for game {}: ", game_id, e.what());
          }
        }
    });
    
    // Close running game
    srv.Get("/api/close/(.*)/(.*)", [&](const httplib::Request& req, httplib::Response& resp) {
        spdlog::get(LOGGER)->debug("[close]");
        std::shared_lock ul_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        spdlog::get(LOGGER)->debug("[close] game-id: {}", game_id);
        if (webgames.count(game_id) == 0) {
          resp.status = 204;
          return;
        }
        // Stop and remove game:
        webgames.at(game_id)->stop();
        webgames.erase(game_id);
        resp.status = 200;
    });
    
    // Check running game
    srv.Get("/api/running/(.*)/(.*)", [&](const httplib::Request& req, httplib::Response& resp) {
        spdlog::get(LOGGER)->debug("[running]");
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        spdlog::get(LOGGER)->debug("[running] game-id: {}", game_id);
        if (webgames.count(game_id) == 0) {
          resp.status = 204;
          return;
        }
        resp.status = 200;
    });
    
    // Function to serve main app
    srv.Get("/(.*)/(.*)", [&](const httplib::Request& req, httplib::Response& resp)  {
        spdlog::get(LOGGER)->debug("[main]");
        std::shared_lock sl_mtx(mutex_webgames);
        std::string game_id = GetWorldId(req.matches[1], req.matches[2]);
        spdlog::get(LOGGER)->debug("[main] game-id: {}", game_id);
        // Get game
        if (webgames.count(game_id) == 0) {
          resp.status = 404;
          return;
        }
        auto cg = webgames.at(game_id)->game_;
        auto port = webgames.at(game_id)->port_;
        nlohmann::json data = { {"port", port}, {"music", cg->get_music()}, {"image", cg->get_background_image()}, 
          {"world", req.matches[2]} };
        data["appearance"] = cg->get_appearance_config();
        std::cout << "APPEARANCE: " << cg->get_appearance_config() << std::endl;
        inja::Environment env;
        inja::Template temp = env.parse_template("index.html");
        spdlog::get(LOGGER)->debug("[main] image: {}", cg->get_background_image());
        resp.set_content(env.render(temp, data), "text/html"); 
    });
 
    //Start main loop.
    srv.listen("0.0.0.0", port);
    return 0;
  });

  t1.join(); 
}
