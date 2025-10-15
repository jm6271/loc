// loc.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <locale>
#include <filesystem>

#include <CLI/CLI.hpp>


// struct for printing out large numbers with commas
struct comma_numpunct : std::numpunct<char> 
{ 
protected: 
    std::string do_grouping() const override { return "\3"; } 
};

// Extensions for the different programming languages
const std::vector<std::string> C_EXTENSIONS = { ".c", ".h" };
const std::vector<std::string> CPP_EXTENSIONS = { ".cpp", ".h", ".hpp", ".cxx", ".hxx", ".c++", ".cc", ".ixx", ".cppm"};
const std::vector<std::string> PY_EXTENSIONS = { ".py", ".pyw" };
const std::vector<std::string> FS_EXTENSIONS = { ".fs", ".fsx" };
const std::vector<std::string> CS_EXTENSIONS = { ".cs" };
