/**     
 * @author fux    
*/    
#include <iostream>    
#include <string>    
    
#include "server/server_frame.h"
#include "world/worlds.h"
#include "world/world.h"
    

int main() {
  
  //Create server frame depending on server (with ssl) or local (no ssl).
#ifdef _COMPILE_FOR_SERVER_
  ServerFrame srv_frame(
      "/etc/letsencrypt/live/kava-i.de/cert.pem",
      "/etc/letsencrypt/live/kava-i.de/privkey.pem");
#else
  ServerFrame srv_frame;
#endif

  //Start server.
  srv_frame.Start(4488); 
}
