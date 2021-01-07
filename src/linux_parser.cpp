#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string mem_type, total_mem, free_mem,line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream1(line);
    linestream1 >> mem_type >> total_mem;


    std::getline(stream, line);
    std::istringstream linestream2(line);
    std::replace(line.begin(), line.end(), ':', ' ');
    linestream2 >> mem_type >> free_mem;
  }
  stream.close(); // Close File

  // (Total-free)/(Total) = Used/Total
  float usage_percentage = (stof(total_mem) - stof(free_mem) )/stof(total_mem);

  return usage_percentage; 
}

// Done: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, idetime, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idetime;
  }
  return long(stof(uptime)); 
}

// TODO: Read and return the number of jiffies for the system
//long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return CPU utilization
//vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
//long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Done: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(const vector<long> &jiff) { 
  return jiff[kUser_]+jiff[kNice_]+jiff[kSystem_]+jiff[kIRQ_]+jiff[kSoftIRQ_]+jiff[kSteal_]+jiff[kGuest_]+jiff[kGuestNice_];
}

// Done: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(const vector<long> &jiff) { 
  return jiff[kIdle_]+jiff[kIOwait_]; 
}

vector<long> LinuxParser::AllJiffies(string path){
  string cpu,line,present_jiff;

  vector<long> jiff;
  std::ifstream stream(path);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;

    for(int i = 0; i<10; i++){
      linestream >> present_jiff;
      jiff.push_back(stol(present_jiff));
      present_jiff = "";
    }

  }

  return jiff;
}

float LinuxParser::SystemCpuUtilization(){
  vector<long> jiff_time_1 = AllJiffies(kProcDirectory + kStatFilename);
  float jiff_ide_1 = IdleJiffies(jiff_time_1);
  float jiff_active_1 = ActiveJiffies(jiff_time_1);

  // Sleep for 250 ms
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  vector<long> jiff_time_2 = AllJiffies(kProcDirectory + kStatFilename);
  float jiff_ide_2 = IdleJiffies(jiff_time_2);
  float jiff_active_2 = ActiveJiffies(jiff_time_2);

  float delta_ide_jiff = abs(jiff_ide_2 - jiff_ide_1);
  float delta_active_jiff = abs(jiff_active_2 - jiff_active_1);

  float delta_total_jiff = delta_active_jiff+delta_ide_jiff;

  return delta_active_jiff/delta_total_jiff;
}

long LinuxParser::ProcessCpuTime(int pid){
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  stream.close();

  string var, utime,stime,cutime,cstime;
  std::istringstream linestream(line);
  for(int i = 0; i < 13;i++){
    linestream >> var;
  }

  linestream >> utime >> stime >> cutime >> cstime;


  return stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
}

// Done: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return int(stof(value));
        }
      }
    }
  }
  return int(); 
}

// Done: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return int(stof(value));
        }
      }
    }
  }
  return int();
}

// Done: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);

  }
  return line; 
}

// Done: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize") {
        return std::to_string(stol(value)/1000); // Convert KB to MB
      }
      
    }
  }
  return string();  

}

// Done: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid") {
        return value;
      }
      
    }
  }
  return string();  
}

// Done: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string line;
  string value,var,key;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> value >> var >> key;
      if (key == uid) {
        return value;
      }
      
    }
  }
  return string();  
}

// Done: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }

  string clock_tick;
  std::istringstream linestream(line);
  for(int i = 0; i < 22;i++){
    linestream >> clock_tick;
  }
  return stol(clock_tick)/sysconf(_SC_CLK_TCK); 
}