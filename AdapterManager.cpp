#include "AdapterManager.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

static std::string toLowerStr(const std::string& str) {
    std::string res = str;
    for (char& c : res) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return res;
}

static bool recordMatches(const EquipmentRecord& rec, const std::string& query) {
    if (query.empty()) return true;
    std::string q = toLowerStr(query);
    if (toLowerStr(rec.model()).find(q) != std::string::npos) return true;
    if (toLowerStr(rec.regNumber()).find(q) != std::string::npos) return true;
    if (toLowerStr(rec.siteName()).find(q) != std::string::npos) return true;
    if (toLowerStr(rec.category()).find(q) != std::string::npos) return true;
    if (toLowerStr(std::to_string(rec.id())).find(q) != std::string::npos) return true;
    return false;
}

AdapterManager::AdapterManager()
    : currentPQueueMode_(1), nextId_(1) {
}

void AdapterManager::seedDemoData() {
    queueClear();
    stackClear();
    pqueueClear();

    std::vector<EquipmentRecord> demoList = {
        EquipmentRecord(1, "CAT 320D", "1928 BK-7", "ЖК Минск-Мир", 120.0, 180, "Земляные работы", 1),
        EquipmentRecord(2, "Komatsu PC210", "4412 MP-7", "Северный Берег", 145.0, 95, "Земляные работы", 3),
        EquipmentRecord(3, "Liebherr LTM 1050", "8821 AB-7", "Мост через Немигу", 250.0, 60, "Монтажные работы", 2),
        EquipmentRecord(4, "JCB 3CX Eco", "3309 TT-7", "ТЦ Мега Молл", 85.0, 220, "Погрузочные работы", 1),
        EquipmentRecord(5, "Volvo EC220E", "5517 KX-7", "Минская кольцевая", 130.0, 140, "Земляные работы", 2),
        EquipmentRecord(6, "CAT 966M", "7714 IX-7", "Стройбаза Колодищи", 110.0, 310, "Погрузочные работы", 3),
        EquipmentRecord(7, "Hitachi ZX330", "9901 OP-7", "ЖК Минск-Мир", 160.0, 75, "Земляные работы", 1),
        EquipmentRecord(8, "KamAZ 6520", "2245 EM-7", "Карьер Заславль", 65.0, 410, "Транспортные работы", 2)
    };

    nextId_ = 9;

    for (const auto& rec : demoList) {
        queuePush(rec);
        stackPush(rec);
        pqueuePush(rec);
    }
}

void AdapterManager::queuePush(const EquipmentRecord& rec) {
    queue_.push(rec);
}

bool AdapterManager::queuePop(EquipmentRecord& outRec) {
    if (queue_.empty()) return false;
    outRec = queue_.front();
    queue_.pop();
    return true;
}

bool AdapterManager::queueFront(EquipmentRecord& outRec) const {
    if (queue_.empty()) return false;
    outRec = queue_.front();
    return true;
}

bool AdapterManager::queueBack(EquipmentRecord& outRec) const {
    if (queue_.empty()) return false;
    outRec = queue_.back();
    return true;
}

void AdapterManager::queuePrint(std::ostream& os) const {
    if (queue_.empty()) {
        os << "Очередь (std::queue) пуста.\n";
        return;
    }
    os << "\n=== СОДЕРЖИМОЕ ОЧЕРЕДИ (std::queue, FIFO) ===\n";
    os << "Порядок элементов: от первого поступившего (голова) к последнему (хвост).\n";
    EquipmentRecord::printHeader(os);

    std::queue<EquipmentRecord> temp = queue_;
    while (!temp.empty()) {
        temp.front().printRow(os);
        temp.pop();
    }
    os << "Всего заявок в очереди: " << queue_.size() << "\n";
}

bool AdapterManager::queueExportToFile(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) return false;
    ofs << "ОТЧЕТ: СОДЕРЖИМОЕ ОЧЕРЕДИ ЗАЯВОК НА ТЕХНИКУ (std::queue)\n";
    queuePrint(ofs);
    return true;
}

std::vector<EquipmentRecord> AdapterManager::queueSearch(const std::string& query) const {
    std::vector<EquipmentRecord> result;
    std::queue<EquipmentRecord> temp = queue_;
    while (!temp.empty()) {
        if (recordMatches(temp.front(), query)) {
            result.push_back(temp.front());
        }
        temp.pop();
    }
    return result;
}

std::vector<EquipmentRecord> AdapterManager::queueFilterByCategory(const std::string& category) const {
    std::vector<EquipmentRecord> result;
    std::queue<EquipmentRecord> temp = queue_;
    while (!temp.empty()) {
        if (temp.front().category() == category) {
            result.push_back(temp.front());
        }
        temp.pop();
    }
    return result;
}

bool AdapterManager::queueRemoveById(int id) {
    if (queue_.empty()) return false;
    std::queue<EquipmentRecord> temp;
    bool found = false;

    while (!queue_.empty()) {
        EquipmentRecord rec = queue_.front();
        queue_.pop();
        if (rec.id() == id && !found) {
            found = true;
        } else {
            temp.push(rec);
        }
    }
    queue_ = std::move(temp);
    return found;
}

bool AdapterManager::queueEditById(int id, double newRate, int newHours, const std::string& newSite, int newPriority) {
    if (queue_.empty()) return false;
    std::queue<EquipmentRecord> temp;
    bool found = false;

    while (!queue_.empty()) {
        EquipmentRecord rec = queue_.front();
        queue_.pop();
        if (rec.id() == id) {
            found = true;
            rec.setHourlyRate(newRate);
            rec.setHoursWorked(newHours);
            rec.setSiteName(newSite);
            rec.setPriorityLevel(newPriority);
        }
        temp.push(rec);
    }
    queue_ = std::move(temp);
    return found;
}

static void sortRecordVector(std::vector<EquipmentRecord>& vec, int criterion) {
    switch (criterion) {
    case 1:
        std::sort(vec.begin(), vec.end(), [](const EquipmentRecord& a, const EquipmentRecord& b) {
            return a.id() < b.id();
        });
        break;
    case 2:
        std::sort(vec.begin(), vec.end(), [](const EquipmentRecord& a, const EquipmentRecord& b) {
            return a.hourlyRate() > b.hourlyRate();
        });
        break;
    case 3:
        std::sort(vec.begin(), vec.end(), [](const EquipmentRecord& a, const EquipmentRecord& b) {
            return a.hoursWorked() > b.hoursWorked();
        });
        break;
    case 4:
        std::sort(vec.begin(), vec.end(), [](const EquipmentRecord& a, const EquipmentRecord& b) {
            return a.totalCost() > b.totalCost();
        });
        break;
    case 5:
        std::sort(vec.begin(), vec.end(), [](const EquipmentRecord& a, const EquipmentRecord& b) {
            if (a.priorityLevel() != b.priorityLevel()) return a.priorityLevel() > b.priorityLevel();
            return a.hourlyRate() > b.hourlyRate();
        });
        break;
    case 6:
        std::sort(vec.begin(), vec.end(), [](const EquipmentRecord& a, const EquipmentRecord& b) {
            return a.model() < b.model();
        });
        break;
    default:
        break;
    }
}

void AdapterManager::queueSort(int criterion) {
    if (queue_.size() < 2) return;
    std::vector<EquipmentRecord> vec;
    while (!queue_.empty()) {
        vec.push_back(queue_.front());
        queue_.pop();
    }

    sortRecordVector(vec, criterion);

    for (const auto& item : vec) {
        queue_.push(item);
    }
}

void AdapterManager::queueClear() {
    std::queue<EquipmentRecord> emptyQ;
    std::swap(queue_, emptyQ);
}

void AdapterManager::stackPush(const EquipmentRecord& rec) {
    stack_.push(rec);
}

bool AdapterManager::stackPop(EquipmentRecord& outRec) {
    if (stack_.empty()) return false;
    outRec = stack_.top();
    stack_.pop();
    return true;
}

bool AdapterManager::stackTop(EquipmentRecord& outRec) const {
    if (stack_.empty()) return false;
    outRec = stack_.top();
    return true;
}

void AdapterManager::stackPrint(std::ostream& os) const {
    if (stack_.empty()) {
        os << "Стек (std::stack) пуст.\n";
        return;
    }
    os << "\n=== СОДЕРЖИМОЕ СТЕКА (std::stack, LIFO) ===\n";
    os << "Порядок элементов: сверху вниз (от вершины stack.top() ко дну).\n";
    EquipmentRecord::printHeader(os);

    std::stack<EquipmentRecord> temp = stack_;
    while (!temp.empty()) {
        temp.top().printRow(os);
        temp.pop();
    }
    os << "Всего записей в стеке: " << stack_.size() << "\n";
}

bool AdapterManager::stackExportToFile(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) return false;
    ofs << "ОТЧЕТ: ЖУРНАЛ ВОЗВРАТА ТЕХНИКИ СО СТРОЙОБЪЕКТОВ (std::stack)\n";
    stackPrint(ofs);
    return true;
}

std::vector<EquipmentRecord> AdapterManager::stackSearch(const std::string& query) const {
    std::vector<EquipmentRecord> result;
    std::stack<EquipmentRecord> temp = stack_;
    while (!temp.empty()) {
        if (recordMatches(temp.top(), query)) {
            result.push_back(temp.top());
        }
        temp.pop();
    }
    return result;
}

std::vector<EquipmentRecord> AdapterManager::stackFilterByCategory(const std::string& category) const {
    std::vector<EquipmentRecord> result;
    std::stack<EquipmentRecord> temp = stack_;
    while (!temp.empty()) {
        if (temp.top().category() == category) {
            result.push_back(temp.top());
        }
        temp.pop();
    }
    return result;
}

bool AdapterManager::stackRemoveById(int id) {
    if (stack_.empty()) return false;
    std::stack<EquipmentRecord> helperStack;
    bool found = false;

    while (!stack_.empty()) {
        EquipmentRecord rec = stack_.top();
        stack_.pop();
        if (rec.id() == id && !found) {
            found = true;
            break;
        }
        helperStack.push(rec);
    }

    while (!helperStack.empty()) {
        stack_.push(helperStack.top());
        helperStack.pop();
    }
    return found;
}

bool AdapterManager::stackEditById(int id, double newRate, int newHours, const std::string& newSite, int newPriority) {
    if (stack_.empty()) return false;
    std::stack<EquipmentRecord> helperStack;
    bool found = false;

    while (!stack_.empty()) {
        EquipmentRecord rec = stack_.top();
        stack_.pop();
        if (rec.id() == id) {
            found = true;
            rec.setHourlyRate(newRate);
            rec.setHoursWorked(newHours);
            rec.setSiteName(newSite);
            rec.setPriorityLevel(newPriority);
        }
        helperStack.push(rec);
    }

    while (!helperStack.empty()) {
        stack_.push(helperStack.top());
        helperStack.pop();
    }
    return found;
}

void AdapterManager::stackSort(int criterion) {
    if (stack_.size() < 2) return;
    std::vector<EquipmentRecord> vec;
    while (!stack_.empty()) {
        vec.push_back(stack_.top());
        stack_.pop();
    }

    sortRecordVector(vec, criterion);

    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        stack_.push(*it);
    }
}

void AdapterManager::stackClear() {
    std::stack<EquipmentRecord> emptyS;
    std::swap(stack_, emptyS);
}

