#pragma once
#include "Task.h"
#include <vector>
#include <memory>
#include <string>

class TaskManager {
    std::vector<std::unique_ptr<Task>> tasks_;
    int nextId_ = 1;

public:
    void add(std::unique_ptr<Task> t);
    bool removeById(int id);
    Task* findById(int id);
    const std::vector<std::unique_ptr<Task>>& all() const { return tasks_; }

    std::vector<Task*> searchByTitle(const std::string& query) const;

    void list(bool onlyOpen = false) const;
    bool toggleDone(int id);

    void sortByPriorityThenId();
    void sortByTitle();

    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

    int nextId() const { return nextId_; }
};

