#pragma once
#include <string>
#include <memory>

enum class Priority {
    Low = 1,
    Medium = 2,
    High = 3
};

inline std::string priorityToString(Priority p) {
    switch (p) {
    case Priority::Low: return "Low";
    case Priority::Medium: return "Medium";
    case Priority::High: return "High";
    default: return "Unknown";
    }
}

inline Priority intToPriority(int v) {
    if (v <= 1) return Priority::Low;
    if (v == 2) return Priority::Medium;
    return Priority::High;
}

class Task {
protected:
    int id_ = 0;
    std::string title_;
    std::string description_;
    Priority priority_ = Priority::Medium;
    bool done_ = false;

public:
    Task() = default;
    Task(int id, std::string title, std::string description, Priority p, bool done);
    virtual ~Task() = default;

    int id() const { return id_; }
    const std::string& title() const { return title_; }
    const std::string& description() const { return description_; }
    Priority priority() const { return priority_; }
    bool done() const { return done_; }

    void setTitle(std::string t) { title_ = std::move(t); }
    void setDescription(std::string d) { description_ = std::move(d); }
    void setPriority(Priority p) { priority_ = p; }
    void setDone(bool d) { done_ = d; }
    void toggleDone() { done_ = !done_; }

    virtual std::string typeName() const = 0;
    virtual std::string detailsLine() const = 0;

    // Persistence (single-line record)
    virtual std::string serialize() const = 0;

    // Factory
    static std::unique_ptr<Task> deserialize(const std::string& line);
};

class SimpleTask final : public Task {
public:
    using Task::Task;
    std::string typeName() const override { return "Simple"; }
    std::string detailsLine() const override;
    std::string serialize() const override;
};

class DeadlineTask final : public Task {
    std::string dueDate_; // YYYY-MM-DD
public:
    DeadlineTask() = default;
    DeadlineTask(int id, std::string title, std::string description, Priority p, bool done, std::string dueDate);

    const std::string& dueDate() const { return dueDate_; }
    void setDueDate(std::string d) { dueDate_ = std::move(d); }

    std::string typeName() const override { return "Deadline"; }
    std::string detailsLine() const override;
    std::string serialize() const override;
};

class RecurringTask final : public Task {
    int intervalDays_ = 7;
public:
    RecurringTask() = default;
    RecurringTask(int id, std::string title, std::string description, Priority p, bool done, int intervalDays);

    int intervalDays() const { return intervalDays_; }
    void setIntervalDays(int d) { intervalDays_ = d; }

    std::string typeName() const override { return "Recurring"; }
    std::string detailsLine() const override;
    std::string serialize() const override;
};
