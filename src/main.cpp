#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "ncurses_display.h"
#include "system.h"

#include "linux_parser.h"

#define DEBUG_OUTPUT 0

int main() 
{
  System system;

#if DEBUG_OUTPUT == 1
  std::cout << "OS: "               << system.OperatingSystem() << std::endl;
  std::cout << "Kernel: "           << system.Kernel() << std::endl;
  std::cout << "System Uptime: "    << std::to_string(system.UpTime()) << std::endl;
  std::cout << "CPU Utilization: "  << std::to_string(system.Cpu().Utilization()*100) << "%" << std::endl;
  std::cout << "Mem Utilization: "  << std::to_string(system.MemoryUtilization()*100) << "%" << std::endl;
  std::cout << "NumProcs: "         << std::to_string(system.TotalProcesses()) << std::endl;
  std::cout << "RunningProcs: "     << std::to_string(system.RunningProcesses()) << std::endl;
  std::cout << "Total Jiffies: "    << std::to_string(LinuxParser::Jiffies()) << std::endl;
  std::cout << "Active Jiffies: "   << std::to_string(LinuxParser::ActiveJiffies()) << std::endl;
  std::cout << "Idle Jiffies: "     << std::to_string(LinuxParser::IdleJiffies()) << std::endl;

  std::cout << "Process List:" << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  for(auto p : system.Processes()) {
    std::string strpid(std::to_string(p.Pid()));

    std::cout << "    cmdline[pid " << strpid << "]: " << p.Command() << std::endl;
    std::cout << "    ram[pid "     << strpid << "]: " << p.Ram() << std::endl;
    std::cout << "    user[pid "    << strpid << "]: " << p.User() << std::endl;
    std::cout << "    uid[pid "     << strpid << "]: " << p.UID() << std::endl;
    std::cout << "    uptime[pid "  << strpid << "]: " << std::to_string(LinuxParser::UpTime(p.Pid())) << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
  }
#endif

  NCursesDisplay::Display(system);
}