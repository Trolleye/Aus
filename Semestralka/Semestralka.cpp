#include "RoutingTableHierarchy.h"
#include "Filter.h"
#include "FilteringOptions.h"
static void showMenu() {
	std::cout << "======Menu======" << '\n';
	std::cout << "1. Filter by lifetime" << '\n';
	std::cout << "2. Filter by nexthop address" << '\n';
	std::cout << "3. Filter by matching with prefix address" << '\n';
	std::cout << "4. Print filtered data structure" << '\n';
	std::cout << "5. Exit the part" << '\n';
	std::cout << "Enter your choice: ";
}

static void showParts() {
	std::cout << "======Parts======" << "\n";
	std::cout << "1. Part1 (simple vector)" << '\n';
	std::cout << "2. Part2 (explicit hierarchy)" << '\n';
	std::cout << "3. Exit the program" << '\n';
	std::cout << "Enter your choice: ";
}

static void showMovementOptions() {
	std::cout << "Chose where to move" << "\n";
	std::cout << "1. To parent" << "\n";
	std::cout << "2. To son" << "\n";
	std::cout << "3. Start filtering" << "\n";
	std::cout << "Enter your choice: ";
}

static void printFiltered(std::vector<RoutingRecord*>& filteredVector) {
	for (RoutingRecord* record : filteredVector) {
		std::cout << record->getInfo() << std::endl;
	}
}

