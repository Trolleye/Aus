#include "Parser.h"
#include <iostream>
#include "Semestralka.h"

int main() {
    std::vector<RoutingRecord> records = Parser::parseCSV("RT.csv");

    return 0;
}