#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid, std::string user, std::string cmd, std::string uid_, int total_core_count);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  std::string UID() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;
  bool operator==(Process const& a);

  float GetCpuUsage() const;
  float CpuUtilization();

  void Print();

 private:
  int pid_;
  std::string user_;
  std::string command_;
  std::string uid_;
  long prev_active_time_;
  long prev_cpu_time_;
  float cpu_usage_;
  int cpu_core_count_;
};

#endif