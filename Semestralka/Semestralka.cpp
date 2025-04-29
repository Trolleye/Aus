#include "RoutingTableHierarchy.h"
#include "Filter.h"
#include "FilteringOptions.h"
static void showMenu() {
    std::cout << "\n";	
    std::cout << "1. Filter by lifetime" << '\n';
	std::cout << "2. Filter by nexthop address" << '\n';
	std::cout << "3. Filter by matching with prefix address" << '\n';
	std::cout << "0. Exit" << '\n';
	std::cout << "Enter your choice: ";
}

static void showParts() {
    std::cout << "\n";	
    std::cout << "1. Vector" << '\n';
	std::cout << "2. MWHierarchy"<< '\n';
	std::cout << "0. Exit" << '\n';
	std::cout << "Enter your choice: ";
}

static void showMovementOptions() {
    std::cout << "\n";
	std::cout << "1. To parent" << "\n";
	std::cout << "2. To son" << "\n";
	std::cout << "3. Filter" << "\n";
	std::cout << "Enter your choice: ";
}

static void printFiltered(std::vector<RoutingRecord*>& filteredVector) {
	for (RoutingRecord* record : filteredVector) {
		std::cout << record->getInfo() << std::endl;
	}
}

void handleInvalidInput() {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cout << "Invalid input. Please enter a number." << '\n';
}

bool confirmExit() {
    std::string confirmation;
    std::cout << "Are you sure you want to exit? (y/n): ";
    std::cin >> confirmation;
    return (confirmation == "y" || confirmation == "Y");
}

int main() {
	initHeapMonitor();
    /*auto node = RoutingRecordNode();
    auto records = Parser::parseCSV("RT.csv");
    for (decltype(auto) record : records) {
        node.addRecord(&record);
    }
    std::cout << node.getRecords().accessFirst()->data_->getInfo();*/
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
	auto isLeaf = [&](RoutingRecordNode& record) { return !record.getRecords().isEmpty();};
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
    void handleInvalidInput();
    bool confirmExit();

    while (true) {
        showParts();
        std::cin >> part;

        if (std::cin.fail()) {
            handleInvalidInput();
            continue;
        }

        switch (part) {
        case 1:
        case 2: {
            bool inSubmenu = true;
            filtered.clear();

            if (part == 2) {
                bool navigating = true;
                while (navigating) {
                    showMovementOptions();
                    std::cin >> choice;

                    if (std::cin.fail()) {
                        handleInvalidInput();
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
            }

            while (inSubmenu) {
                showMenu();
                std::cin >> choice;

                if (std::cin.fail()) {
                    handleInvalidInput();
                    continue;
                }

                switch (choice) {
                case 1:
                    do {
                        std::cout << "Enter minimum life time: ";
                        std::cin >> userFromTime;
                        if (!options.isValidTime(userFromTime)) {
                            std::cout << "Invalid time format. Please try again.\n";
                        }
                    } while (!options.isValidTime(userFromTime));

                    do {
                        std::cout << "Enter maximum life time: ";
                        std::cin >> userToTime;
                        if (!options.isValidTime(userToTime)) {
                            std::cout << "Invalid time format. Please try again.\n";
                        }
                    } while (!options.isValidTime(userToTime));

                    if (part == 1) {
                        Filter::filter(records.begin(), records.end(), matchLifetimePredRef, pushToFilteredRef);
                    }
                    else {
                        Filter::filter(hierarchyCurrentNode, hierarchyEnd, isLeaf, processLifetimePred);
                    }
                    break;

                case 2:
                    do {
                        std::cout << "Enter a nexthop address: ";
                        std::cin >> userNextHop;
                        if (!options.isValidIP(userNextHop)) {
                            std::cout << "Invalid IP address. Please try again.\n";
                        }
                    } while (!options.isValidIP(userNextHop));

                    if (part == 1) {
                        Filter::filter(records.begin(), records.end(), matchNextHopPredRef, pushToFilteredRef);
                    }
                    else {
                        Filter::filter(hierarchyCurrentNode, hierarchyEnd, isLeaf, processNextHopPred);
                    }
                    break;

                case 3:
                    do {
                        std::cout << "Enter address you want to match: ";
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

                    if (part == 1) {
                        Filter::filter(records.begin(), records.end(), matchWithAddressPredRef, pushToFilteredRef);
                    }
                    else {
                        Filter::filter(hierarchyCurrentNode, hierarchyEnd, isLeaf, processAddressPred);
                    }
                    break;

                case 4:
                    std::cout << "Current filtered results (" << filtered.size() << " records):" << '\n';
                    printFiltered(filtered);
                    break;

                case 0:
                    if (confirmExit()) {
                        inSubmenu = false;
                    }
                    break;

                default:
                    std::cout << "Invalid choice. Try again.\n";
                }

                if (choice >= 1 && choice <= 3) {
                    printFiltered(filtered);
                    std::cout << "Filter applied. Current results: " << filtered.size() << " records." << '\n';
                    filtered.clear();
                }
            }
            break;
        }
        case 0:
            if (confirmExit()) {
                return 0;
            }
            break;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}