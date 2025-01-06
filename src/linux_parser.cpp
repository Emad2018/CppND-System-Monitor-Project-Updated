#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>
using std::stof;
using std::string;
using std::to_string;
using std::vector;
#define UTIME_IDX 14
#define STIME_IDX 15
#define CUTIME_IDX 16
#define CSTIME_IDX 17
#define STARTTIME_IDX 22
// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  } else {
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      } else {
      }
    } else {
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key;
  float value, MemTotal, MemAvailable;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key >> value) {
        if (key == "MemTotal:") {
          MemTotal = value;
        } else if (key == "MemAvailable:") {
          MemAvailable = value;
          break;
        } else {
        }
      } else {
      }
    }
  } else {
  }
  return ((float)(MemTotal - MemAvailable) / MemTotal);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long value;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> value;
    } else {
    }
  } else {
  }
  return value;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  int count = 0;
  long total_time, utime, stime, cutime, cstime;
  bool isReache = false;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        count++;
        switch (count) {
          case UTIME_IDX:
            utime = std::stoi(value);
            break;
          case STIME_IDX:
            stime = std::stoi(value);
            break;
          case CUTIME_IDX:
            cutime = std::stoi(value);
            break;
          case CSTIME_IDX:
            cstime = std::stoi(value);
            break;

          default:
            break;
        }
        if (count == CSTIME_IDX) {
          isReache = true;
          break;
        } else {
        }
      }
      if (isReache == true) {
        break;
      }
    }
  } else {
  }
  total_time = utime + stime + cutime + cstime;
  return total_time / sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line, cpu_;
  unsigned long kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_,
      kSteal_, kGuest_, kGuestNice_, totalactive_;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu_ >> kUser_ >> kNice_ >> kSystem_ >> kIdle_ >>
          kIOwait_ >> kIRQ_ >> kSoftIRQ_ >> kSteal_ >> kGuest_ >> kGuestNice_;
      totalactive_ = kUser_ + kNice_ + kSystem_ + kIRQ_ + kSoftIRQ_ + kSteal_;
    } else {
    }
  } else {
  }
  return totalactive_;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line, cpu_;
  unsigned long kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_,
      kSteal_, kGuest_, kGuestNice_, totalidle_;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu_ >> kUser_ >> kNice_ >> kSystem_ >> kIdle_ >>
          kIOwait_ >> kIRQ_ >> kSoftIRQ_ >> kSteal_ >> kGuest_ >> kGuestNice_;
      totalidle_ = kIdle_ + kIOwait_;
    } else {
    }
  } else {
  }
  return totalidle_;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu;
  float CPU_Percentage;

  CPU_Percentage =
      ((float)LinuxParser::ActiveJiffies() / LinuxParser::Jiffies());
  cpu.push_back(std::to_string(CPU_Percentage));
  return cpu;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key) {
        if (key == "processes") {
          linestream >> value;
        } else {
        }
      } else {
      }
    }
  } else {
  }
  return value;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key) {
        if (key == "procs_running") {
          linestream >> value;
        } else {
        }
      } else {
      }
    }
  } else {
  }
  return value;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
    } else {
    }
  } else {
  }
  return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key;
  long value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key) {
        if (key == "VmSize:") {
          linestream >> value;
        } else {
        }
      } else {
      }
    }
  } else {
  }
  value /= 1024;
  return std::to_string(value);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key) {
        if (key == "Uid:") {
          linestream >> value;
        } else {
        }
      } else {
      }
    }
  } else {
  }
  return value;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, user, xchar, uid, value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      if (linestream >> user >> xchar >> value) {
        if (value == LinuxParser::Uid(pid)) {
          break;

        } else {
        }
      } else {
      }
    }
  } else {
  }
  return user;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, key;
  int count = 0;
  bool isReache = false;
  long startTime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        count++;
        if (count == STARTTIME_IDX) {
          isReache = true;
          break;
        } else {
        }
      }
      if (isReache == true) {
        break;
      }
    }
  } else {
  }
  startTime = (long)std::stoi(key) / sysconf(_SC_CLK_TCK);
  return LinuxParser::UpTime() - startTime;
}
