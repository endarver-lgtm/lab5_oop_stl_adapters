#pragma once
#include "EquipmentRecord.h"

struct EquipmentPriorityCompare {
    bool operator()(const EquipmentRecord& a, const EquipmentRecord& b) const {
        if (a.priorityLevel() != b.priorityLevel()) {
            return a.priorityLevel() < b.priorityLevel();
        }
        if (a.hourlyRate() != b.hourlyRate()) {
            return a.hourlyRate() < b.hourlyRate();
        }
        return a.id() < b.id();
    }
};

struct EquipmentRateCompare {
    bool operator()(const EquipmentRecord& a, const EquipmentRecord& b) const {
        if (a.hourlyRate() != b.hourlyRate()) {
            return a.hourlyRate() < b.hourlyRate();
        }
        return a.id() < b.id();
    }
};

struct EquipmentHoursCompare {
    bool operator()(const EquipmentRecord& a, const EquipmentRecord& b) const {
        if (a.hoursWorked() != b.hoursWorked()) {
            return a.hoursWorked() < b.hoursWorked();
        }
        return a.id() < b.id();
    }
};

struct EquipmentIdCompare {
    bool operator()(const EquipmentRecord& a, const EquipmentRecord& b) const {
        return a.id() < b.id();
    }
};
