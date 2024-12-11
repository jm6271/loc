// loc.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <locale>

#include <boost/program_options.hpp>

#include "Counter.h"


// struct for printing out large numbers with commas
struct comma_numpunct : std::numpunct<char> 
{ 
protected: 
    virtual std::string do_grouping() const override { return "\3"; } 
};
