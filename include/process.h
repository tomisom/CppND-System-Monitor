#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid, std::string user, std::string cmd, std::string uid_);
  int Pid();
  std::string User();
  std::string Command();
  std::string UID();
  float CpuUtilization() const;
  std::string Ram();
  long int UpTime() const;
  bool operator<(Process const& a) const;

 private:
  int pid_;
  std::string user_;
  std::string command_;
  std::string uid_;
};

#endif