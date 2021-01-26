#include <string>
#include <sstream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{
    int dHours = seconds / (60*60);
    int dMinutes = (seconds % (60*60)) / 60;
    int dSeconds = (seconds % 60);

    std::ostringstream stringStream;
    stringStream << dHours << ":" << dMinutes << ":" << dSeconds;
    
    return stringStream.str();
}