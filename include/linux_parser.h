#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// status fields
enum STATUS_FIELDS {
  FIELD_PID = 0,
  FIELD_COMM,
  FIELD_STATE,
  FIELD_PPID,
  FIELD_PGRP,
  FIELD_SESSION,
  FIELD_TTY_NR,
  FIELD_TPGID,
  FIELD_FLAGS,
  FIELD_MINFLT,
  FIELD_CMINFLT,
  FIELD_MAJFLT,
  FIELD_CMAJFLT,
  FIELD_UTIME,
  FIELD_STIME,
  FIELD_CUTIME,
  FIELD_CSTIME,
  FIELD_PRIORITY,
  FIELD_NICE,
  FIELD_NUM_THREADS,
  FIELD_ITREALVALUE,
  FIELD_STARTTIME,
  FIELD_VSIZE,
  FIELD_RSS,
  FIELD_RSSLIM,
  FIELD_STARTCODE,
  FIELD_ENDCODE,
  FIELD_STARTSTACK,
  FIELD_KSTKESP,
  FIELD_KSTKEIP,
  FIELD_SIGNAL,
  FIELD_BLOCKED,
  FIELD_SIGIGNORE,
  FIELD_SIGCATCH,
  FIELD_WCHAN,
  FIELD_NSWAP,
  FIELD_CNSWAP,
  FIELD_EXIT_SIGNAL,
  FIELD_PROCESSOR,
  FIELD_RT_PRIORITY,
  FIELD_POLICY,
  FIELD_DELAYACCT_BLKIO_TICKS,
  FIELD_GUEST_TIME,
  FIELD_CGUEST_TIME,
  FIELD_START_DATA,
  FIELD_END_DATA,
  FIELD_START_BRK,
  FIELD_ARG_START,
  FIELD_ARG_END,
  FIELD_ENV_START,
  FIELD_ENV_END,
  FIELD_EXIT_CODE
};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_,
  CPUStates_Count
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif