#include "EquipmentRecord.h"
#include <iomanip>

EquipmentRecord::EquipmentRecord()
    : id_(0), model_("—"), regNumber_("—"), siteName_("—"),
      hourlyRate_(0.0), hoursWorked_(0), category_("—"), priorityLevel_(1) {
}

EquipmentRecord::EquipmentRecord(int id, const std::string& model, const std::string& regNumber,
                                 const std::string& siteName, double hourlyRate, int hoursWorked,
                                 const std::string& category, int priorityLevel)
    : id_(id), model_(model), regNumber_(regNumber), siteName_(siteName),
      hourlyRate_(hourlyRate), hoursWorked_(hoursWorked), category_(category),
      priorityLevel_(priorityLevel) {
}

std::string EquipmentRecord::priorityString() const {
    switch (priorityLevel_) {
    case 3:  return "3-Аварийный";
    case 2:  return "2-Срочный";
    case 1:  return "1-Плановый";
    default: return std::to_string(priorityLevel_);
    }
}

bool EquipmentRecord::operator<(const EquipmentRecord& other) const {
    if (priorityLevel_ != other.priorityLevel_) {
        return priorityLevel_ < other.priorityLevel_;
    }
    if (hourlyRate_ != other.hourlyRate_) {
        return hourlyRate_ < other.hourlyRate_;
    }
    return id_ < other.id_;
}

bool EquipmentRecord::operator>(const EquipmentRecord& other) const {
    return other < *this;
}

void EquipmentRecord::printHeader(std::ostream& os) {
    std::ios::fmtflags old = os.flags();
    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(5)  << "ID"
       << " | " << std::setw(20) << "Модель"
       << " | " << std::setw(14) << "Гос. номер"
       << " | " << std::setw(22) << "Стройобъект"
       << " | " << std::setw(12) << "Тариф, руб/ч"
       << " | " << std::setw(8)  << "Отраб. ч"
       << " | " << std::setw(12) << "Сумма, руб."
       << " | " << std::setw(20) << "Категория работ"
       << " | " << std::setw(12) << "Приоритет"
       << "\n";
    os << std::string(140, '-') << "\n";
    os.flags(old);
}

void EquipmentRecord::printRow(std::ostream& os) const {
    std::ios::fmtflags old = os.flags();
    std::streamsize oldPrec = os.precision();

    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(5)  << id_
       << " | " << std::setw(20) << model_
       << " | " << std::setw(14) << regNumber_
       << " | " << std::setw(22) << siteName_;

    os.setf(std::ios::fixed, std::ios::floatfield);
    os << " | " << std::setw(12) << std::setprecision(2) << hourlyRate_;

    os.setf(std::ios::left, std::ios::adjustfield);
    os << " | " << std::setw(8) << hoursWorked_;

    os.setf(std::ios::fixed, std::ios::floatfield);
    os << " | " << std::setw(12) << std::setprecision(2) << totalCost();

    os.setf(std::ios::left, std::ios::adjustfield);
    os << " | " << std::setw(20) << category_
       << " | " << std::setw(12) << priorityString()
       << "\n";

    os.flags(old);
    os.precision(oldPrec);
}
