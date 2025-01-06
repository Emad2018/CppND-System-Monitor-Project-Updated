#include "format.h"

#include <chrono>
#include <iostream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::chrono::seconds s{seconds};
  return std::format("{:%T}", s);
}