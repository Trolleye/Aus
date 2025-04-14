#pragma once
#include "libds/amt/explicit_hierarchy.h"
#include <vector>
#include "Parser.h"

class RoutingRecordNode
{
private:
	int ip;
	ds::amt::ImplicitSequence<RoutingRecord*> records;

public:
	inline RoutingRecordNode() : ip(0) {}
	inline RoutingRecordNode(int ip) : ip(ip) {}
	inline ~RoutingRecordNode() { records.clear(); }

	inline void addRecord(RoutingRecord* record) { records.insertLast().data_ = record; }
	inline int getIp() { return ip; }
	inline ds::amt::ImplicitSequence<RoutingRecord*> getRecords() { return records; }
	inline bool operator==(const RoutingRecordNode& other) const { return ip == other.ip; }
};

class RoutingTableHierarchy
{
private:
	ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode> hierarchy;
public:
	RoutingTableHierarchy();
	~RoutingTableHierarchy();
	void addFromVector();
	ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* addNode(int IP, ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>& currentNode, int octet, RoutingRecord* record);
	std::vector<int> getOctets(RoutingRecord& record);
};

inline RoutingTableHierarchy::RoutingTableHierarchy()
{
	hierarchy.emplaceRoot();
}

inline RoutingTableHierarchy::~RoutingTableHierarchy()
{
	hierarchy.clear();
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

inline void RoutingTableHierarchy::addFromVector()
{
    std::vector<RoutingRecord> vector = Parser::parseCSV("RT.csv");

    for (RoutingRecord& record : vector) {
        std::vector<int> octetsIP = this->getOctets(record);
        decltype(auto) currentNode = hierarchy.accessRoot();
        for (size_t i = 0; i < 4; i++)
        {
            int IP = octetsIP[i];
            bool found = false;

            if (currentNode->sons_->size() == 0)
            {
                currentNode = this->addNode(IP, *currentNode, i, &record);
                
            }
            else
            {
                for (decltype(auto) son : *currentNode->sons_)
                {
                    if (son->data_.getIp() == IP)
                    {
                        currentNode = son;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    currentNode = this->addNode(IP, *currentNode, i, &record);
                }
            }
        }
    }
    std::cout << this->hierarchy.size();
}


inline ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>* RoutingTableHierarchy::addNode(int IP, ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>& currentNode, int octet, RoutingRecord* record)
{
	decltype(auto) sonNode = this->hierarchy.emplaceSon(currentNode, currentNode.sons_->size());
    sonNode.data_ = RoutingRecordNode(IP);

    if (octet == 3)
    {
        sonNode.data_.addRecord(record);
    }
	return &sonNode;
}