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
      srv_frame.ServeFile(req, resp); });
  srv.Get("/(.*)/files/(.*)/(.*)/(.*)/(.*)", 
      [&](const Request& req, Response& resp) { 
    srv_frame.ServeFile(req, resp);});
  srv.Get("/(.*)/files/(.*)/(.*)/(.*)", [&](const Request& req, Response& resp) { 
    srv_frame.ServeFile(req, resp); });
  srv.Get("/(.*)/files/(.*)/(.*)", [&](const Request& req, Response& resp) { 
    srv_frame.ServeFile(req, resp); });
  srv.Get("/(.*)/backups/(.*)", [&](const Request& req, Response& resp) { 
    srv_frame.ServeFile(req, resp, true); });
  srv.Get("/(.*)/files/(.*)", [&](const Request& req, Response& resp) { 
    srv_frame.ServeFile(req, resp); });

  //Actions
  srv.Post("/api/user_login", [&](const Request& req, Response& resp) {
    srv_frame.DoLogin(req, resp); });
  srv.Post("/api/user_registration", [&](const Request& req, Response& resp) {
    srv_frame.DoRegistration(req, resp); });
  srv.Post("/api/user_logout", [&](const Request& req, Response& resp) {
    srv_frame.DoLogout(req, resp); });
  srv.Post("/api/create_backup", [&](const Request& req, Response& resp) {
    srv_frame.Backups(req, resp, "create"); });
  srv.Post("/api/restore_backup", [&](const Request& req, Response& resp) {
    srv_frame.Backups(req, resp, "restore"); });
  srv.Post("/api/delete_backup", [&](const Request& req, Response& resp) {
    srv_frame.Backups(req, resp, "delete"); });
  srv.Post("/api/write_object", [&](const Request& req, Response& resp) {
    srv_frame.WriteObject(req, resp); });

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
      resp.set_content(func::GetPage("web/general.js"), "application/javascript"); });
  srv.Get("/web/object.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/object.js"), "application/javascript"); });
  srv.Get("/web/backup.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/backup.js"), "application/javascript"); });
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
