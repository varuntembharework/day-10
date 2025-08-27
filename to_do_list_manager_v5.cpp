#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
using namespace std;

// ---------------------------
// Utility: Get Current Date
// ---------------------------
string getTodayDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, 11, "%Y-%m-%d", ltm);
    return string(buffer);
}

// ---------------------------
// Task Class
// ---------------------------
class Task {
private:
    int id;
    string title;
    string description;
    string priority;     // High / Medium / Low
    string dueDate;      // YYYY-MM-DD
    bool completed;

public:
    Task(int id, string title, string description, string priority, string dueDate, bool completed = false) {
        this->id = id;
        this->title = title;
        this->description = description;
        this->priority = priority;
        this->dueDate = dueDate;
        this->completed = completed;
    }

    // Mark as completed
    void markCompleted() { completed = true; }

    // Edit task details
    void editTask(string newTitle, string newDesc, string newPriority, string newDueDate) {
        title = newTitle;
        description = newDesc;
        priority = newPriority;
        dueDate = newDueDate;
    }

    // Display task (formatted in table row)
    void displayTask() const {
        cout << left << setw(5) << id
             << setw(20) << title.substr(0, 18)   // truncate long titles
             << setw(12) << priority
             << setw(12) << dueDate
             << setw(12) << (completed ? "Completed" : "Pending");

        // Highlight overdue
        if (!completed && dueDate < getTodayDate())
            cout << "  (Overdue!)";

        cout << endl;
    }

    // Getters
    int getId() const { return id; }
    bool isCompleted() const { return completed; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    string getPriority() const { return priority; }
    string getDueDate() const { return dueDate; }

    // Convert to file string
    string toFileString() const {
        return to_string(id) + "|" + title + "|" + description + "|" + priority + "|" + dueDate + "|" + (completed ? "1" : "0");
    }

    // Load from file string
    static Task fromFileString(const string& line) {
        int id;
        string title, description, priority, dueDate, status;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);

        id = stoi(line.substr(0, p1));
        title = line.substr(p1 + 1, p2 - p1 - 1);
        description = line.substr(p2 + 1, p3 - p2 - 1);
        priority = line.substr(p3 + 1, p4 - p3 - 1);
        dueDate = line.substr(p4 + 1, p5 - p4 - 1);
        status = line.substr(p5 + 1);

        bool completed = (status == "1");
        return Task(id, title, description, priority, dueDate, completed);
    }
};

// ---------------------------
// File Handling Functions
// ---------------------------
void saveTasks(const vector<Task>& tasks, const string& filename) {
    ofstream fout(filename);
    for (const auto &task : tasks)
        fout << task.toFileString() << endl;
    fout.close();
}

void loadTasks(vector<Task>& tasks, const string& filename, int &nextId) {
    ifstream fin(filename);
    string line;
    int maxId = 0;
    while (getline(fin, line)) {
        Task t = Task::fromFileString(line);
        tasks.push_back(t);
        if (t.getId() > maxId) maxId = t.getId();
    }
    fin.close();
    nextId = maxId + 1;
}

// ---------------------------
// Display Helpers
// ---------------------------
void displayHeader() {
    cout << left << setw(5) << "ID"
         << setw(20) << "Title"
         << setw(12) << "Priority"
         << setw(12) << "Due Date"
         << setw(12) << "Status" << endl;
    cout << string(65, '-') << endl;
}

// ---------------------------
// Main Program
// ---------------------------
int main() {
    vector<Task> tasks;
    int choice;
    int nextId = 1;
    string filename = "tasks.txt";

    loadTasks(tasks, filename, nextId);

    while (true) {
        cout << "\n===== TO-DO LIST MANAGER (Day 10 Final) =====" << endl;
        cout << "1. Add Task" << endl;
        cout << "2. View All Tasks" << endl;
        cout << "3. Mark Task as Completed" << endl;
        cout << "4. Delete Task" << endl;
        cout << "5. Edit Task" << endl;
        cout << "6. Search Tasks" << endl;
        cout << "7. Export Tasks to CSV" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string title, description, priority, dueDate;
            cout << "Enter Title: "; getline(cin, title);
            cout << "Enter Description: "; getline(cin, description);
            cout << "Enter Priority (High/Medium/Low): "; getline(cin, priority);
            cout << "Enter Due Date (YYYY-MM-DD): "; getline(cin, dueDate);

            Task newTask(nextId, title, description, priority, dueDate);
            tasks.push_back(newTask);
            saveTasks(tasks, filename);
            cout << "Task added successfully! [ID: " << nextId << "]\n";
            nextId++;
        }
        else if (choice == 2) {
            if (tasks.empty()) cout << "No tasks available.\n";
            else {
                displayHeader();
                for (const auto &task : tasks) task.displayTask();
            }
        }
        else if (choice == 3) {
            int id; cout << "Enter Task ID to mark completed: "; cin >> id;
            bool found = false;
            for (auto &task : tasks) {
                if (task.getId() == id) {
                    task.markCompleted();
                    saveTasks(tasks, filename);
                    cout << "Task " << id << " marked as completed!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Task not found!\n";
        }
        else if (choice == 4) {
            int id; cout << "Enter Task ID to delete: "; cin >> id;
            char confirm; cout << "Are you sure? (y/n): "; cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                bool found = false;
                for (auto it = tasks.begin(); it != tasks.end(); ++it) {
                    if (it->getId() == id) {
                        tasks.erase(it);
                        saveTasks(tasks, filename);
                        cout << "Task deleted successfully!\n";
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Task not found!\n";
            } else {
                cout << "Delete cancelled.\n";
            }
        }
        else if (choice == 5) {
            int id; cout << "Enter Task ID to edit: "; cin >> id; cin.ignore();
            bool found = false;
            for (auto &task : tasks) {
                if (task.getId() == id) {
                    string newTitle, newDesc, newPriority, newDueDate;
                    cout << "Enter New Title: "; getline(cin, newTitle);
                    cout << "Enter New Description: "; getline(cin, newDesc);
                    cout << "Enter New Priority: "; getline(cin, newPriority);
                    cout << "Enter New Due Date (YYYY-MM-DD): "; getline(cin, newDueDate);

                    task.editTask(newTitle, newDesc, newPriority, newDueDate);
                    saveTasks(tasks, filename);
                    cout << "Task updated successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Task not found!\n";
        }
        else if (choice == 6) {
            string keyword; cout << "Enter keyword to search: "; getline(cin, keyword);
            displayHeader();
            bool found = false;
            for (const auto &task : tasks) {
                if (task.getTitle().find(keyword) != string::npos ||
                    task.getDescription().find(keyword) != string::npos) {
                    task.displayTask();
                    found = true;
                }
            }
            if (!found) cout << "No matching tasks found.\n";
        }
        else if (choice == 7) {
            ofstream csv("tasks.csv");
            csv << "ID,Title,Description,Priority,DueDate,Status\n";
            for (const auto &task : tasks) {
                csv << task.getId() << ","
                    << task.getTitle() << ","
                    << task.getDescription() << ","
                    << task.getPriority() << ","
                    << task.getDueDate() << ","
                    << (task.isCompleted() ? "Completed" : "Pending") << "\n";
            }
            csv.close();
            cout << "Tasks exported to tasks.csv successfully!\n";
        }
        else if (choice == 8) {
            cout << "Exiting program. Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice! Try again.\n";
        }
    }

    return 0;
}