int main() {
	initHeapMonitor();
    std::vector<RoutingRecord> records = Parser::parseCSV("RT.csv");
	std::string userNextHop;
	std::string userAddress;
	int userMask;
	std::string userFromTime;
	std::string userToTime;
	FilteringOptions options = FilteringOptions();
	std::vector<RoutingRecord*> filtered = std::vector<RoutingRecord*>();
	RoutingTableHierarchy hierarchy = RoutingTableHierarchy();
	hierarchy.addFromVector(records);
	auto hierarchyCurrentNode = RoutingTableHierarchyIterator(hierarchy.getHierarchy()->accessRoot(), hierarchy.getHierarchy());
	auto hierarchyEnd = RoutingTableHierarchyIterator(nullptr, hierarchy.getHierarchy());
	auto matchNextHopPred = [&](RoutingRecord* record) { return record->matchNextHop(userNextHop); };
	auto matchWithAddressPred = [&](RoutingRecord* record) {return record->matchWithAddress(userAddress, userMask); };
	auto matchLifetimePred = [&](RoutingRecord* record) {return record->matchLifeTime(userFromTime, userToTime); };
    auto matchNextHopPredRef = [&](RoutingRecord& record) { return record.matchNextHop(userNextHop); };
    auto matchWithAddressPredRef = [&](RoutingRecord& record) {return record.matchWithAddress(userAddress, userMask); };
    auto matchLifetimePredRef = [&](RoutingRecord& record) {return record.matchLifeTime(userFromTime, userToTime); };
	auto isLeaf = [&](RoutingRecordNode& record) { return !record.getRecords().empty();};
	auto pushToFiltered = [&](RoutingRecord* record) {
        filtered.push_back(record);
	};

    auto pushToFilteredRef = [&](RoutingRecord& record) {
        filtered.push_back(&record);
    };

	auto processLifetimePred = [&](RoutingRecordNode& node) {
		Filter::filter(node.getRecords().begin(), node.getRecords().end(), matchLifetimePred, pushToFiltered);
		};
	auto processNextHopPred = [&](RoutingRecordNode& node) {
		Filter::filter(node.getRecords().begin(), node.getRecords().end(), matchNextHopPred, pushToFiltered);
		};
	auto processAddressPred = [&](RoutingRecordNode& node) {
		Filter::filter(node.getRecords().begin(), node.getRecords().end(), matchWithAddressPred, pushToFiltered);
		};

    int choice, octetValue, part;
    std::string confirmation;

    while (true) {
        bool filtering = true;
        bool navigating = true;
        showParts();
        std::cin >> part;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number." << '\n';
            continue;
        }

        switch (part) {
        case 1:
            filtered.clear();
            while (filtering) {
                showMenu();
                std::cin >> choice;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Invalid input. Please enter a number." << '\n';
                    continue;
                }

                switch (choice) {
                case 1:
                    do {
                        std::cout << "Enter minimum life time (example >> 1w4d3h or 6:25:20): ";
                        std::cin >> userFromTime;
                        if (!options.isValidTime(userFromTime)) {
                            std::cout << "Invalid time format. Please try again.\n";
                        }
                    } while (!options.isValidTime(userFromTime));

                    do {
                        std::cout << "Enter maximum life time (example >> 1w4d3h or 6:25:20): ";
                        std::cin >> userToTime;
                        if (!options.isValidTime(userToTime)) {
                            std::cout << "Invalid time format. Please try again.\n";
                        }
                    } while (!options.isValidTime(userToTime));

                    Filter::filter(records.begin(), records.end(), matchLifetimePredRef, pushToFilteredRef);
                    printFiltered(filtered);
                    break;

                case 2:
                    do {
                        std::cout << "Enter a nexthop address (example >> 192.168.1.10): ";
                        std::cin >> userNextHop;
                        if (!options.isValidIP(userNextHop)) {
                            std::cout << "Invalid IP address. Please try again.\n";
                        }
                    } while (!options.isValidIP(userNextHop));

                    Filter::filter(records.begin(), records.end(), matchNextHopPredRef, pushToFilteredRef);
                    printFiltered(filtered);
                    break;

                case 3:
                    do {
                        std::cout << "Enter address you want to match (example >> 192.168.1.10): ";
                        std::cin >> userAddress;
                        if (!options.isValidIP(userAddress)) {
                            std::cout << "Invalid IP address. Please try again.\n";
                        }
                    } while (!options.isValidIP(userAddress));

                    do {
                        std::cout << "Enter max prefix you want to match (1-31): ";
                        std::cin >> userMask;
                        if (userMask < 1 || userMask > 31) {
                            std::cout << "Invalid mask. Please enter a value between 1-31.\n";
                        }
                    } while (userMask < 1 || userMask > 31);

                    Filter::filter(records.begin(), records.end(), matchWithAddressPredRef, pushToFilteredRef);
                    printFiltered(filtered);
                    break;

                case 4:
                    std::cout << "Current filtered results (" << filtered.size() << " records):" << '\n';
                    printFiltered(filtered);
                    break;

                case 5:
                    std::cout << "Are you sure you want to exit? (y/n): ";
                    std::cin >> confirmation;
                    if (confirmation == "y" || confirmation == "Y") {
                        filtering = false;
                        std::cout << "Exiting...\n";
                    }
                    break;

                default:
                    std::cout << "Invalid choice. Try again.\n";
                }

                if (choice >= 1 && choice <= 3) {
                    std::cout << "Filter applied. Current results: " << filtered.size() << " records." << '\n';
                    filtered.clear();
                }
            }
            break;
        case 2:
            filtered.clear();
            while (navigating) {
                showMovementOptions();
                std::cin >> choice;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Invalid input. Please enter a number." << '\n';
                    continue;
                }

                switch (choice) {
                case 1:
                    hierarchyCurrentNode.goToParent();
                    break;
                case 2:
                    std::cout << "Enter octet value: ";
                    std::cin >> octetValue;
                    hierarchyCurrentNode.goToSon(octetValue);
                    break;
                case 3:
                    navigating = false;
                    break;
                default:
                    std::cout << "Invalid choice. Try again.\n";
                }
            }
            while (filtering) {
                showMenu();
                std::cin >> choice;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Invalid input. Please enter a number." << '\n';
                    continue;
                }

                switch (choice) {
                case 1:
                    do {
                        std::cout << "Enter minimum life time (example >> 1w4d3h or 6:25:20): ";
                        std::cin >> userFromTime;
                        if (!options.isValidTime(userFromTime)) {
                            std::cout << "Invalid time format. Please try again.\n";
                        }
                    } while (!options.isValidTime(userFromTime));

                    do {
                        std::cout << "Enter maximum life time (example >> 1w4d3h or 6:25:20): ";
                        std::cin >> userToTime;
                        if (!options.isValidTime(userToTime)) {
                            std::cout << "Invalid time format. Please try again.\n";
                        }
                    } while (!options.isValidTime(userToTime));

                    Filter::filter(hierarchyCurrentNode, hierarchyEnd, isLeaf, processLifetimePred);
                    printFiltered(filtered);
                    break;

                case 2:
                    do {
                        std::cout << "Enter a nexthop address (example >> 192.168.1.10): ";
                        std::cin >> userNextHop;
                        if (!options.isValidIP(userNextHop)) {
                            std::cout << "Invalid IP address. Please try again.\n";
                        }
                    } while (!options.isValidIP(userNextHop));

                    Filter::filter(hierarchyCurrentNode, hierarchyEnd, isLeaf, processNextHopPred);
                    printFiltered(filtered);
                    break;

                case 3:
                    do {
                        std::cout << "Enter address you want to match (example >> 192.168.1.10): ";
                        std::cin >> userAddress;
                        if (!options.isValidIP(userAddress)) {
                            std::cout << "Invalid IP address. Please try again.\n";
                        }
                    } while (!options.isValidIP(userAddress));

                    do {
                        std::cout << "Enter max prefix you want to match (1-31): ";
                        std::cin >> userMask;
                        if (userMask < 1 || userMask > 31) {
                            std::cout << "Invalid mask. Please enter a value between 1-31.\n";
                        }
                    } while (userMask < 1 || userMask > 31);

                    Filter::filter(hierarchyCurrentNode, hierarchyEnd, isLeaf, processAddressPred);
                    printFiltered(filtered);
                    break;

                case 4:
                    std::cout << "Current filtered results (" << filtered.size() << " records):" << '\n';
                    printFiltered(filtered);
                    break;

                case 5:
                    std::cout << "Are you sure you want to exit? (y/n): ";
                    std::cin >> confirmation;
                    if (confirmation == "y" || confirmation == "Y") {
                        filtering = false;
                        std::cout << "Exiting...\n";
                    }
                    break;

                default:
                    std::cout << "Invalid choice. Try again.\n";
                }

                if (choice >= 1 && choice <= 3) {
                    std::cout << "Filter applied. Current results: " << filtered.size() << " records." << '\n';
                    filtered.clear();
                }
            }
            break;
        case 3:
            std::cout << "Are you sure you want to exit? (y/n): ";
            std::cin >> confirmation;
            if (confirmation == "y" || confirmation == "Y") {
                std::cout << "Exiting...\n";
                return 0;
            }
            break;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}