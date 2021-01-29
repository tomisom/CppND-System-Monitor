#include <sstream>
#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int dHours = seconds / (60 * 60);
  int dMinutes = (seconds % (60 * 60)) / 60;
  int dSeconds = (seconds % 60);

  std::ostringstream string_stream;
  if(dHours < 10) string_stream << "0";
  string_stream << dHours << ":";
  if(dMinutes < 10) string_stream << "0";
  string_stream << dMinutes << ":";
  if(dSeconds < 10) string_stream << "0";
  string_stream << dSeconds;

  return string_stream.str();
}