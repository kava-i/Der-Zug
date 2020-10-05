/**     
 * @author fux    
*/    
#include <filesystem>    
#include <fstream>    
#include <iostream>    
#include <string>    
    
#include <httplib.h>    
    
#include "server/server_frame.h"
#include "util/func.h"    
    
namespace fs = std::filesystem;    
using namespace httplib;

int main() {
  Server srv;
  ServerFrame srv_frame;

  int start_port = std::stoi("4488");
  std::cout << "Starting on port " << start_port << std::endl;

  //Server-frame 
  
  //Pages
  srv.Get("/login", [&](const Request& req, Response& resp) { 
        srv_frame.LoginPage(req, resp);});
  srv.Get("/overview", [&](const Request& req, Response& resp) { 
      srv_frame.Overview(req, resp); });

  srv.Get("/world_(.*)_category_(.*)_sub_(.*)_obj_(.*)", 
      [&](const Request& req, Response& resp) { 
      try{srv_frame.Object(req, resp); }
      catch(std::exception& e) {std::cout << e.what() << std::endl;} });

  srv.Get("/world_(.*)_category_(.*)_sub_(.*)", [&](const Request& req, Response& resp) { 
      try{srv_frame.SubCategory(req, resp); }
      catch(std::exception& e) {std::cout << e.what() << std::endl;} });

  srv.Get("/world_(.*)_category_(.*)", [&](const Request& req, Response& resp) { 
      try{srv_frame.Category(req, resp); }
      catch(std::exception& e) {std::cout << e.what() << std::endl;} });

  srv.Get("/world_(.*)", [&](const Request& req, Response& resp) { 
      try{srv_frame.World(req, resp); }
      catch(std::exception& e) {std::cout << e.what() << std::endl;} });

  //Actions
  srv.Post("/api/user_login", [&](const Request& req, Response& resp) 
      { srv_frame.DoLogin(req, resp); });
  srv.Post("/api/user_registration", [&](const Request& req, Response& resp) {
        try{srv_frame.DoRegistration(req, resp);}
        catch(std::exception& e) {std::cout << e.what() << std::endl;}
      });
  srv.Post("/api/user_logout", [&](const Request& req, Response& resp)
      { srv_frame.DoLogout(req, resp); });

  //html
  srv.Get("/", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/main.html"), "text/html"); });
  srv.Get("/login", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/login.html"), "text/html"); });
  srv.Get("/registration", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/registration.html"), "text/html"); });

  //javascript/ css
  srv.Get("/web/general.css", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/general.css"), "text/css"); });
  srv.Get("/web/object.css", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/object.css"), "text/css"); });
  srv.Get("/web/general.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/general.js"), "application/js"); });
  srv.Get("/web/registration.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/registration.js"), 
          "application/javascript");});
  srv.Get("/web/login.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/login.js"), 
          "application/javascript");});
  srv.Get("/web/overview.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/overview.js"), 
          "application/javascript");});


  //Images
  srv.Get("/web/background.jpg", [](const Request& req, Response& resp) {
      resp.set_content(func::GetImage("web/images/background.jpg"), "image/jpg");});

  srv.listen("0.0.0.0", start_port);
}
