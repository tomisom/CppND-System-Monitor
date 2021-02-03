#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include <assert.h>

#include <iostream>
#include <fstream>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
using std::ios;

System::System() :
cpu_(), processes_(), kernel_(""), os_("")
{}

vector<Process>& System::Processes()
{
    // get the current PID vector
    vector<int> vecCurPids = LinuxParser::Pids();
    vector<Process> vecProcesses;

    // for each process, if the pid is in vecCurPids, update the cpu utilization
    // if it's not in vecCurPids, delete it
    for(Process& proc : processes_) {
        // if no more pids in the vector, break out of the loop
        if(vecCurPids.size() == 0) break;

        // search for the processes pid in the pid vector
        auto pidIt = find_if(vecCurPids.begin(), vecCurPids.end(), [&proc](const int& pid) {return pid == proc.Pid();});

        if(pidIt != vecCurPids.end()) {
            proc.CpuUtilization();
            vecProcesses.push_back(proc);
        }

        // remove the pid from the vector
        vecCurPids.erase(pidIt);
    }

    //for any pids left in vecCurPids, add the process to the vector
    for(int pid : vecCurPids) {
        vecProcesses.emplace_back(pid, LinuxParser::User(pid), LinuxParser::Command(pid), LinuxParser::Uid(pid), cpu_.GetCoreCount());
    }

    // sort the processes_ vector twice, once to order by pid (ascending), and again using stable_sort to order by cpu_usage_
    // this will keep the pids in order for matching usage amounts, making the display much more stable
    std::sort(vecProcesses.begin(), vecProcesses.end(), [ ](const Process& lhs, const Process& rhs) { return lhs.Pid() < rhs.Pid() ; });

    // sort the vector again to sort by cpu_usage_ (uses operator< overload to compare cpu usage)
    std::stable_sort(vecProcesses.rbegin(), vecProcesses.rend());

    processes_.clear();
    processes_.assign(vecProcesses.begin(), vecProcesses.end());
    return processes_;
}

std::string System::Kernel()
{
    if(kernel_.empty()) {
        kernel_ = LinuxParser::Kernel();
    }
    return kernel_;
}

std::string System::OperatingSystem()
{
    if(os_.empty()) {
        os_ = LinuxParser::OperatingSystem();
    }
    return os_;
}

Processor& System::Cpu() { return cpu_; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }