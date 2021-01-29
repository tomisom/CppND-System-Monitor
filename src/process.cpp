#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, std::string user, std::string cmd, std::string uid) :
    pid_(pid), user_(user), command_(cmd), uid_(uid)
{ }

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const
{
    long total_time = LinuxParser::ActiveJiffies(pid_);
    float cpu_usage = 100 * ((float)total_time / sysconf(_SC_CLK_TCK) / UpTime());
    return cpu_usage;
}

string Process::Command() { return command_; }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return user_; }

std::string Process::UID() { return uid_; }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const { return CpuUtilization() > a.CpuUtilization(); }