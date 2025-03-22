#pragma once
class FilteringOptions {
private:
	int userChoice;
	std::string nextHop;
	std::string address;
	std::string fromTime;
	std::string toTime;
public:
	FilteringOptions() : userChoice(0), nextHop(""), address(""), fromTime(""), toTime("") {}
	FilteringOptions(int userChoice, const std::string& nextHop, const std::string& address, const std::string& fromTime, const std::string& toTime) : userChoice(userChoice), nextHop(nextHop), address(address), fromTime(fromTime), toTime(toTime) {}
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
	void setUserChoice(int userChoice) {
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
	;
};