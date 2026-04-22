#pragma once
#include <switch.h>
#include <vector>

std::string getCurrentTime();
std::string getSystemFirmware();
void writeCSV(const std::string& path, const std::vector<std::string>& row);
std::vector<std::string> readCSVRow(const std::string& path, const int row);
void log(const std::string& msg);