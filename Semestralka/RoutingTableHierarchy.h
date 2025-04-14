#pragma once
#include "libds/amt/explicit_hierarchy.h"
#include "RoutingRecordNode.h"
#include <vector>
class RoutingTableHierarchy
{
private:
	ds::amt::MultiWayExplicitHierarchy<RoutingRecordNode> hierarchy;
public:
	RoutingTableHierarchy();
	~RoutingTableHierarchy();
	void addFromVector(std::vector<RoutingRecord>& vector);
	void addNode(int IP, ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>& currentNode);
	std::vector<int> getOctets(RoutingRecord& record);
};

RoutingTableHierarchy::RoutingTableHierarchy()
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

inline void RoutingTableHierarchy::addFromVector(std::vector<RoutingRecord>& vector)
{
	decltype(auto) currentNode = hierarchy.accessRoot();

	for (RoutingRecord record : vector) {
		std::vector<int> octetsIP = this->getOctets(record);
		for (size_t i = 0; i < 4; i++)
		{
			int IP = octetsIP[i];

			for (decltype(auto) son : *currentNode->sons_)
			{
				if (son->data_.getIp() == IP)
				{
					currentNode = son;
				}
				else
				{
					this->addNode(IP, *currentNode);

				}
			}
		}
	}
}


inline void RoutingTableHierarchy::addNode(int IP, ds::amt::MultiWayExplicitHierarchyBlock<RoutingRecordNode>& currentNode)
{
	decltype(auto) sonNode = this->hierarchy.emplaceSon(currentNode, currentNode.sons_->size());
	sonNode.data_ = RoutingRecordNode(IP);
	currentNode = sonNode;
}