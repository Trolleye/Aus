#include "RoutingTableHierarchy.h"
int main() {
	RoutingTableHierarchy hierarchy = RoutingTableHierarchy();
	hierarchy.addFromVector();
	auto iterator = RoutingTableHierarchyIterator(hierarchy.getHierarchy()->accessRoot(), hierarchy.getHierarchy());
	
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