#pragma once
#include "libds/adt/table.h"
#include "libds/amt/implicit_sequence.h"
#include "RoutingRecord.h"
class SortedTable
{
public:
	inline SortedTable(std::vector<RoutingRecord>& vectorRecords);
	inline ~SortedTable();
	inline void addRecords(std::vector<RoutingRecord>& vectorRecords);
	inline void findRecords(std::string& key);
	inline std::string getSize() { return "Sorted Table size: " + std::to_string(this->records.size()); };
private:
	ds::adt::SortedSequenceTable<std::string, ds::amt::ImplicitSequence<RoutingRecord*>*> records;
};

inline SortedTable::SortedTable(std::vector<RoutingRecord>& vectorRecords)
{
	addRecords(vectorRecords);
}

inline SortedTable::~SortedTable()
{
	for (auto& record : this->records)
	{
		delete record.data_;
	}
	this->records.clear();
}

inline void SortedTable::addRecords(std::vector<RoutingRecord>& vectorRecords)
{
	ds::amt::ImplicitSequence<RoutingRecord*>** recordData = nullptr;
	for (RoutingRecord& record : vectorRecords)
	{
		if (this->records.tryFind(record.getNextHop(), recordData))
		{
			(*recordData)->insertLast().data_ = &record;
		}
		else
		{
			ds::amt::ImplicitSequence<RoutingRecord*>* newRecord = new ds::amt::ImplicitSequence<RoutingRecord*>();
			newRecord->insertLast().data_ = &record;
			this->records.insert(record.getNextHop(), newRecord);
		}
	}
}

inline void SortedTable::findRecords(std::string& key)
{
	ds::amt::ImplicitSequence<RoutingRecord*>** recordData = nullptr;
	if (this->records.tryFind(key, recordData))
	{
		int size = 0;
		for (auto& record : **recordData)
		{
			std::cout << record->getInfo() << std::endl;
			size++;
		}
		std::cout << "Table size: " << size << std::endl;

	}
	else
	{
		std::cout << "No records found for the given key." << std::endl;
	}
}
