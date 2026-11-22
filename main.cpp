#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "AdapterManager.h"
#include "Input.h"

using namespace std;

static EquipmentRecord inputNewRecord(AdapterManager& mgr) {
    int id = mgr.generateNextId();
    cout << "\n--- Добавление строительной техники (Назначен ID: " << id << ") ---\n";
    string model = readLine(cin, "Модель техники (напр. Caterpillar 320D): ", 40);
    string reg = readLine(cin, "Гос. регистрационный номер (напр. 1928 BK-7): ", 20);
    string site = readLine(cin, "Строительный объект (напр. ЖК Минск-Мир): ", 40);
    double rate = readDouble(cin, "Тариф аренды, руб/час (1 - 10000): ", 1.0, 10000.0);
    int hours = readInt(cin, "Отработано моточасов (0 - 5000): ", 0, 5000);

    cout << "Выберите категорию выполняемых работ:\n"
         << " 1 - Земляные работы\n"
         << " 2 - Монтажные работы\n"
         << " 3 - Погрузочные работы\n"
         << " 4 - Транспортные работы\n";
    int catChoice = readInt(cin, "Категория (1-4): ", 1, 4);
    string cat = "Земляные работы";
    if (catChoice == 2) cat = "Монтажные работы";
    else if (catChoice == 3) cat = "Погрузочные работы";
    else if (catChoice == 4) cat = "Транспортные работы";

    cout << "Уровень диспетчерского приоритета:\n"
         << " 1 - Плановый (обычная плановая заявка)\n"
         << " 2 - Срочный (срочное выделение техники)\n"
         << " 3 - Аварийно-критический (ликвидация ЧС / максимальный приоритет)\n";
    int priority = readInt(cin, "Приоритет (1-3): ", 1, 3);

    return EquipmentRecord(id, model, reg, site, rate, hours, cat, priority);
}

