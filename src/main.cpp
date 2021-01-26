#include "ncurses_display.h"
#include "system.h"
#include <iostream>
#include <vector>
#include <string>

#include "linux_parser.h"

int main() {
  System system;

  long dUptime = LinuxParser::UpTime();
  std::cout << "uptime [ main]: " << dUptime << std::endl;

  long dPid1Uptime = LinuxParser::ActiveJiffies(1);
  std::cout << "uptime [pid 1]: " << dPid1Uptime << std::endl;

    std::vector<std::string> usage = LinuxParser::CpuUtilization();
    std::cout << "cpu usage: ";
    for(std::string s : usage) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    int num_processes = LinuxParser::TotalProcesses();
    std::cout << "NumProcs: " << num_processes << std::endl;

    int num_running_processes = LinuxParser::RunningProcesses();
    std::cout << "RunningProcs: " << num_running_processes << std::endl;

    std::string cmdline_pid1 = LinuxParser::Command(1);
    std::cout << "cmdline[pid 1]: " << cmdline_pid1 << std::endl;

    std::string ram = LinuxParser::Ram(1);
    std::cout << "ram[pid 1]: " << ram << std::endl;
 
    std::string uid = LinuxParser::Uid(1);
    std::cout << "uid[pid 1]: " << uid << std::endl;
 
    std::string user = LinuxParser::User(1);
    std::cout << "user[pid 1]: " << user << std::endl;

    long uptime = LinuxParser::UpTime(1);
    std::cout << "uptime[pid 1]: " << uptime << std::endl;

    //NCursesDisplay::Display(system);
}