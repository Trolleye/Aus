#include "RoutingTableHierarchy.h"
#include "Filter.h"
#include "FilteringOptions.h"
#include "SortedTable.h"
#include "VectorQuickSort.h"

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
    std::cout << "2. MWHierarchy" << '\n';
    std::cout << "3. Sorted Table" << '\n';
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

bool compareIP(RoutingRecord& record1, RoutingRecord& record2) {
    if (record1.getIP() != record2.getIP()) {
        return record1.getIP() < record2.getIP();
    }
    return record1.getMask() < record2.getMask();
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
    std::vector<RoutingRecord> records = Parser::parseCSV("RT.csv");
    std::string userNextHop;
    std::string userAddress;
    int userMask;
    std::string userFromTime;
    std::string userToTime;
    FilteringOptions options = FilteringOptions();
    std::vector<RoutingRecord*> filtered = std::vector<RoutingRecord*>();
    RoutingTableHierarchy hierarchy = RoutingTableHierarchy();
    SortedTable sortedTable = SortedTable(records);
    hierarchy.addFromVector(records);
    std::cout << hierarchy.getSize() << std::endl << sortedTable.getSize() << std::endl;
    auto hierarchyCurrentNode = RoutingTableHierarchyIterator(hierarchy.getHierarchy()->accessRoot(), hierarchy.getHierarchy());
    auto hierarchyEnd = RoutingTableHierarchyIterator(nullptr, hierarchy.getHierarchy());

    auto matchNextHopPred = [&](RoutingRecord* record) { return record->matchNextHop(userNextHop); };
    auto matchWithAddressPred = [&](RoutingRecord* record) { return record->matchWithAddress(userAddress, userMask); };
    auto matchLifetimePred = [&](RoutingRecord* record) { return record->matchLifeTime(userFromTime, userToTime); };
    auto matchNextHopPredRef = [&](RoutingRecord& record) { return record.matchNextHop(userNextHop); };
    auto matchWithAddressPredRef = [&](RoutingRecord& record) { return record.matchWithAddress(userAddress, userMask); };
    auto matchLifetimePredRef = [&](RoutingRecord& record) { return record.matchLifeTime(userFromTime, userToTime); };
    auto isLeaf = [&](RoutingRecordNode& record) { return !record.getRecords().isEmpty(); };

    auto pushToFiltered = [&](RoutingRecord* record) { filtered.push_back(record); };
    auto pushToFilteredRef = [&](RoutingRecord& record) { filtered.push_back(&record); };

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

                    if (!filtered.empty()) {
                        std::cout << "Do you want to sort these results? (y/n): ";
                        std::cin >> confirmation;

                        if (confirmation == "Y" || confirmation == "y") {
                            int sortChoice;
                            std::cout << "Choose sort option:\n"
                                << "1. Sort by lifetime\n"
                                << "2. Sort by IP prefix\n"
                                << "Enter your choice: ";
                            std::cin >> sortChoice;

                            if (sortChoice == 1) {
                                auto compareTime = [](RoutingRecord* record1, RoutingRecord* record2) {
                                    return record1->getLifeTime() < record2->getLifeTime();
                                    };
                                VectorQuickSort::sort(filtered, compareTime);
                            }
                            else if (sortChoice == 2) {
                                auto comparePrefix = [](RoutingRecord* record1, RoutingRecord* record2) {
                                    return compareIP(*record1, *record2);
                                    };
                                VectorQuickSort::sort(filtered, comparePrefix);
                            }
                            else {
                                std::cout << "Invalid choice. No sorting applied.\n";
                            }

                            std::cout << "Sorted results:\n";
                            printFiltered(filtered);
                        }
                    }

                    filtered.clear();
                }
            }
            break;
        }

        case 3: {
            bool inSubmenu = true;
            while (inSubmenu) {
                showMenu();
                std::cin >> choice;

                if (std::cin.fail()) {
                    handleInvalidInput();
                    continue;
                }

                switch (choice) {
                case 1:
                case 3:
                    std::cout << "This option is not available for SortedTable.\n";
                    break;

                case 2: {
                    std::cout << "Enter nextHop address to search: ";
                    std::cin >> userNextHop;
                    sortedTable.findRecords(userNextHop);
                    break;
                }

                case 0:
                    if (confirmExit()) {
                        inSubmenu = false;
                    }
                    break;

                default:
                    std::cout << "Invalid choice. Try again.\n";
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