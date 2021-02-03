#include "processor.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;

Processor::Processor()
{
    for(int i = 0; i < LinuxParser::CPUStates_Count; ++i) {
        proc_util_.push_back(0);
    }

    core_count_ = LinuxParser::CpuCoreCount();
}

int Processor::GetCoreCount() { return core_count_; }

float Processor::Utilization()
{
  float utilization = 0.0;
  std::vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  if (cpu_utilization.size() >= LinuxParser::CPUStates_Count) {
    int current_total_util = 0, prev_total_util = 0;

    // calculate the previous total utilization time
    for (int i : proc_util_) {
      prev_total_util += i;
    }

    // add the user, nice, and system time to get the previous total
    int prev_work = proc_util_[LinuxParser::kUser_] + proc_util_[LinuxParser::kNice_] + proc_util_[LinuxParser::kSystem_];

    // clear the previous values from the vector
    proc_util_.clear();

    // add current values to the vector and add them up
    for (string s : cpu_utilization) {
      int u = std::stoi(s);
      current_total_util += u;
      proc_util_.push_back(u);
    }

    // add the user, nice, and system time to get the current total
    int current_work = proc_util_[LinuxParser::kUser_] + proc_util_[LinuxParser::kNice_] + proc_util_[LinuxParser::kSystem_];

    // compute the time in work and total since last update
    int work_in_period = current_work - prev_work;
    int total_in_period = current_total_util - prev_total_util;

    // calculate the utilization as a ratio between current work and total time in period
    utilization = ((float)work_in_period / total_in_period);
  }
  return utilization;
}