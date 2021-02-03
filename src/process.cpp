#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, std::string user, std::string cmd, std::string uid, int total_core_count) :
    pid_(pid), user_(user), command_(cmd), uid_(uid), 
    prev_active_time_(0), prev_cpu_time_(0), cpu_usage_(0.0f), cpu_core_count_(total_core_count)
{ }

int Process::Pid() const { return pid_; }

float Process::GetCpuUsage() const { return cpu_usage_; }

float Process::CpuUtilization()
{
    // get the current time values
    long cur_proc_active_time = LinuxParser::ActiveJiffies(pid_);
    long cur_cpu_time = LinuxParser::Jiffies();

    // compute the time deltas
    long cpu_time_delta = cur_cpu_time - prev_cpu_time_;
    long active_time_delta = cur_proc_active_time - prev_active_time_;

    // compute the cpu-usage using the deltas
    // note - if uptime delta is too small, leave usage as-is
    // based on htop source
    if(cpu_time_delta > 1e-6) {
        cpu_usage_ = cpu_core_count_ * (float)active_time_delta / (float)cpu_time_delta;
        
        // save the current time values
        prev_active_time_ = cur_proc_active_time;
        prev_cpu_time_ = cur_cpu_time;
    }

    // limit to 1 decimal place (result is later multiplied by 100 for display)
    int cpu_use = cpu_usage_ * 1000;
    cpu_usage_ = (cpu_use / 1000.0f);

    return cpu_usage_;
}

string Process::Command() const { return command_; }

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() const { return user_; }

std::string Process::UID() const { return uid_; }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

void Process::Print() 
{
    std::cout << "    cmdline[pid " << pid_ << "]: " << this->Command() << std::endl;
    std::cout << "    ram[pid "     << pid_ << "]: " << this->Ram() << std::endl;
    std::cout << "    user[pid "    << pid_ << "]: " << this->User() << std::endl;
    std::cout << "    uid[pid "     << pid_ << "]: " << this->UID() << std::endl;
    std::cout << "    cpu[pid "     << pid_ << "]: " << this->CpuUtilization() << std::endl;
    std::cout << "    uptime[pid "  << pid_ << "]: " << std::to_string(this->UpTime()) << std::endl;
}

bool Process::operator<(Process const& a) const { return (this->cpu_usage_ < a.cpu_usage_); }
bool Process::operator==(Process const& a) { return (this->pid_ == a.pid_); }