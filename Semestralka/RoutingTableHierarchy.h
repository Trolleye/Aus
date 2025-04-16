#pragma once
#include "libds/amt/explicit_hierarchy.h"
#include <vector>
#include "Parser.h"

class RoutingRecordNode
{
private:
	int ip;
	std::vector<RoutingRecord*> records;
public:
	inline RoutingRecordNode() : ip(0) {}
	inline RoutingRecordNode(int ip) : ip(ip) {}

    inline void addRecord(RoutingRecord* record) { records.push_back(record); }
	inline int getIp() { return ip; }
	inline std::vector<RoutingRecord*> getRecords() { return records; }
	inline bool operator==(const RoutingRecordNode& other) const { return ip == other.ip; }
};

class RoutingTableHierarchy
{
private:
	ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode> hierarchy;
    ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* userCurrentNode = nullptr;
public:
	RoutingTableHierarchy();
	void addFromVector(std::vector<RoutingRecord>& vector);
	ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* addNode(int IP, ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>& currentNode, int octet, RoutingRecord* record);
	std::vector<int> getOctets(RoutingRecord& record);
    ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>* getHierarchy();
};

class RoutingTableHierarchyIterator {
private:
    ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* userCurrentNode;
    ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>* hierarchy;
    int currentUserOctet = 0;
public:
    RoutingTableHierarchyIterator(ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* userCurrentNode, ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>* hierarchy);
    auto currentPosition();
    auto end();
    auto goToSon(int IP);
    auto goToParent();
    void printRecords();
};

inline RoutingTableHierarchyIterator::RoutingTableHierarchyIterator(ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* userCurrentNode, ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>* hierarchy)
{
    this->userCurrentNode = userCurrentNode;
    this->hierarchy = hierarchy;
}

inline auto RoutingTableHierarchyIterator::currentPosition()
{
    return  ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>::PreOrderHierarchyIterator(this->hierarchy, this->userCurrentNode);
}

inline auto RoutingTableHierarchyIterator::end()
{
    return ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>::PreOrderHierarchyIterator(this->hierarchy, nullptr);
}

inline auto RoutingTableHierarchyIterator::goToSon(int IP)
{
    for (decltype(auto) son : *this->userCurrentNode->sons_)
    {
        if (!son) continue;
        if (son->data_.getIp() == IP) {
            userCurrentNode = son;
            ++this->currentUserOctet;
            break;
        }
    }
    if (this->currentUserOctet == 0)
    {
        std::cout << "Currently in root" << std::endl;
    }
    else
    {
        std::cout << "Currently in octet: " << this->currentUserOctet << " with value: " << userCurrentNode->data_.getIp() << std::endl;
    }
}

inline auto RoutingTableHierarchyIterator::goToParent()
{
    if (this->userCurrentNode != this->hierarchy->accessRoot()) {
       this->userCurrentNode = this->hierarchy->accessParent(*userCurrentNode);
       --this->currentUserOctet;
       if (this->currentUserOctet != 0)
       {
           std::cout << "Currently in octet: " << this->currentUserOctet << " with value: " << userCurrentNode->data_.getIp() << std::endl;
       }
       else
       {
           std::cout << "Currently in root" << std::endl;
       }

    }
    else
    {
        std::cout << "Already in root" << std::endl;
    }
}

inline void RoutingTableHierarchyIterator::printRecords()
{
    if (this->currentUserOctet == 4)
    {
        for (RoutingRecord* record : this->userCurrentNode->data_.getRecords()) {
            std::cout << record->getInfo() << std::endl;
        }
    }
}

inline RoutingTableHierarchy::RoutingTableHierarchy()
{
    if (!this->hierarchy.accessRoot()) {
        this->hierarchy.emplaceRoot();
        this->userCurrentNode = this->hierarchy.accessRoot();
    }
}


inline std::vector<int> RoutingTableHierarchy::getOctets(RoutingRecord& record)
{
	std::vector<int> octets;
	std::string ip = record.getIPAddress();
	std::stringstream ss(ip);
	std::string octet;

	while (std::getline(ss, octet, '.')) {
		octets.push_back(std::stoi(octet));
	}

	return octets;
}

inline ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode>* RoutingTableHierarchy::getHierarchy()
{
    return &this->hierarchy;
}

inline void RoutingTableHierarchy::addFromVector(std::vector<RoutingRecord>& vector)
{
    for (RoutingRecord& record : vector) {
        std::vector<int> octetsIP = this->getOctets(record);
        decltype(auto) currentNode = hierarchy.accessRoot();

        if (!currentNode) {
            std::cerr << "Error: Root node is null!" << std::endl;
            continue;
        }

        for (int i = 0; i < 4; i++) {
            int IP = octetsIP[i];
            bool found = false;

            if (!currentNode->sons_) {
                currentNode = this->addNode(IP, *currentNode, i, (i == 3) ? &record : nullptr);
                continue;
            }

            for (decltype(auto) son : *currentNode->sons_) {
                if (!son) continue;
                if (son->data_.getIp() == IP) {
                    currentNode = son;
                    found = true;
                    if (i == 3)
                    {
                        son->data_.addRecord(&record);
                    }
                    break;
                }
            }

            if (!found) {
                currentNode = this->addNode(IP, *currentNode, i, (i == 3) ? &record : nullptr);
            }
        }
    }
    std::cout << "Hierarchy size: " << this->hierarchy.size() << std::endl;
}




inline ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* RoutingTableHierarchy::addNode(int IP, ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>& currentNode, int octet, RoutingRecord* record)
{
    decltype(auto) sonNode = this->hierarchy.emplaceSon(currentNode, currentNode.sons_->size());
    sonNode.data_ = RoutingRecordNode(IP);
    if (record != nullptr) {
        sonNode.data_.addRecord(record);
    }
    return &sonNode;
}