static void menuQueue(AdapterManager& mgr) {
    while (true) {
        clearScreen();
        cout << "========================================================================\n"
             << "  ДИСПЕТЧЕРСКАЯ ОЧЕРЕДЬ ЗАЯВОК (std::queue, FIFO)\n"
             << "========================================================================\n"
             << "1.  Просмотр всех заявок в очереди (табличный вывод)\n"
             << "2.  Добавить новую заявку в очередь (push)\n"
             << "3.  Извлечь и обслужить заявку из начала (front + pop)\n"
             << "4.  Посмотреть заявку во главе (front) и в конце (back)\n"
             << "5.  Редактировать заявку по ID\n"
             << "6.  Удалить заявку по ID (отмена заявки)\n"
             << "7.  Поиск заявки (по модели, госномеру, объекту, категории)\n"
             << "8.  Фильтр заявок по категории работ\n"
             << "9.  Сортировка очереди по выбранному критерию\n"
             << "10. Экспорт содержимого очереди в файл\n"
             << "11. Очистить очередь\n"
             << "0.  Назад в главное меню\n"
             << "------------------------------------------------------------------------\n";

        int choice = readInt(cin, "Ваш выбор: ", 0, 11);
        if (choice == 0) break;

        clearScreen();
        switch (choice) {
        case 1:
            mgr.queuePrint(cout);
            break;
        case 2: {
            EquipmentRecord rec = inputNewRecord(mgr);
            mgr.queuePush(rec);
            cout << "\nЗаявка успешно добавлена в очередь!\n";
            break;
        }
        case 3: {
            EquipmentRecord rec;
            if (mgr.queuePop(rec)) {
                cout << "Обслужена и извлечена из очереди заявка:\n";
                EquipmentRecord::printHeader(cout);
                rec.printRow(cout);
            } else {
                cout << "Очередь пуста, нечего извлекать.\n";
            }
            break;
        }
        case 4: {
            EquipmentRecord f, b;
            if (mgr.queueFront(f)) {
                cout << "Заявка в начале очереди (front):\n";
                EquipmentRecord::printHeader(cout);
                f.printRow(cout);
                if (mgr.queueBack(b)) {
                    cout << "\nЗаявка в конце очереди (back):\n";
                    b.printRow(cout);
                }
            } else {
                cout << "Очередь пуста.\n";
            }
            break;
        }
        case 5: {
            mgr.queuePrint(cout);
            if (mgr.queueEmpty()) break;
            int id = readInt(cin, "Введите ID заявки для редактирования: ", 1, 99999);
            double newRate = readDouble(cin, "Новый тариф (руб/ч): ", 1.0, 10000.0);
            int newHours = readInt(cin, "Новые отработанные часы: ", 0, 5000);
            string newSite = readLine(cin, "Новый стройобъект: ", 40);
            int newPriority = readInt(cin, "Новый приоритет (1-3): ", 1, 3);

            if (mgr.queueEditById(id, newRate, newHours, newSite, newPriority)) {
                cout << "Заявка с ID " << id << " успешно обновлена!\n";
            } else {
                cout << "Заявка с ID " << id << " не найдена.\n";
            }
            break;
        }
        case 6: {
            mgr.queuePrint(cout);
            if (mgr.queueEmpty()) break;
            int id = readInt(cin, "Введите ID заявки для удаления: ", 1, 99999);
            if (mgr.queueRemoveById(id)) {
                cout << "Заявка с ID " << id << " успешно удалена из очереди!\n";
            } else {
                cout << "Заявка с ID " << id << " не найдена.\n";
            }
            break;
        }
        case 7: {
            string q = readLine(cin, "Введите строку для поиска: ", 50);
            auto found = mgr.queueSearch(q);
            if (found.empty()) {
                cout << "По запросу '" << q << "' ничего не найдено.\n";
            } else {
                cout << "\nРезультаты поиска в очереди:\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : found) r.printRow(cout);
            }
            break;
        }
        case 8: {
            cout << "Выберите категорию:\n 1 - Земляные\n 2 - Монтажные\n 3 - Погрузочные\n 4 - Транспортные\n";
            int c = readInt(cin, "Выбор: ", 1, 4);
            string cat = (c == 1) ? "Земляные работы" : (c == 2 ? "Монтажные работы" : (c == 3 ? "Погрузочные работы" : "Транспортные работы"));
            auto filtered = mgr.queueFilterByCategory(cat);
            if (filtered.empty()) {
                cout << "Техники в категории '" << cat << "' не найдено.\n";
            } else {
                cout << "\nТехника по категории '" << cat << "':\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : filtered) r.printRow(cout);
            }
            break;
        }
        case 9: {
            cout << "Критерии сортировки очереди:\n"
                 << " 1 - По инвентарному номеру ID (возрастание)\n"
                 << " 2 - По тарифной ставке (убывание)\n"
                 << " 3 - По наработке моточасов (убывание)\n"
                 << " 4 - По суммарной стоимости (убывание)\n"
                 << " 5 - По приоритету (убывание)\n"
                 << " 6 - По наименованию модели (по алфавиту)\n";
            int crit = readInt(cin, "Выберите критерий (1-6): ", 1, 6);
            mgr.queueSort(crit);
            cout << "Очередь успешно отсортирована!\n";
            mgr.queuePrint(cout);
            break;
        }
        case 10: {
            string fn = readLine(cin, "Введите имя файла для экспорта (напр. queue_data.txt): ", 50);
            if (mgr.queueExportToFile(fn)) {
                cout << "Данные очереди успешно сохранены в файл '" << fn << "'!\n";
            } else {
                cout << "Ошибка при открытии файла для записи.\n";
            }
            break;
        }
        case 11:
            if (askYesNo(cin, "Вы уверены, что хотите очистить всю очередь? (y/n): ")) {
                mgr.queueClear();
                cout << "Очередь очищена.\n";
            }
            break;
        }
        pauseScreen();
    }
}

