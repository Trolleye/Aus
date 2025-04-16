#pragma once
#include <string>
#include <iostream>
#include <limits>
#include <sstream>
class FilteringOptions {
private:
	int userChoice;
	int userMask;
	std::string nextHop;
	std::string address;
	std::string fromTime;
	std::string toTime;
public:
	FilteringOptions() : userChoice(0), userMask(0), nextHop(""), address(""), fromTime(""), toTime("") {}
	FilteringOptions(int userChoice, int userMask,const std::string& nextHop, const std::string& address, const std::string& fromTime, const std::string& toTime) : userChoice(userChoice), userMask(userMask), nextHop(nextHop), address(address), fromTime(fromTime), toTime(toTime) {}
	int getUserChoice() const {
		return userChoice;
	}
	std::string getNextHop() const {
		return nextHop;
	}
	std::string getAddress() const {
		return address;
	}
	std::string getFromTime() const {
		return fromTime;
	}
	std::string getToTime() const {
		return toTime;
	}
	int getMask() const {
		return this->userMask;
	}
	void setMask(int& mask) {
		this->userMask = mask;
	}
	void setUserChoice(int& userChoice) {
		this->userChoice = userChoice;
	}
	void setNextHop(const std::string& nextHop) {
		this->nextHop = nextHop;
	}
	void setAddress(const std::string& address) {
		this->address = address;
	}
	void setFromTime(const std::string& fromTime) {
		this->fromTime = fromTime;
	}
	void setToTime(const std::string& toTime) {
		this->toTime = toTime;
	}
	void printAll() {
		std::cout << "User choice: " << userChoice << std::endl;
		std::cout << "Next hop: " << nextHop << std::endl;
		std::cout << "Address: " << address << std::endl;
		std::cout << "From time: " << fromTime << std::endl;
		std::cout << "To time: " << toTime << std::endl;
	}
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

	;
};