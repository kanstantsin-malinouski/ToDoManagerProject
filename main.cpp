#include <iostream>
#include <memory>
#include "TaskManager.h"
#include "Utils.h"

static void printMenu() {
    std::cout << "\n=== Task Manager (OOP) ===\n"
        << "1) Add task\n"
        << "2) List all tasks\n"
        << "3) List open tasks (not done)\n"
        << "4) Toggle done by ID\n"
        << "5) Remove by ID\n"
        << "6) Search by title\n"
        << "7) Sort (priority desc, id asc)\n"
        << "8) Sort (title asc)\n"
        << "9) Save\n"
        << "10) Load\n"
        << "0) Exit\n";
}

static std::unique_ptr<Task> createTaskInteractively(int id) {
    int type = utils::readInt("Task type: 1=Simple, 2=Deadline, 3=Recurring: ", 1, 3);
    std::string title = utils::readLine("Title: ");
    std::string desc = utils::readLine("Description (optional): ", true);

    int pr = utils::readInt("Priority: 1=Low, 2=Medium, 3=High: ", 1, 3);
    Priority priority = intToPriority(pr);

    bool done = false;

    if (type == 1) {
        return std::make_unique<SimpleTask>(id, title, desc, priority, done);
    }
    if (type == 2) {
        std::string due;
        while (true) {
            due = utils::readLine("Due date (YYYY-MM-DD): ");
            if (utils::isValidDateYYYYMMDD(due)) break;
            std::cout << "Invalid date format. Example: 2026-01-17\n";
        }
        return std::make_unique<DeadlineTask>(id, title, desc, priority, done, due);
    }

    int interval = utils::readInt("Repeat every N days (1..365): ", 1, 365);
    return std::make_unique<RecurringTask>(id, title, desc, priority, done, interval);
}

int main() {
    TaskManager tm;
    const std::string dbPath = "tasks.db";

    // Auto-load if file exists
    tm.loadFromFile(dbPath);

    while (true) {
        printMenu();
        int choice = utils::readInt("Choose: ", 0, 10);

        if (choice == 0) {
            tm.saveToFile(dbPath);
            std::cout << "Saved to " << dbPath << ". Bye!\n";
            break;
        }

        switch (choice) {
        case 1: {
            auto t = createTaskInteractively(tm.nextId());
            tm.add(std::move(t));
            std::cout << "Task added.\n";
            break;
        }
        case 2:
            tm.list(false);
            break;
        case 3:
            tm.list(true);
            break;
        case 4: {
            int id = utils::readInt("Enter ID: ", 1, 1000000);
            if (tm.toggleDone(id)) std::cout << "Toggled.\n";
            else std::cout << "Task not found.\n";
            break;
        }
        case 5: {
            int id = utils::readInt("Enter ID: ", 1, 1000000);
            if (tm.removeById(id)) std::cout << "Removed.\n";
            else std::cout << "Task not found.\n";
            break;
        }
        case 6: {
            std::string q = utils::readLine("Search query: ");
            auto results = tm.searchByTitle(q);
            if (results.empty()) {
                std::cout << "No matches.\n";
            }
            else {
                std::cout << "Matches:\n";
                for (auto* t : results) {
                    std::cout << "- [" << t->id() << "] "
                        << (t->done() ? "[done] " : "")
                        << t->title() << " (" << t->typeName() << ", "
                        << priorityToString(t->priority()) << ")\n";
                }
            }
            break;
        }
        case 7:
            tm.sortByPriorityThenId();
            std::cout << "Sorted.\n";
            break;
        case 8:
            tm.sortByTitle();
            std::cout << "Sorted.\n";
            break;
        case 9:
            if (tm.saveToFile(dbPath)) std::cout << "Saved to " << dbPath << "\n";
            else std::cout << "Save failed.\n";
            break;
        case 10:
            if (tm.loadFromFile(dbPath)) std::cout << "Loaded from " << dbPath << "\n";
            else std::cout << "Load failed (file missing?).\n";
            break;
        }
    }

    return 0;
}
