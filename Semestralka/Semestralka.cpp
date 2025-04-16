#include "RoutingTableHierarchy.h"
int main() {
	initHeapMonitor();
	std::string userNextHop;
	std::string userAddress;
	int userMask;
	std::string userFromTime;
	std::string userToTime;

	RoutingTableHierarchy hierarchy = RoutingTableHierarchy();
	hierarchy.addFromVector();
	auto iterator = RoutingTableHierarchyIterator(hierarchy.getHierarchy()->accessRoot(), hierarchy.getHierarchy());
	auto matchNextHopPred = [&](RoutingRecord& record) { return record.matchNextHop(userNextHop); };
	auto matchWithAddressPred = [&](RoutingRecord& record) {return record.matchWithAddress(userAddress, userMask); };
	auto matchLifetimePred = [&](RoutingRecord& record) {return record.matchLifeTime(userFromTime, userToTime); };
	auto isLeaf = [&](RoutingRecordNode& record) { return record.getRecords().size() > 0;};



	while (true) {
		std::string input;
		std::cin >> input;
		if (input == "parent")
		{
			iterator.goToParent();
		}
		else if (input == "records")
		{
			iterator.printRecords();
		}
		else
		{
			int itinput = std::stoi(input);
			iterator.goToSon(itinput);
		}
	}
}