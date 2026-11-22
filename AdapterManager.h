#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include "EquipmentRecord.h"
#include "Comparators.h"

class AdapterManager {
private:

    std::queue<EquipmentRecord> queue_;

    std::stack<EquipmentRecord> stack_;

    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentPriorityCompare> pqueueDefault_;
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentRateCompare> pqueueRate_;
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentHoursCompare> pqueueHours_;
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentIdCompare> pqueueId_;

    int currentPQueueMode_;
    int nextId_;

public:
    AdapterManager();

    void seedDemoData();
    int generateNextId() { return nextId_++; }

    void queuePush(const EquipmentRecord& rec);
    bool queuePop(EquipmentRecord& outRec);
    bool queueFront(EquipmentRecord& outRec) const;
    bool queueBack(EquipmentRecord& outRec) const;
    size_t queueSize() const { return queue_.size(); }
    bool queueEmpty() const { return queue_.empty(); }
    void queuePrint(std::ostream& os) const;
    bool queueExportToFile(const std::string& filename) const;
    std::vector<EquipmentRecord> queueSearch(const std::string& query) const;
    std::vector<EquipmentRecord> queueFilterByCategory(const std::string& category) const;
    bool queueRemoveById(int id);
    bool queueEditById(int id, double newRate, int newHours, const std::string& newSite, int newPriority);
    void queueSort(int criterion);
    void queueClear();

    void stackPush(const EquipmentRecord& rec);
    bool stackPop(EquipmentRecord& outRec);
    bool stackTop(EquipmentRecord& outRec) const;
    size_t stackSize() const { return stack_.size(); }
    bool stackEmpty() const { return stack_.empty(); }
    void stackPrint(std::ostream& os) const;
    bool stackExportToFile(const std::string& filename) const;
    std::vector<EquipmentRecord> stackSearch(const std::string& query) const;
    std::vector<EquipmentRecord> stackFilterByCategory(const std::string& category) const;
    bool stackRemoveById(int id);
    bool stackEditById(int id, double newRate, int newHours, const std::string& newSite, int newPriority);
    void stackSort(int criterion);
    void stackClear();

    void pqueuePush(const EquipmentRecord& rec);
    bool pqueuePop(EquipmentRecord& outRec);
    bool pqueueTop(EquipmentRecord& outRec) const;
    size_t pqueueSize() const;
    bool pqueueEmpty() const;
    void pqueuePrint(std::ostream& os) const;
    bool pqueueExportToFile(const std::string& filename) const;
    std::vector<EquipmentRecord> pqueueSearch(const std::string& query) const;
    std::vector<EquipmentRecord> pqueueFilterByCategory(const std::string& category) const;
    bool pqueueRemoveById(int id);
    bool pqueueEditById(int id, double newRate, int newHours, const std::string& newSite, int newPriority);
    void pqueueSetCriterion(int criterion);
    int pqueueCurrentCriterion() const { return currentPQueueMode_; }
    std::string pqueueCurrentCriterionName() const;
    void pqueueClear();

    void printAllSummary(std::ostream& os) const;
    bool exportAllToFile(const std::string& filename) const;
};
