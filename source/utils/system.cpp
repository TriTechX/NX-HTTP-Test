#include <switch.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sys/stat.h>

std::string getCurrentTime(){
    // get current time
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);

    // format as HH:MM
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
       << std::setw(2) << std::setfill('0') << t->tm_min;

    return ss.str();
}


std::string getSystemFirmware(){
    std::string sysVer = "Firmware: ";
    
    // initialise set sys first!!!!!
    Result rc = setsysInitialize();

    if (R_FAILED(rc)){
        return sysVer+"ER.RO.R?";
    }

    // get the firmware version
    SetSysFirmwareVersion firmware;
    if (R_SUCCEEDED(setsysGetFirmwareVersion(&firmware))){
        sysVer += firmware.display_version;
    }
    else {
        sysVer += "??.??.??";
    }

    // exit setsys
    setsysExit();

    return sysVer;
}

void writeCSV(const std::string& path, const std::vector<std::string>& row){
    // make the ttdata directory
    mkdir("sdmc:/ttdata", 0777);
    
    std::ofstream file(path, std::ios::out);

    for (long unsigned int i = 0; i < row.size(); i++){
        file << row[i];
        if (i < row.size() - 1) file << ",";
    }

    file << "\n";
    file.close();
}

std::vector<std::string> readCSVRow(const std::string& path, const int row){
    // start an input file stream for the provided path
    std::ifstream file(path);

    std::string line;
    int currentRow = 0;

    while(std::getline(file, line)){
        if (currentRow == row){
            // found the row, split by comma
            std::vector<std::string> result;
            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, ',')){ // stream, string to output to, deliminator (separator)
                // iteration 0 (before processing), ss is host ip, host port
                // iteration 1, ss is now just host port
                // iteration 2, ss is empty
                // when contents are popped from ss, they are pushed to cell
                result.push_back(cell);
            }

            return result;
        }
    }

    currentRow++;

    // return empty row if index out of range
    return {};
}

void log(const std::string& msg){
    svcOutputDebugString(msg.c_str(), msg.size());
}