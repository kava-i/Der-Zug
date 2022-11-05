/**     
 * @author fux    
*/    
#include <iostream>    
#include <string>    
    
#include "server/server_frame.h"
#include "world/worlds.h"
#include "world/world.h"
    

int main(int x, char **argc) {
  int port = (x > 1) ? std::stoi(argc[1]) : 4488;
  int textad_port = (x > 2) ? std::stoi(argc[2]) : 4489;
  
  
  //Create server frame depending on server (with ssl) or local (no ssl).
#ifdef _COMPILE_FOR_SERVER_
  ServerFrame srv_frame(textad_port,
      "/etc/letsencrypt/live/kava-i.de-0001/cert.pem",
      "/etc/letsencrypt/live/kava-i.de-0001/privkey.pem");
#else
  ServerFrame srv_frame(textad_port);
#endif

  //Start server.
  srv_frame.Start(port); 
}