static std::vector<EquipmentRecord> dumpPQueue(std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentPriorityCompare> pq) {
    std::vector<EquipmentRecord> res;
    while (!pq.empty()) {
        res.push_back(pq.top());
        pq.pop();
    }
    return res;
}

static std::vector<EquipmentRecord> dumpPQueue(std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentRateCompare> pq) {
    std::vector<EquipmentRecord> res;
    while (!pq.empty()) {
        res.push_back(pq.top());
        pq.pop();
    }
    return res;
}

static std::vector<EquipmentRecord> dumpPQueue(std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentHoursCompare> pq) {
    std::vector<EquipmentRecord> res;
    while (!pq.empty()) {
        res.push_back(pq.top());
        pq.pop();
    }
    return res;
}

static std::vector<EquipmentRecord> dumpPQueue(std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentIdCompare> pq) {
    std::vector<EquipmentRecord> res;
    while (!pq.empty()) {
        res.push_back(pq.top());
        pq.pop();
    }
    return res;
}

static std::vector<EquipmentRecord> getActivePQueueItems(int mode,
    const std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentPriorityCompare>& p1,
    const std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentRateCompare>& p2,
    const std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentHoursCompare>& p3,
    const std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentIdCompare>& p4) {

    switch (mode) {
    case 1: return dumpPQueue(p1);
    case 2: return dumpPQueue(p2);
    case 3: return dumpPQueue(p3);
    case 4: return dumpPQueue(p4);
    default: return dumpPQueue(p1);
    }
}

void AdapterManager::pqueuePush(const EquipmentRecord& rec) {
    pqueueDefault_.push(rec);
    pqueueRate_.push(rec);
    pqueueHours_.push(rec);
    pqueueId_.push(rec);
}

bool AdapterManager::pqueuePop(EquipmentRecord& outRec) {
    if (pqueueEmpty()) return false;
    pqueueTop(outRec);
    pqueueRemoveById(outRec.id());
    return true;
}

bool AdapterManager::pqueueTop(EquipmentRecord& outRec) const {
    if (pqueueEmpty()) return false;
    switch (currentPQueueMode_) {
    case 1: outRec = pqueueDefault_.top(); break;
    case 2: outRec = pqueueRate_.top(); break;
    case 3: outRec = pqueueHours_.top(); break;
    case 4: outRec = pqueueId_.top(); break;
    default: outRec = pqueueDefault_.top(); break;
    }
    return true;
}

size_t AdapterManager::pqueueSize() const {
    return pqueueDefault_.size();
}

bool AdapterManager::pqueueEmpty() const {
    return pqueueDefault_.empty();
}

void AdapterManager::pqueuePrint(std::ostream& os) const {
    if (pqueueEmpty()) {
        os << "Очередь с приоритетом (std::priority_queue) пуста.\n";
        return;
    }
    os << "\n=== СОДЕРЖИМОЕ ПРИОРИТЕТНОЙ ОЧЕРЕДИ (std::priority_queue) ===\n";
    os << "Текущий критерий упорядочения: " << pqueueCurrentCriterionName() << "\n";
    os << "Порядок элементов: от наивысшего приоритета (top()) к наименьшему.\n";
    EquipmentRecord::printHeader(os);

    auto items = getActivePQueueItems(currentPQueueMode_, pqueueDefault_, pqueueRate_, pqueueHours_, pqueueId_);
    for (const auto& item : items) {
        item.printRow(os);
    }
    os << "Всего записей в приоритетной очереди: " << items.size() << "\n";
}

bool AdapterManager::pqueueExportToFile(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) return false;
    ofs << "ОТЧЕТ: ДИСПЕТЧЕРСКАЯ ОЧЕРЕДЬ С ПРИОРИТЕТАМИ (std::priority_queue)\n";
    pqueuePrint(ofs);
    return true;
}

