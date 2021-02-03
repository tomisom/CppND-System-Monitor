#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  Processor();
  float Utilization();
  int GetCoreCount();

 private:
  std::vector<int> proc_util_;
  int core_count_;
};

#endif