#include <iostream>

#include "tsdb/version.hpp"

int main() {
  std::cout << "tsdb " << tsdb::version() << '\n';
  return 0;
}
