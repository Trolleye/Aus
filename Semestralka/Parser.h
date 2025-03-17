#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "RoutingRecord.h"

class Parser {
public:
    static std::vector<RoutingRecord> parseCSV(const std::string& filename) {
        std::vector<RoutingRecord> records;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return records;
        }

        std::string line;
        bool isFirstLine = true;

        while (std::getline(file, line)) {
            if (isFirstLine) {
                isFirstLine = false;
                continue;
            }
            RoutingRecord record = parseLine(line);
            records.push_back(record);
        }

        file.close();

	
        return records;
    }

private:
    static RoutingRecord parseLine(const std::string& line) {
        std::istringstream ss(line);
        std::string token;

        std::getline(ss, token, ';');
        std::getline(ss, token, ';');
        std::string lifeTime = token;

        std::getline(ss, token, ';');
        std::string prefixAddress = token;

        std::getline(ss, token, ';');
        std::string prefixMask = token;

        std::getline(ss, token, ';');
        std::getline(ss, token, ';');
        std::string nextHop = token;
        size_t viaPos = nextHop.find("via ");
        if (viaPos != std::string::npos) {
            nextHop = nextHop.substr(viaPos + 4);
        }

        // Create and return a RoutingRecord object using the constructor
        return RoutingRecord(lifeTime, prefixAddress, prefixMask, nextHop);
    }
};

#endif // PARSER_H