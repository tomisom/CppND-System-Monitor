#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <limits.h>

#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { return 0.0; }

long LinuxParser::UpTime() 
{
    long dUptime = 0;
    string uptime;
    string line;

    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if(stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime;

        try {
            dUptime = stol(uptime.c_str());
        } catch(std::exception& e) {
            dUptime = 0;
        }
    }

    return dUptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{ 
    return 0;
}

string GetStatusFieldForPid(int pid, enum LinuxParser::STATUS_FIELDS field)
{
    string val;

    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
    string line;

    if(stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        std::istream_iterator<string> isiter(linestream);
        int field_counter = 0;
        while(isiter != std::istream_iterator<string>()) {
            if(field_counter++ == field) {
                break;
            }
            ++isiter;
        }
        if(isiter != std::istream_iterator<string>()) {
            val = *isiter;
        }
    }

    return val;
}

long LinuxParser::ActiveJiffies(int pid)
{
    /// TODO: rework this function to track active jiffies, not total uptime
    /// use CPUStates to pull data from /proc/pid/stat
    long total_jiffies = 0;
    string line = GetStatusFieldForPid(pid, FIELD_STARTTIME);
    try {
        total_jiffies = stol(line);
    } catch(std::exception& e) {
        total_jiffies = 0;
    }

    return (UpTime() - (total_jiffies / sysconf(_SC_CLK_TCK)));
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() {
    std::ifstream stream(kProcDirectory + kStatFilename);
    string line, strtemp;
    vector<string> usage;
    if(stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> strtemp;
        while(linestream >> strtemp) {
            usage.push_back(strtemp);
        }
    }
    return usage;
}

int GetIntValueFromStat(std::string key, int& val)
{
    std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    string line, strprocesses;
    int err = 0;
    bool key_found = false;
    if(stream.is_open()) {
        while(std::getline(stream, line)) {
            if(line.find(key) != std::string::npos) {
                std::istringstream linestream(line);
                linestream >> strprocesses >> val;
                key_found = true;
            }
        }

        if(!key_found) {
            err = 2;
        }
    } else {
        err = 1;
    }
    return err;
}

int GetStrValuesFromStatusForPid(int pid, std::string key, std::vector<string>& val)
{
    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatusFilename);
    string line, tempstr;
    int err = 0;
    bool key_found = false;
    if(stream.is_open()) {
        while(std::getline(stream, line)) {
            if(line.find(key) != std::string::npos) {
                std::istringstream linestream(line);
                key_found = true;

                linestream >> tempstr;
                while(linestream >> tempstr) {
                    val.push_back(tempstr);
                }
            }
        }

        if(!key_found) {
            err = 2;
        }
    } else {
        err = 1;
    }
    return err;
}

int LinuxParser::TotalProcesses() 
{
    int num_processes = 0;
    GetIntValueFromStat("processes", num_processes);
    return num_processes;
}

int LinuxParser::RunningProcesses()
{ 
    int num_running_processes = 0;
    GetIntValueFromStat("procs_running", num_running_processes);
    return num_running_processes;
}

string LinuxParser::Command(int pid) 
{
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    string cmdline;
    if(stream.is_open()) {
        std::getline(stream, cmdline);
    }
    return cmdline;
}

string LinuxParser::Ram(int pid)
{
    string ram;
    string ramfield = GetStatusFieldForPid(pid, FIELD_VSIZE);

    int ram_int = 0;
    std::string units(" B");
    try{
        ram_int = std::stoi(ramfield);

        if(ram_int > 1024) {
            ram_int /= 1024; // kilobytes
            if(ram_int < 1024) {
                units = " KB";
            } else {
                ram_int /= 1024;
                units = " MB";
            }
        }
    } catch( std::exception& e) {
        ram_int = 0;
        units = "";
    }
    ram = std::to_string(ram_int);
    ram.append(units);

    return ram;
}

string LinuxParser::Uid(int pid) 
{
    string struid;
    std::vector<string> values;
    int err = GetStrValuesFromStatusForPid( pid, "Uid", values);
    if(err == 0) {
        struid = values.at(0);
    } else {
        struid = -1;
    }
    return struid;
}

string LinuxParser::User(int pid[[maybe_unused]]) 
{
    string user;
    string struid = Uid(pid);

    std::ifstream stream(kPasswordPath);
    if(stream.is_open()) {
        string line;
        while(std::getline(stream, line)) {
            std::istringstream tokens(line);
            string val;
            std::vector<string> vals;
            while(std::getline(tokens, val, ':')) {
                vals.push_back(val);
            }

            if(vals.at(2) == struid) {
                user = vals.at(0);
                break;
            }
        }
    }

    return user;
}

long LinuxParser::UpTime(int pid)
{
    long uptime = 0;
    string line = GetStatusFieldForPid(pid, FIELD_STARTTIME);
    try {
        uptime = stol(line);
    } catch(std::exception& e) {
        uptime = 0;
    }

    return (UpTime() - (uptime / sysconf(_SC_CLK_TCK)));
}