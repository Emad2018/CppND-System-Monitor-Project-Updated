#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid) : pid_(pid) {};
// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  long activejiffes, uptime;
  float cpu_usage;
  activejiffes = LinuxParser::ActiveJiffies(pid_);
  uptime = LinuxParser::UpTime(pid_);

  try {
    cpu_utilization_ = float(activejiffes) / float(uptime);

  } catch (...) {
    cpu_utilization_ = 0;
  }

  return cpu_utilization_;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (this->cpu_utilization_ > a.cpu_utilization_) ? true : false;
}