std::vector<EquipmentRecord> AdapterManager::pqueueSearch(const std::string& query) const {
    std::vector<EquipmentRecord> result;
    auto items = getActivePQueueItems(currentPQueueMode_, pqueueDefault_, pqueueRate_, pqueueHours_, pqueueId_);
    for (const auto& item : items) {
        if (recordMatches(item, query)) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<EquipmentRecord> AdapterManager::pqueueFilterByCategory(const std::string& category) const {
    std::vector<EquipmentRecord> result;
    auto items = getActivePQueueItems(currentPQueueMode_, pqueueDefault_, pqueueRate_, pqueueHours_, pqueueId_);
    for (const auto& item : items) {
        if (item.category() == category) {
            result.push_back(item);
        }
    }
    return result;
}

bool AdapterManager::pqueueRemoveById(int id) {
    if (pqueueEmpty()) return false;
    auto items = getActivePQueueItems(currentPQueueMode_, pqueueDefault_, pqueueRate_, pqueueHours_, pqueueId_);
    auto it = std::find_if(items.begin(), items.end(), [id](const EquipmentRecord& r) {
        return r.id() == id;
    });
    if (it == items.end()) return false;
    items.erase(it);

    pqueueClear();
    for (const auto& rec : items) {
        pqueuePush(rec);
    }
    return true;
}

bool AdapterManager::pqueueEditById(int id, double newRate, int newHours, const std::string& newSite, int newPriority) {
    if (pqueueEmpty()) return false;
    auto items = getActivePQueueItems(currentPQueueMode_, pqueueDefault_, pqueueRate_, pqueueHours_, pqueueId_);
    bool found = false;
    for (auto& item : items) {
        if (item.id() == id) {
            item.setHourlyRate(newRate);
            item.setHoursWorked(newHours);
            item.setSiteName(newSite);
            item.setPriorityLevel(newPriority);
            found = true;
            break;
        }
    }
    if (!found) return false;

    pqueueClear();
    for (const auto& rec : items) {
        pqueuePush(rec);
    }
    return true;
}

void AdapterManager::pqueueSetCriterion(int criterion) {
    if (criterion >= 1 && criterion <= 4) {
        currentPQueueMode_ = criterion;
    }
}

std::string AdapterManager::pqueueCurrentCriterionName() const {
    switch (currentPQueueMode_) {
    case 1: return "Уровень приоритета заявки (1-Плановый, 2-Срочный, 3-Аварийный) + тариф";
    case 2: return "Часовая тарифная ставка (наивысшая ставка первой)";
    case 3: return "Отработанные моточасы (наибольшая наработка первой)";
    case 4: return "Идентификатор ID (наибольший ID первым)";
    default: return "По умолчанию";
    }
}

void AdapterManager::pqueueClear() {
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentPriorityCompare> empty1;
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentRateCompare> empty2;
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentHoursCompare> empty3;
    std::priority_queue<EquipmentRecord, std::vector<EquipmentRecord>, EquipmentIdCompare> empty4;
    std::swap(pqueueDefault_, empty1);
    std::swap(pqueueRate_, empty2);
    std::swap(pqueueHours_, empty3);
    std::swap(pqueueId_, empty4);
}

void AdapterManager::printAllSummary(std::ostream& os) const {
    os << "\n================================================================================\n"
       << "  СВОДНЫЙ ОТЧЁТ ДИСПЕТЧЕРСКОЙ СИСТЕМЫ УЧЁТА СТРОИТЕЛЬНОЙ ТЕХНИКИ\n"
       << "================================================================================\n";
    os << "1. Размер очереди заявок (std::queue): " << queueSize() << "\n";
    os << "2. Размер стека журнала возврата (std::stack): " << stackSize() << "\n";
    os << "3. Размер приоритетной очереди (std::priority_queue): " << pqueueSize() << "\n";

    queuePrint(os);
    stackPrint(os);
    pqueuePrint(os);
}

bool AdapterManager::exportAllToFile(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) return false;
    printAllSummary(ofs);
    return true;
}
