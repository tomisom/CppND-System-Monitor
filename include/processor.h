#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  Processor();
  float Utilization();

 private:
  std::vector<int> proc_util_;
};

#endif