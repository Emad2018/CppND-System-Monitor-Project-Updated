#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  return std::stof(LinuxParser::CpuUtilization()[0]);
}