#include <unistd.h>
#include <cctype>
#include <sstream>
#include <unistd.h>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    this->pid_ = pid;
}

// DOne: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    long process_total_tick = LinuxParser::ProcessCpuTime(this->pid_);
    long process_total_sec = process_total_tick/sysconf(_SC_CLK_TCK);
    return float(process_total_sec)/UpTime(); 
}

// Done: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(this->pid_); 
}

// Done: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(this->pid_); 
}

// Done: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(this->pid_); 
}

// Done: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime()-LinuxParser::UpTime(this->pid_); 
}

// Done: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process & a)  { 
    return CpuUtilization() > a.CpuUtilization(); 
}