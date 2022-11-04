#include <cctype>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <filesystem>

#define LOGGER "logger"

int main( int argc, char* argv[] ) {
  // global setup...
  std::filesystem::remove("test/logs/test-log.txt");
  
  srand (time(NULL));

  int result = Catch:: Session().run( argc, argv );

  return result;
}


