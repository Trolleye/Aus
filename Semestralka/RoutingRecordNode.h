#pragma once

#include <string>
#include <libds/amt/implicit_sequence.h>
#include "RoutingRecord.h"

class RoutingRecordNode
{
private:
    int ip;
    ds::amt::ImplicitSequence<RoutingRecord*> records;

public:
    RoutingRecordNode();
    RoutingRecordNode(int ip);
    ~RoutingRecordNode();
    void addRecord(RoutingRecord* record);
    int getIp();
    ds::amt::ImplicitSequence<RoutingRecord*> getRecords();
    bool operator==(const RoutingRecordNode& other) const;
};

RoutingRecordNode::RoutingRecordNode()
{
    this->ip = 0;
}

inline RoutingRecordNode::RoutingRecordNode(int ip)
{
    this->ip = ip;
}

inline RoutingRecordNode::~RoutingRecordNode()
{
    this->records.clear();
}

inline void RoutingRecordNode::addRecord(RoutingRecord* record)
{
    this->records.insertLast().data_ = record;
}

inline int RoutingRecordNode::getIp()
{
    return this->ip;
}

inline ds::amt::ImplicitSequence<RoutingRecord*> RoutingRecordNode::getRecords()
{
    return this->records;
}

inline bool RoutingRecordNode::operator==(const RoutingRecordNode& other) const
{
    return this->ip == other.ip;
}