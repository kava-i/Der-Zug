#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "test_params.h"

TestParameters *TestParameters::instance = 0;

int main (int argc, char* argv[]) {

  Catch::Session session;

  std::string path = "";

  //Build new parser on top of Catch's
  using namespace Catch::clara;
  auto cli = session.cli()  
    | Opt(path, "path")
      ["-p"]["--path"]
      ("which game shall be tested");

  //No pass the new composite back to Catch so it uses that
  session.cli(cli);

  //Let Catch (using Clara) parse the command line
  int return_code = session.applyCommandLine(argc, argv);
  if (return_code != 0) //Indicates a command line error
    return return_code;

  //If set in the command line then "path" is now set at this point
  if (path != "") {
    TestParameters* params = params->getInstance();
    params->set_txtad_path(path);
  }

  return session.run();
}