static void menuStack(AdapterManager& mgr) {
    while (true) {
        clearScreen();
        cout << "========================================================================\n"
             << "  ЖУРНАЛ ВОЗВРАТА ТЕХНИКИ / ОТКАТА ОПЕРАЦИЙ (std::stack, LIFO)\n"
             << "========================================================================\n"
             << "1.  Просмотр всех записей в стеке (от вершины ко дну)\n"
             << "2.  Зафиксировать возврат техники на вершину стека (push)\n"
             << "3.  Извлечь последнюю запись с вершины (top + pop)\n"
             << "4.  Посмотреть запись на вершине (top)\n"
             << "5.  Редактировать запись по ID\n"
             << "6.  Удалить запись по ID\n"
             << "7.  Поиск записи (по модели, госномеру, объекту, категории)\n"
             << "8.  Фильтр записей по категории работ\n"
             << "9.  Сортировка стека по критерию\n"
             << "10. Экспорт содержимого стека в файл\n"
             << "11. Очистить стек\n"
             << "0.  Назад в главное меню\n"
             << "------------------------------------------------------------------------\n";

        int choice = readInt(cin, "Ваш выбор: ", 0, 11);
        if (choice == 0) break;

        clearScreen();
        switch (choice) {
        case 1:
            mgr.stackPrint(cout);
            break;
        case 2: {
            EquipmentRecord rec = inputNewRecord(mgr);
            mgr.stackPush(rec);
            cout << "\nЗапись успешно помещена на вершину стека!\n";
            break;
        }
        case 3: {
            EquipmentRecord rec;
            if (mgr.stackPop(rec)) {
                cout << "Извлечена запись с вершины стека:\n";
                EquipmentRecord::printHeader(cout);
                rec.printRow(cout);
            } else {
                cout << "Стек пуст, нечего извлекать.\n";
            }
            break;
        }
        case 4: {
            EquipmentRecord rec;
            if (mgr.stackTop(rec)) {
                cout << "Запись на вершине стека (top):\n";
                EquipmentRecord::printHeader(cout);
                rec.printRow(cout);
            } else {
                cout << "Стек пуст.\n";
            }
            break;
        }
        case 5: {
            mgr.stackPrint(cout);
            if (mgr.stackEmpty()) break;
            int id = readInt(cin, "Введите ID записи для редактирования: ", 1, 99999);
            double newRate = readDouble(cin, "Новый тариф (руб/ч): ", 1.0, 10000.0);
            int newHours = readInt(cin, "Новые отработанные часы: ", 0, 5000);
            string newSite = readLine(cin, "Новый стройобъект: ", 40);
            int newPriority = readInt(cin, "Новый приоритет (1-3): ", 1, 3);

            if (mgr.stackEditById(id, newRate, newHours, newSite, newPriority)) {
                cout << "Запись с ID " << id << " в стеке успешно обновлена!\n";
            } else {
                cout << "Запись с ID " << id << " не найдена.\n";
            }
            break;
        }
        case 6: {
            mgr.stackPrint(cout);
            if (mgr.stackEmpty()) break;
            int id = readInt(cin, "Введите ID записи для удаления: ", 1, 99999);
            if (mgr.stackRemoveById(id)) {
                cout << "Запись с ID " << id << " успешно удалена из стека!\n";
            } else {
                cout << "Запись с ID " << id << " не найдена.\n";
            }
            break;
        }
        case 7: {
            string q = readLine(cin, "Введите строку для поиска: ", 50);
            auto found = mgr.stackSearch(q);
            if (found.empty()) {
                cout << "По запросу '" << q << "' ничего не найдено.\n";
            } else {
                cout << "\nРезультаты поиска в стеке:\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : found) r.printRow(cout);
            }
            break;
        }
        case 8: {
            cout << "Выберите категорию:\n 1 - Земляные\n 2 - Монтажные\n 3 - Погрузочные\n 4 - Транспортные\n";
            int c = readInt(cin, "Выбор: ", 1, 4);
            string cat = (c == 1) ? "Земляные работы" : (c == 2 ? "Монтажные работы" : (c == 3 ? "Погрузочные работы" : "Транспортные работы"));
            auto filtered = mgr.stackFilterByCategory(cat);
            if (filtered.empty()) {
                cout << "Техники в категории '" << cat << "' не найдено.\n";
            } else {
                cout << "\nТехника в стеке по категории '" << cat << "':\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : filtered) r.printRow(cout);
            }
            break;
        }
        case 9: {
            cout << "Критерии сортировки стека (элемент с наибольшим значением окажется на вершине top):\n"
                 << " 1 - По ID (наименьший ID на вершине)\n"
                 << " 2 - По тарифной ставке (наибольший тариф на вершине)\n"
                 << " 3 - По наработке моточасов (наибольшие часы на вершине)\n"
                 << " 4 - По суммарной стоимости (наибольшая сумма на вершине)\n"
                 << " 5 - По приоритету (наивысший приоритет на вершине)\n";
            int crit = readInt(cin, "Выберите критерий (1-5): ", 1, 5);
            mgr.stackSort(crit);
            cout << "Стек успешно отсортирован!\n";
            mgr.stackPrint(cout);
            break;
        }
        case 10: {
            string fn = readLine(cin, "Введите имя файла для экспорта (напр. stack_data.txt): ", 50);
            if (mgr.stackExportToFile(fn)) {
                cout << "Данные стека успешно сохранены в файл '" << fn << "'!\n";
            } else {
                cout << "Ошибка при открытии файла для записи.\n";
            }
            break;
        }
        case 11:
            if (askYesNo(cin, "Вы уверены, что хотите очистить весь стек? (y/n): ")) {
                mgr.stackClear();
                cout << "Стек очищен.\n";
            }
            break;
        }
        pauseScreen();
    }
}

