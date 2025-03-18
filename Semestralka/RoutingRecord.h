#pragma once
#include <iostream>
#include <string>
class RoutingRecord{
private:
	std::string lifeTime;
	std::string prefixAddress;
	std::string prefixMask;
	std::string nextHop;
	int lifeTimeSec;
public:
	bool matchWithAddress(std::string& address, int mask);
	bool matchLifeTime(std::string& fromTime, std::string& toTime);
	bool matchNextHop(std::string& paNextHop);
	std::string getInfo();
	int toSec(std::string& paLifeTime);
	RoutingRecord();
	RoutingRecord(std::string lifeTime, std::string prefixAddress, std::string prefixMask, std::string nextHop);
};

RoutingRecord::RoutingRecord()
{
	this->lifeTime = "";
	this->lifeTimeSec = 0;
	this->prefixAddress = "";
	this->prefixMask = "";
	this->nextHop = "";
}


RoutingRecord::RoutingRecord(std::string lifeTime, std::string prefixAddress, std::string prefixMask, std::string nextHop)
{
	this->lifeTime = lifeTime;
	this->lifeTimeSec = toSec(lifeTime);
	this->prefixAddress = prefixAddress;
	this->prefixMask = prefixMask;
	this->nextHop = nextHop;
}



bool RoutingRecord::matchWithAddress(std::string& address, int mask)
{
	return false;
}

bool RoutingRecord::matchLifeTime(std::string& fromTime, std::string& toTime)
{
	if (this->toSec(toTime) >= this->lifeTimeSec && this->lifeTimeSec >= this->toSec(fromTime))
	{
		return true;
	}
	return false;
}

bool RoutingRecord::matchNextHop(std::string& paNextHop)
{
	return paNextHop == this->nextHop;
}

std::string RoutingRecord::getInfo()
{
	return "LifeTime: " + this->lifeTime + " Prefix Address: " + this->prefixAddress + " Prefix Mask: " + this->prefixMask + " Next Hop: " + this->nextHop;
}

int RoutingRecord::toSec(std::string& paLifeTime)
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
