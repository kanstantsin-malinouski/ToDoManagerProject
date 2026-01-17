#include "TaskManager.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void TaskManager::add(std::unique_ptr<Task> t) {
    if (!t) return;
    nextId_ = std::max(nextId_, t->id() + 1);
    tasks_.push_back(std::move(t));
}

bool TaskManager::removeById(int id) {
    auto it = std::remove_if(tasks_.begin(), tasks_.end(),
        [id](const std::unique_ptr<Task>& t) { return t && t->id() == id; });
    if (it == tasks_.end()) return false;
    tasks_.erase(it, tasks_.end());
    return true;
}

Task* TaskManager::findById(int id) {
    for (auto& t : tasks_) {
        if (t && t->id() == id) return t.get();
    }
    return nullptr;
}

std::vector<Task*> TaskManager::searchByTitle(const std::string& query) const {
    std::vector<Task*> out;
    std::string q = utils::toLower(utils::trim(query));
    for (auto& t : tasks_) {
        if (!t) continue;
        std::string title = utils::toLower(t->title());
        if (title.find(q) != std::string::npos) out.push_back(t.get());
    }
    return out;
}

void TaskManager::list(bool onlyOpen) const {
    if (tasks_.empty()) {
        std::cout << "No tasks.\n";
        return;
    }

    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "ID  | Done | Pri | Type      | Title                         | Details\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    for (auto& t : tasks_) {
        if (!t) continue;
        if (onlyOpen && t->done()) continue;

        std::cout << t->id() << (t->id() < 10 ? "   " : (t->id() < 100 ? "  " : " "))
            << "| " << (t->done() ? "Yes " : "No  ")
            << "| " << (int)t->priority() << "   "
            << "| " << t->typeName();
        // align type column roughly
        if (t->typeName().size() < 9) std::cout << std::string(9 - t->typeName().size(), ' ');
        std::cout << "| ";

        std::string title = t->title();
        if (title.size() > 28) title = title.substr(0, 25) + "...";
        std::cout << title;
        if (title.size() < 28) std::cout << std::string(28 - title.size(), ' ');

        std::cout << " | " << t->detailsLine() << "\n";
    }

    std::cout << "--------------------------------------------------------------------------------\n";
}

bool TaskManager::toggleDone(int id) {
    Task* t = findById(id);
    if (!t) return false;
    t->toggleDone();
    return true;
}

void TaskManager::sortByPriorityThenId() {
    std::sort(tasks_.begin(), tasks_.end(),
        [](const std::unique_ptr<Task>& a, const std::unique_ptr<Task>& b) {
            if (!a) return false;
            if (!b) return true;
            if ((int)a->priority() != (int)b->priority())
                return (int)a->priority() > (int)b->priority(); // High first
            return a->id() < b->id();
        });
}

void TaskManager::sortByTitle() {
    std::sort(tasks_.begin(), tasks_.end(),
        [](const std::unique_ptr<Task>& a, const std::unique_ptr<Task>& b) {
            if (!a) return false;
            if (!b) return true;
            return utils::toLower(a->title()) < utils::toLower(b->title());
        });
}

bool TaskManager::saveToFile(const std::string& path) const {
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out) return false;

    // header: nextId
    out << "NEXTID|" << nextId_ << "\n";
    for (auto& t : tasks_) {
        if (!t) continue;
        out << t->serialize() << "\n";
    }
    return true;
}

bool TaskManager::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    tasks_.clear();
    nextId_ = 1;

    std::string line;
    while (std::getline(in, line)) {
        line = utils::trim(line);
        if (line.empty()) continue;

        // header
        if (line.rfind("NEXTID|", 0) == 0) {
            auto parts = utils::split(line, '|');
            if (parts.size() >= 2) nextId_ = std::stoi(parts[1]);
            continue;
        }

        auto t = Task::deserialize(line);
        if (t) add(std::move(t));
    }
    // nextId could be behind if file didn't have header
    int maxId = 0;
    for (auto& t : tasks_) if (t) maxId = std::max(maxId, t->id());
    nextId_ = std::max(nextId_, maxId + 1);

    return true;
}
