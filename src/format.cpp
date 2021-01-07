#include <string>
#include <sstream>
#include <iomanip> 

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    std::stringstream formater;
    
    int ss = seconds%60;
    formater<<std::setw(2)<<std::setfill('0')<<ss;
    string SS = formater.str();
    formater.str(""); // Clears the Stream

    int minutes = seconds/60;
    int mm = minutes%60;
    formater<<std::setw(2)<<std::setfill('0')<<mm;
    string MM = formater.str();
    formater.str(""); // Clears the Stream

    int hh = minutes/60;
    formater<<std::setw(2)<<std::setfill('0')<<hh;
    string HH = formater.str();
    formater.str(""); // Clears the Stream

    string HH_MM_SS = HH+":"+MM+":"+SS;

    return HH_MM_SS;    
}