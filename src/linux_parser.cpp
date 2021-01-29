#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <string>
#include <vector>

#include <experimental/filesystem>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

namespace fs = std::experimental::filesystem;

string GetFloatStringWithPrecision(float val, int precision = 5, bool fixed = false)
{
  std::ostringstream out;
  out.precision(precision);
  if(fixed) out << fixed;
  out << val;

  return out.str();
}

string GetStatusFieldForPid(int pid, enum LinuxParser::STATUS_FIELDS field) {
  string val;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatFilename);
  string line;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> isiter(linestream);
    int field_counter = 0;
    while (isiter != std::istream_iterator<string>()) {
      if (field_counter++ == field) {
        break;
      }
      ++isiter;
    }
    if (isiter != std::istream_iterator<string>()) {
      val = *isiter;
    }
  }

  return val;
}

int GetIntValueFromStat(std::string key, int& val) {
  std::ifstream stream(LinuxParser::kProcDirectory +
                       LinuxParser::kStatFilename);
  string line, strprocesses;
  int err = 0;
  bool key_found = false;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      if (line.find(key) != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> strprocesses >> val;
        key_found = true;
      }
    }

    if (!key_found) {
      err = 2;
    }
  } else {
    err = 1;
  }
  return err;
}

int GetStrValuesFromStatusForPid(int pid, std::string key,
                                 std::vector<string>& val) {
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  string line, tempstr;
  int err = 0;
  bool key_found = false;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      if (line.find(key) != std::string::npos) {
        std::istringstream linestream(line);
        key_found = true;

        linestream >> tempstr;
        while (linestream >> tempstr) {
          val.push_back(tempstr);
        }
      }
    }

    if (!key_found) {
      err = 2;
    }
  } else {
    err = 1;
  }
  return err;
}

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

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (auto procdirit = fs::directory_iterator(kProcDirectory);
       procdirit != fs::directory_iterator(); ++procdirit) {
    // check if it's not a directory
    if (fs::is_directory(procdirit->symlink_status())) {
      string filename(procdirit->path().filename());
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }

  return pids;
}

float LinuxParser::MemoryUtilization()
{
  int memtotal = 0, memfree = 0; //, buffers = 0, cached = 0;
  string tmpstr, line, units;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream totallinestream(line);
    totallinestream >> tmpstr >> memtotal >> units;
    std::getline(stream, line);
    std::istringstream freelinestream(line);
    freelinestream >> tmpstr >> memfree >> units;
    //linestream >> tmpstr >> buffers >> units;
    //linestream >> tmpstr >> cached >> units;
  }

  float util = (memtotal - memfree) / (float)memtotal; // percentage utilized
  return util;
}

long LinuxParser::UpTime() {
  long dUptime = 0;
  string uptime;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;

    try {
      dUptime = stol(uptime.c_str());
    } catch (std::exception& e) {
      dUptime = 0;
    }
  }

  return dUptime;
}

long LinuxParser::Jiffies()
{
  vector<string> cpuvec = LinuxParser::CpuUtilization();
  int sum(0);
  try {
    for(uint c = kUser_; c < CPUStates_Count && c < cpuvec.size(); ++c) {
      sum += atoi(cpuvec[c].c_str());
    }
  } catch(std::exception e) {
    sum = 0;
  }
  return sum;
}

long LinuxParser::ActiveJiffies() 
{
  int user, nice, system, irq, softirq, steal;
  long active_jiffies(0);
  vector<string> cpuvec = LinuxParser::CpuUtilization();
  try {
    user = atoi(cpuvec[kUser_].c_str());
    nice = atoi(cpuvec[kNice_].c_str());
    system = atoi(cpuvec[kSystem_].c_str());
    irq = atoi(cpuvec[kIRQ_].c_str());
    softirq = atoi(cpuvec[kSoftIRQ_].c_str());
    steal = atoi(cpuvec[kSteal_].c_str());
    active_jiffies = user + nice + system + irq + softirq + steal;
  } catch( std::exception e) {
    active_jiffies = 0;
  }
  return active_jiffies;
}

long LinuxParser::IdleJiffies()
{
  int idle, iowait;
  long idle_jiffies(0);
  vector<string> cpuvec = LinuxParser::CpuUtilization();
  try {
    idle = atoi(cpuvec[kIdle_].c_str());
    iowait = atoi(cpuvec[kIOwait_].c_str());
    idle_jiffies = idle + iowait;
  } catch( std::exception e) {
    idle_jiffies = 0;
  }
  return idle_jiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, strtemp;
  vector<string> usage;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // the first token is 'cpu', drop it
    linestream >> strtemp;
    // add the remaining tokens in the line to the vector
    while (linestream >> strtemp) {
      usage.push_back(strtemp);
    }
  }
  return usage;
}

int LinuxParser::TotalProcesses() {
  int num_processes = 0;
  GetIntValueFromStat("processes", num_processes);
  return num_processes;
}

int LinuxParser::RunningProcesses() {
  int num_running_processes = 0;
  GetIntValueFromStat("procs_running", num_running_processes);
  return num_running_processes;
}

long LinuxParser::ActiveJiffies(int pid) {
  long utime(0), stime(0), usage(0);
  string strutime = GetStatusFieldForPid(pid, FIELD_UTIME);
  string strstime = GetStatusFieldForPid(pid, FIELD_STIME);
  try {
    utime = stol(strutime);
    stime = stol(strstime);

    usage = (utime + stime);
  } catch (std::exception& e) {
    usage = 0;
  }

  return usage;
}

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string cmdline;
  if (stream.is_open()) {
    std::getline(stream, cmdline);
  }
  return cmdline;
}

string LinuxParser::Ram(int pid) {
  string units;
  int ram_int = 0;
  float ram_float = 0.0;

  vector<string> ramfields;
  int errcode = GetStrValuesFromStatusForPid(pid, "VmSize", ramfields);
  if(errcode == 0) {
    try {
      // the ram value is the first field
      ram_int = std::stoi(ramfields[0]);

      // the units value is the second field
      units = ramfields[1];

      // compute ram_float based on units - we want MB output
      if(units == "B") { // bytes input
        ram_float = ram_int / (1024.0 * 1024.0);  // megabytes output
      } else if(units == "kB") { // kilobytes input
        ram_float = ram_int / 1024.0;  // megabytes output
      } else if(units == "GB") { // gigabytes input
        ram_float = ram_int * 1024.0;  // megabytes output
      }
    } catch (std::exception& e) {
      ram_float = 0.0;
    }
  }

  // convert the float to a string with max 7 digits total and return it
  return GetFloatStringWithPrecision(ram_float, 7);
}

string LinuxParser::Uid(int pid) {
  string struid;
  std::vector<string> values;
  int err = GetStrValuesFromStatusForPid(pid, "Uid", values);
  if (err == 0) {
    struid = values.at(0);
  } else {
    struid = std::to_string(-1);
  }
  return struid;
}

string LinuxParser::User(int pid[[maybe_unused]]) {
  string user;
  string struid = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream tokens(line);
      string val;
      std::vector<string> vals;
      while (std::getline(tokens, val, ':')) {
        vals.push_back(val);
      }

      if (vals.at(2) == struid) {
        user = vals.at(0);
        break;
      }
    }
  }

  return user;
}

long LinuxParser::UpTime(int pid) {
  long starttime = 0;
  string line = GetStatusFieldForPid(pid, FIELD_STARTTIME);
  try {
    starttime = stol(line);
  } catch (std::exception& e) {
    starttime = 0;
  }

  return (UpTime() - (starttime / sysconf(_SC_CLK_TCK)));
}