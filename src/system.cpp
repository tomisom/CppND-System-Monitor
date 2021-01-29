#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() :
cpu_(),
kernel_(""), os_("")
{}

vector<Process>& System::Processes()
{
    vector<int> vecPids = LinuxParser::Pids();
    processes_.clear();
    for(int p : vecPids) {
        processes_.emplace_back(p, LinuxParser::User(p), LinuxParser::Command(p), LinuxParser::Uid(p));
    }
    std::sort(processes_.begin(), processes_.end());
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