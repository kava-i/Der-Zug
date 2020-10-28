/**     
 * @author fux    
*/    
#include <iostream>    
#include <string>    
    
#include "server/server_frame.h"
    

int main() {

  //Create server frame, then start server.
  ServerFrame srv_frame;
  //Start server.
  srv_frame.Start(4488); 
}
