#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <array>
class RoutingRecord{
private:
	std::string lifeTime;
	std::string prefixAddress;
	std::string prefixMask;
	std::string nextHop;
	int lifeTimeSec;
	int prefixMaskInt;
	int toSec(std::string& paLifeTime);
	std::array<int, 32> toBinary(const std::string& paAddress);
public:
	bool matchWithAddress(std::string& paAdress, int& paMask);
	bool matchLifeTime(std::string& fromTime, std::string& toTime);
	bool matchNextHop(std::string& paNextHop);
	int getMask();
	uint32_t getIP();
	int getLifeTime();
	std::string getInfo();
	std::string getIPAddress();
	std::string getNextHop() { return this->nextHop; }
	RoutingRecord();
	RoutingRecord(std::string lifeTime, std::string prefixAddress, std::string prefixMask, std::string nextHop);
};

inline RoutingRecord::RoutingRecord()
{
	this->lifeTime = "";
	this->lifeTimeSec = 0;
	this->prefixAddress = "";
	this->prefixMask = "";
	this->nextHop = "";
	this->prefixMaskInt = 0;
}


inline RoutingRecord::RoutingRecord(std::string lifeTime, std::string prefixAddress, std::string prefixMask, std::string nextHop)
{
	this->lifeTime = lifeTime;
	this->lifeTimeSec = toSec(lifeTime);
	this->prefixAddress = prefixAddress;
	this->prefixMask = prefixMask;
	this->nextHop = nextHop;
	if (prefixMask == "") {
		this->prefixMaskInt = 0;
	}
	else {
		this->prefixMaskInt = std::stoi(prefixMask);
	}
}



inline bool RoutingRecord::matchWithAddress(std::string& paAddress, int& paMask)
{
	if (paMask == 0) {
		return true;
	}
	
	std::array<int, 32> myAddress = this->toBinary(this->prefixAddress);
	std::array<int, 32> otherAddress = this->toBinary(paAddress);

	for (int i = 0; i < paMask; i++) {
		if (myAddress[i] != otherAddress[i]) {
			return false;
		}
	}
	return true;
}

inline bool RoutingRecord::matchLifeTime(std::string& fromTime, std::string& toTime)
{
	if (this->toSec(toTime) >= this->lifeTimeSec && this->lifeTimeSec >= this->toSec(fromTime))
	{
		return true;
	}
	return false;
}

inline bool RoutingRecord::matchNextHop(std::string& paNextHop)
{
	return paNextHop == this->nextHop;
}

inline int RoutingRecord::getMask()
{
	return this->prefixMaskInt;
}

inline uint32_t RoutingRecord::getIP()
{
	std::string ip = this->prefixAddress;
	uint32_t a, b, c, d;
	char dot;
	std::istringstream iss(ip);
	iss >> a >> dot >> b >> dot >> c >> dot >> d;
	return (a << 24) | (b << 16) | (c << 8) | d;
}

inline int RoutingRecord::getLifeTime()
{
	return toSec(this->lifeTime);
}

inline std::string RoutingRecord::getInfo()
{
	return "LifeTime: " + this->lifeTime + " Prefix Address: " + this->prefixAddress + " Prefix Mask: " + this->prefixMask + " Next Hop: " + this->nextHop;
}

inline std::string RoutingRecord::getIPAddress()
{
	return this->prefixAddress;
}

inline int RoutingRecord::toSec(std::string& paLifeTime)
{
	int sec = 0;
	std::string temp = "";

	if (paLifeTime.find(':') != std::string::npos) {
		size_t colon1 = paLifeTime.find(':');
		size_t colon2 = paLifeTime.find(':', colon1 + 1);

		int hours = std::stoi(paLifeTime.substr(0, colon1));
		int minutes = std::stoi(paLifeTime.substr(colon1 + 1, colon2 - (colon1 + 1)));
		int seconds = std::stoi(paLifeTime.substr(colon2 + 1));

		sec = hours * 3600 + minutes * 60 + seconds;
	}
	else {
		for (size_t i = 0; i < paLifeTime.size(); i++) {
			if (paLifeTime[i] == 'w') {
				sec += std::stoi(temp) * 604800;
				temp = "";
			}
			else if (paLifeTime[i] == 'd') {
				sec += std::stoi(temp) * 86400;
				temp = "";
			}
			else if (paLifeTime[i] == 'h') {
				sec += std::stoi(temp) * 3600;
				temp = "";
			}
			else if (paLifeTime[i] == 'm') {
				sec += std::stoi(temp) * 60;
				temp = "";
			}
			else if (paLifeTime[i] == 's') {
				sec += std::stoi(temp);
				temp = "";
			}
			else {
				temp += paLifeTime[i];
			}
		}
	}

	return sec;
}


inline std::array<int, 32> RoutingRecord::toBinary(const std::string& paAddress) {
	std::array<int, 32> bitArray{};
	std::stringstream ss(paAddress);
	std::string segment;
	int index = 0;

	while (std::getline(ss, segment, '.')) {
		int octet = std::stoi(segment);
		std::bitset<8> bits(octet);
		for (int i = 7; i >= 0; --i) {
			bitArray[index++] = bits[i];
		}
	}

	return bitArray;
}
