#include "Task.h"
#include "Utils.h"
#include <sstream>
#include <vector>
#include <stdexcept>

Task::Task(int id, std::string title, std::string description, Priority p, bool done)
    : id_(id), title_(std::move(title)), description_(std::move(description)), priority_(p), done_(done) {
}

std::string SimpleTask::detailsLine() const {
    std::ostringstream oss;
    oss << "(no extra details)";
    return oss.str();
}

std::string DeadlineTask::detailsLine() const {
    std::ostringstream oss;
    oss << "due: " << dueDate_;
    return oss.str();
}

std::string RecurringTask::detailsLine() const {
    std::ostringstream oss;
    oss << "every " << intervalDays_ << " day(s)";
    return oss.str();
}

// Format (one line):
// TYPE|id|done|priority|title|description|extra
// where title/description are escaped
std::string SimpleTask::serialize() const {
    std::ostringstream oss;
    oss << "SIMPLE" << '|'
        << id_ << '|'
        << (done_ ? 1 : 0) << '|'
        << (int)priority_ << '|'
        << utils::escapePipes(title_) << '|'
        << utils::escapePipes(description_) << '|'
        << ""; // extra empty
    return oss.str();
}

DeadlineTask::DeadlineTask(int id, std::string title, std::string description, Priority p, bool done, std::string dueDate)
    : Task(id, std::move(title), std::move(description), p, done), dueDate_(std::move(dueDate)) {
}

std::string DeadlineTask::serialize() const {
    std::ostringstream oss;
    oss << "DEADLINE" << '|'
        << id_ << '|'
        << (done_ ? 1 : 0) << '|'
        << (int)priority_ << '|'
        << utils::escapePipes(title_) << '|'
        << utils::escapePipes(description_) << '|'
        << utils::escapePipes(dueDate_);
    return oss.str();
}

RecurringTask::RecurringTask(int id, std::string title, std::string description, Priority p, bool done, int intervalDays)
    : Task(id, std::move(title), std::move(description), p, done), intervalDays_(intervalDays) {
}

std::string RecurringTask::serialize() const {
    std::ostringstream oss;
    oss << "RECURRING" << '|'
        << id_ << '|'
        << (done_ ? 1 : 0) << '|'
        << (int)priority_ << '|'
        << utils::escapePipes(title_) << '|'
        << utils::escapePipes(description_) << '|'
        << intervalDays_;
    return oss.str();
}

std::unique_ptr<Task> Task::deserialize(const std::string& line) {
    auto parts = utils::split(line, '|');
    if (parts.size() < 7) return nullptr;

    std::string type = parts[0];
    int id = std::stoi(parts[1]);
    bool done = (std::stoi(parts[2]) != 0);
    Priority pr = intToPriority(std::stoi(parts[3]));
    std::string title = utils::unescapePipes(parts[4]);
    std::string desc = utils::unescapePipes(parts[5]);
    std::string extra = utils::unescapePipes(parts[6]);

    if (type == "SIMPLE") {
        return std::make_unique<SimpleTask>(id, title, desc, pr, done);
    }
    if (type == "DEADLINE") {
        return std::make_unique<DeadlineTask>(id, title, desc, pr, done, extra);
    }
    if (type == "RECURRING") {
        int interval = 7;
        if (!extra.empty()) interval = std::stoi(extra);
        return std::make_unique<RecurringTask>(id, title, desc, pr, done, interval);
    }
    return nullptr;
}
