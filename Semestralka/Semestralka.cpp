#include "Parser.h"
#include <iostream>
#include "Filter.h"
#include <vector>
#include <limits> 
#include "FilteringOptions.h"
#include <cctype>

bool isValidIP(const std::string& ip) {
    std::string octet;
    std::istringstream ss(ip);
    int octetCount = 0;
    while (std::getline(ss, octet, '.')) {
        if (octetCount > 3) {
            return false;
        }

        if (octet.empty() || octet.find_first_not_of("0123456789") != std::string::npos) {
            return false;
        }

        try {
            int octetValue = std::stoi(octet);
            if (octetValue < 0 || octetValue > 255) {
                return false;
            }
        }
        catch (const std::invalid_argument&) {
            return false;
        }
        catch (const std::out_of_range&) {
            return false;
        }
        octetCount++;
    }
    return octetCount == 4;
}

bool isValidTime(const std::string& time) {
    if (time.find(':') != std::string::npos) {
        std::string timeUnit;
        std::istringstream ss(time);
        int timeCount = 0;

        while (std::getline(ss, timeUnit, ':')) {
            if (timeCount > 2) {
                return false;
            }

            if (timeUnit.empty() || timeUnit.find_first_not_of("0123456789") != std::string::npos) {
                return false;
            }

            int timeValue = std::stoi(timeUnit);

            if (timeCount == 0 && (timeValue < 0 || timeValue > 23)) {
                return false;
            }
            else if ((timeCount == 1 || timeCount == 2) && (timeValue < 0 || timeValue > 59)) {
                return false;
            }

            timeCount++;
        }

        return timeCount == 3;
    }
    else if (time.find('w') != std::string::npos || time.find('d') != std::string::npos || time.find('h') != std::string::npos) {
        std::istringstream ss(time);
        char unit;
        int value;
        bool hasValidUnit = false;

        while (ss >> value >> unit) {
            if (unit == 'w' || unit == 'd' || unit == 'h') {
                if (value < 0) {
                    return false;
                }
                hasValidUnit = true;
            }
            else {
                return false;
            }
        }

        return hasValidUnit;
    }
    else {
        return false;
    }
}


FilteringOptions promptUser() {
    FilteringOptions filterOptions;
    int userChoice;
    bool validChoice = false;

    int userMask;
    std::string userNextHop;
    std::string userAddress;
    std::string userFromTime;
    std::string userToTime;

    while (!validChoice) {
        std::cout << "Choose your filter options:" << std::endl;
        std::cout << "1. Filter by next hop" << std::endl;
        std::cout << "2. Filter by address" << std::endl;
        std::cout << "3. Filter by lifetime" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> userChoice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter an integer." << std::endl;
        }
        else {
            switch (userChoice) {
            case 1:
                std::cout << "Enter next hop: ";
                std::cin >> userNextHop;
                if (!isValidIP(userNextHop)) {
                    std::cout << "Invalid IP address. Please try again." << std::endl;
                }
                else {
                    filterOptions.setUserChoice(userChoice);
                    filterOptions.setNextHop(userNextHop);
                    validChoice = true;
                }
                break;
            case 2:
                std::cout << "Enter address: ";
                std::cin >> userAddress;
                std::cout << "Enter mask (0-32): ";
                while (!(std::cin >> userMask) || userMask < 0 || userMask > 32) {
                    std::cout << "Invalid mask. Please enter a number between 0 and 32: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                if (!isValidIP(userAddress)) {
                    std::cout << "Invalid IP address. Please try again." << std::endl;
                }
                else {
                    filterOptions.setUserChoice(userChoice);
                    filterOptions.setAddress(userAddress);
                    filterOptions.setMask(userMask);
                    validChoice = true;
                }
                break;
            case 3:
                std::cout << "Enter from time: ";
                std::cin >> userFromTime;
                std::cout << "Enter to time: ";
                std::cin >> userToTime;
                if (!isValidTime(userFromTime) || !isValidTime(userToTime)) {
                    std::cout << "Invalid time. Please try again." << std::endl;
                }
                else {
                    filterOptions.setUserChoice(userChoice);
                    filterOptions.setFromTime(userFromTime);
                    filterOptions.setToTime(userToTime);
                    validChoice = true;
                }
                break;
            case 0:
                exit(0);
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
            }
        }
    }
    return filterOptions;
}

int main() {
    std::vector<RoutingRecord> records = Parser::parseCSV("RT.csv");
    bool continueFiltering = true;

    while (continueFiltering) {
        FilteringOptions options = promptUser();

        int userMask = options.getMask();
        std::string userNextHop = options.getNextHop();
        std::string userAddress = options.getAddress();
        std::string userFromTime = options.getFromTime();
        std::string userToTime = options.getToTime();

        auto matchNextHopPred = [&](RoutingRecord& record) { return record.matchNextHop(userNextHop); };
        auto matchWithAddressPred = [&](RoutingRecord& record) {return record.matchWithAddress(userAddress, userMask); };
        auto matchLifetimePred = [&](RoutingRecord& record) {return record.matchLifeTime(userFromTime, userToTime); };

        std::vector<RoutingRecord> filteredRecords;

        switch (options.getUserChoice())
        {
        case 1:
            filteredRecords = Filter::filter(records.begin(), records.end(), matchNextHopPred);
            break;
        case 2:
            filteredRecords = Filter::filter(records.begin(), records.end(), matchWithAddressPred);
            break;
        case 3:
            filteredRecords = Filter::filter(records.begin(), records.end(), matchLifetimePred);
            break;
        default:
            break;
        }


        std::cout << std::endl;
        for (RoutingRecord& record : filteredRecords) {
            std::cout << record.getInfo() << std::endl;
        }
        std::cout << std::endl;


        char response;
        std::cout << "Do you want to apply another filter? (y/n): ";
        std::cin >> response;

        if (response == 'n' || response == 'N') {
            continueFiltering = false;
            break;
        }
        std::cout << std::endl;

        std::cout << "Do you want to (a/b):" << std::endl << "A) filter the original table" << std::endl << "B) filter the already filtered table: ";
        std::cin >> response;

        if (response == 'a' || response == 'A') {
            records = Parser::parseCSV("RT.csv");
        }
        else {
            records = filteredRecords;
        }
        std::cout << std::endl;


    }
    return 0;
}