static void menuPriorityQueue(AdapterManager& mgr) {
    while (true) {
        clearScreen();
        cout << "========================================================================\n"
             << "  ОЧЕРЕДЬ С ПРИОРИТЕТОМ (std::priority_queue, Max-Heap)\n"
             << "  Текущий режим: " << mgr.pqueueCurrentCriterionName() << "\n"
             << "========================================================================\n"
             << "1.  Просмотр всех записей (от наивысшего приоритета к наименьшему)\n"
             << "2.  Добавить заявку с приоритетом (push)\n"
             << "3.  Извлечь самую приоритетную заявку (top + pop)\n"
             << "4.  Посмотреть самую приоритетную заявку (top)\n"
             << "5.  Редактировать заявку по ID\n"
             << "6.  Удалить заявку по ID\n"
             << "7.  Поиск заявки (по модели, госномеру, объекту, категории)\n"
             << "8.  Фильтр заявок по категории работ\n"
             << "9.  Переключить критерий упорядочения кучи (Компараторы)\n"
             << "10. Экспорт содержимого приоритетной очереди в файл\n"
             << "11. Очистить приоритетную очередь\n"
             << "0.  Назад в главное меню\n"
             << "------------------------------------------------------------------------\n";

        int choice = readInt(cin, "Ваш выбор: ", 0, 11);
        if (choice == 0) break;

        clearScreen();
        switch (choice) {
        case 1:
            mgr.pqueuePrint(cout);
            break;
        case 2: {
            EquipmentRecord rec = inputNewRecord(mgr);
            mgr.pqueuePush(rec);
            cout << "\nЗаявка успешно добавлена в очередь с приоритетом!\n";
            break;
        }
        case 3: {
            EquipmentRecord rec;
            if (mgr.pqueuePop(rec)) {
                cout << "Извлечена наиболее приоритетная заявка (top):\n";
                EquipmentRecord::printHeader(cout);
                rec.printRow(cout);
            } else {
                cout << "Очередь с приоритетом пуста.\n";
            }
            break;
        }
        case 4: {
            EquipmentRecord rec;
            if (mgr.pqueueTop(rec)) {
                cout << "Заявка с наивысшим приоритетом во главе кучи (top):\n";
                EquipmentRecord::printHeader(cout);
                rec.printRow(cout);
            } else {
                cout << "Очередь с приоритетом пуста.\n";
            }
            break;
        }
        case 5: {
            mgr.pqueuePrint(cout);
            if (mgr.pqueueEmpty()) break;
            int id = readInt(cin, "Введите ID заявки для редактирования: ", 1, 99999);
            double newRate = readDouble(cin, "Новый тариф (руб/ч): ", 1.0, 10000.0);
            int newHours = readInt(cin, "Новые отработанные часы: ", 0, 5000);
            string newSite = readLine(cin, "Новый стройобъект: ", 40);
            int newPriority = readInt(cin, "Новый приоритет (1-3): ", 1, 3);

            if (mgr.pqueueEditById(id, newRate, newHours, newSite, newPriority)) {
                cout << "Заявка с ID " << id << " успешно обновлена и куча перестроена!\n";
            } else {
                cout << "Заявка с ID " << id << " не найдена.\n";
            }
            break;
        }
        case 6: {
            mgr.pqueuePrint(cout);
            if (mgr.pqueueEmpty()) break;
            int id = readInt(cin, "Введите ID заявки для удаления: ", 1, 99999);
            if (mgr.pqueueRemoveById(id)) {
                cout << "Заявка с ID " << id << " успешно удалена из очереди с приоритетом!\n";
            } else {
                cout << "Заявка с ID " << id << " не найдена.\n";
            }
            break;
        }
        case 7: {
            string q = readLine(cin, "Введите строку для поиска: ", 50);
            auto found = mgr.pqueueSearch(q);
            if (found.empty()) {
                cout << "По запросу '" << q << "' ничего не найдено.\n";
            } else {
                cout << "\nРезультаты поиска в приоритетной очереди:\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : found) r.printRow(cout);
            }
            break;
        }
        case 8: {
            cout << "Выберите категорию:\n 1 - Земляные\n 2 - Монтажные\n 3 - Погрузочные\n 4 - Транспортные\n";
            int c = readInt(cin, "Выбор: ", 1, 4);
            string cat = (c == 1) ? "Земляные работы" : (c == 2 ? "Монтажные работы" : (c == 3 ? "Погрузочные работы" : "Транспортные работы"));
            auto filtered = mgr.pqueueFilterByCategory(cat);
            if (filtered.empty()) {
                cout << "Техники в категории '" << cat << "' не найдено.\n";
            } else {
                cout << "\nТехника по категории '" << cat << "':\n";
                EquipmentRecord::printHeader(cout);
                for (const auto& r : filtered) r.printRow(cout);
            }
            break;
        }
        case 9: {
            cout << "Доступные режимы компаратора для std::priority_queue:\n"
                 << " 1 - Уровень приоритета (3-Аварийный > 2-Срочный > 1-Плановый) + тариф\n"
                 << " 2 - Часовая тарифная ставка аренды (наибольшая на вершине)\n"
                 << " 3 - Наработка моточасов (наибольшая на вершине)\n"
                 << " 4 - Идентификатор ID (наибольший ID на вершине)\n";
            int crit = readInt(cin, "Выберите критерий кучи (1-4): ", 1, 4);
            mgr.pqueueSetCriterion(crit);
            cout << "Критерий переключен на: " << mgr.pqueueCurrentCriterionName() << "\n";
            mgr.pqueuePrint(cout);
            break;
        }
        case 10: {
            string fn = readLine(cin, "Введите имя файла для экспорта (напр. priority_data.txt): ", 50);
            if (mgr.pqueueExportToFile(fn)) {
                cout << "Данные приоритетной очереди успешно сохранены в файл '" << fn << "'!\n";
            } else {
                cout << "Ошибка при открытии файла для записи.\n";
            }
            break;
        }
        case 11:
            if (askYesNo(cin, "Вы уверены, что хотите очистить приоритетную очередь? (y/n): ")) {
                mgr.pqueueClear();
                cout << "Приоритетная очередь очищена.\n";
            }
            break;
        }
        pauseScreen();
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    AdapterManager mgr;
    mgr.seedDemoData();

    while (true) {
        clearScreen();
        cout << "========================================================================\n"
             << "   ЛАБОРАТОРНАЯ РАБОТА №5: АДАПТЕРЫ КОНТЕЙНЕРОВ STL (Вариант 10)\n"
             << "   Тема: Учет занятости строительной техники в строительной компании\n"
             << "========================================================================\n"
             << "1. Работа с Очередью заявок на выделение техники (std::queue, FIFO)\n"
             << "2. Работа со Стеком журнала возврата техники (std::stack, LIFO)\n"
             << "3. Работа с Приоритетной очередью заявок (std::priority_queue, Heap)\n"
             << "4. Заполнить все контейнеры демонстрационными данными (Seed Data)\n"
             << "5. Сводный отчёт и экспорт всех трёх адаптеров в единый файл\n"
             << "0. Завершить работу и выйти из программы\n"
             << "========================================================================\n";

        int choice = readInt(cin, "Выберите пункт меню (0-5): ", 0, 5);
        if (choice == 0) {
            if (askYesNo(cin, "Вы действительно хотите выйти из программы? (y/n): ")) {
                cout << "\nПрограмма успешно завершена. До свидания!\n";
                break;
            }
            continue;
        }

        switch (choice) {
        case 1:
            menuQueue(mgr);
            break;
        case 2:
            menuStack(mgr);
            break;
        case 3:
            menuPriorityQueue(mgr);
            break;
        case 4:
            clearScreen();
            mgr.seedDemoData();
            cout << "Все три контейнера-адаптера успешно наполнены 8 демонстрационными записями техники!\n";
            mgr.printAllSummary(cout);
            pauseScreen();
            break;
        case 5: {
            clearScreen();
            mgr.printAllSummary(cout);
            string fn = readLine(cin, "Введите имя файла для полного экспорта (напр. full_summary.txt): ", 50);
            if (mgr.exportAllToFile(fn)) {
                cout << "Полный сводный отчёт успешно сохранён в файл '" << fn << "'!\n";
            } else {
                cout << "Ошибка сохранения файла.\n";
            }
            pauseScreen();
            break;
        }
        }
    }

    return 0;
}
