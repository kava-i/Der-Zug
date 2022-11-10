#include "objects/room.h"
#include "tools/gramma.h"
#include <memory>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include "test_params.h"

TestParameters *TestParameters::instance = 0;

int main (int argc, char* argv[]) {

  Catch::Session session;

  std::string path = "";
  std::string player = "";

  //Build new parser on top of Catch's
  using namespace Catch::Clara;
  auto cli = session.cli()  
    | Opt(path, "path")
      ["-g"]["--path"]
      ("which game shall be tested?")
    | Opt(player, "player")
      ["-p"]["--player"]
      ("with which player shall the game be tested?");

  //No pass the new composite back to Catch so it uses that
  session.cli(cli);

  //Let Catch (using Clara) parse the command line
  int return_code = session.applyCommandLine(argc, argv);
  if (return_code != 0) //Indicates a command line error
    return return_code;

  //Set testing parameters
  TestParameters* params = TestParameters::getInstance();
  if (path != "") 
    params->set_txtad_path(path);
  if (player != "") 
    params->set_txtad_player(player);

  // set statics 
  CGramma gramma({});
  CRoom::set_gramma(std::make_shared<CGramma>(gramma));

  return session.run();
}
