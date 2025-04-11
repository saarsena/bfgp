#include "examples/CollisionTest.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    Examples::CollisionTest test;
    test.run();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}