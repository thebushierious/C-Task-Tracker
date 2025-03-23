#include <iostream>
#include <conio.h>
#include <map>
#include <string>
#include <limits>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

class Progress
{
    public:
        string class_progress;
};

map<int, string> taskList;
map<string, Progress> progressMap;
int taskIndex = 0;
const string fileName = "tasks.json";

void saveTasksToFile()
{
    ofstream file(fileName);
    if (file.is_open())
    {
        file << "{\n";
        file << "  \"taskIndex\": " << taskIndex << ",\n";
        file << "  \"taskList\": {\n";
        for (auto it = taskList.begin(); it != taskList.end(); ++it)
        {
            file << "    \"" << it->first << "\": \"" << it->second << "\"";
            if (next(it) != taskList.end()) file << ",";
            file << "\n";
        }
        file << "  },\n";
        file << "  \"progressMap\": {\n";
        for (auto it = progressMap.begin(); it != progressMap.end(); ++it)
        {
            file << "    \"" << it->first << "\": \"" << it->second.class_progress << "\"";
            if (next(it) != progressMap.end()) file << ",";
            file << "\n";
        }
        file << "  }\n";
        file << "}\n";
        file.close();
    }
}

void loadTasksFromFile()
{
    ifstream file(fileName);
    if (file.is_open())
    {
        stringstream buffer;
        buffer << file.rdbuf();
        string content = buffer.str();
        file.close();

        size_t pos = content.find("\"taskIndex\": ");
        if (pos != string::npos)
        {
            taskIndex = stoi(content.substr(pos + 13));
        }

        pos = content.find("\"taskList\": {");
        if (pos != string::npos)
        {
            size_t endPos = content.find("}", pos);
            string taskListStr = content.substr(pos + 13, endPos - pos - 13);
            stringstream ss(taskListStr);
            string line;
            while (getline(ss, line, ','))
            {
                size_t colonPos = line.find(": ");
                int index = stoi(line.substr(1, colonPos - 2));
                string task = line.substr(colonPos + 3, line.length() - colonPos - 4);
                taskList[index] = task;
            }
        }

        pos = content.find("\"progressMap\": {");
        if (pos != string::npos)
        {
            size_t endPos = content.find("}", pos);
            string progressMapStr = content.substr(pos + 16, endPos - pos - 16);
            stringstream ss(progressMapStr);
            string line;
            while (getline(ss, line, ','))
            {
                size_t colonPos = line.find(": ");
                string task = line.substr(1, colonPos - 2);
                string progress = line.substr(colonPos + 3, line.length() - colonPos - 4);
                Progress p;
                p.class_progress = progress;
                progressMap[task] = p;
            }
        }
    }
}

void displayTasks()
{
    cout << "---------------------------------------------" << endl;
    cout << "| Index | Task           | Progress         |" << endl;
    cout << "---------------------------------------------" << endl;
    for (auto display : taskList) 
    {
        cout << "| " << setw(5) << display.first << " | " << setw(14) << display.second;
        if (progressMap.find(display.second) != progressMap.end()) {
            cout << " | " << setw(15) << progressMap.at(display.second).class_progress << " |" << endl;
        } else {
            cout << " | " << setw(15) << "Not Set" << " |" << endl;
        }
    }
    cout << "---------------------------------------------" << endl;
}

void addTask()
{
    cout << "\nADD TASKS" << endl;
    int input, x = 0;
    x++;
    string task, progress = "Not Set", name;
    cout << "Enter task, enter /q to quit: ";
    getline(cin >> ws, task);
    if (task == "/q")
    {
        return;
    }

    cout << "Choose progress: " << endl;
    name = task + to_string(x);

    while(1)
    {
        cout << "(1) Done" << endl;
        cout << "(2) Not Done" << endl;
        cout << "(3) In Progress" << endl;

        if (!(cin >> input)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input, try again" << endl;
            continue;
        }
        switch(input)
        {
            case 1:
                progress = "Done";
                break;
            case 2:
                progress = "Not Done";
                break;
            case 3:
                progress = "In Progress";
                break;
            default:
                cout << "Invalid input, try again" << endl;
                continue;
        }
        break;
    }

    taskIndex++;
    taskList.insert({taskIndex, task});

    Progress p;
    p.class_progress = progress;
    progressMap.insert({task, p});
    cout << "Task added successfully!\n" << endl;
    saveTasksToFile();
}

void editTask()
{
    if (taskList.empty()) {
        cout << "No tasks to edit." << endl;
        return;
    }

    cout << "\nEDIT TASKS" << endl;
    string task;
    int index;
    displayTasks();

    cout << "Enter the index of the task you want to edit, enter -1 to quit: ";
    cin >> index;

    if (index == -1)
    {
        return;
    }

    if (taskList.find(index) != taskList.end()) {
        cout << "Enter new task: ";
        cin >> task;
        taskList[index] = task;
        cout << "Task edited successfully!\n" << endl;
        saveTasksToFile();
    } else {
        cout << "Task index not found." << endl;
    }
}

void deleteTask()
{
    if (taskList.empty()) {
        cout << "No tasks to delete." << endl;
        return;
    }

    cout << "\nDELETE TASKS" << endl;
    int index;
    displayTasks();

    cout << "Enter the index of the task you want to delete, enter -1 to quit: ";
    cin >> index;

    if (index == -1)
    {
        return;
    }

    if (taskList.find(index) != taskList.end()) {
        taskList.erase(index);

        map<int, string> tempList;
        int newIndex = 1;
        for (auto& task : taskList)
        {
            tempList[newIndex++] = task.second;
        }
        taskList = tempList;

        if (taskList.empty()) {
            taskIndex = 0; // Reset taskIndex if all tasks are deleted
        } else {
            taskIndex = newIndex - 1; // Update taskIndex to the last valid index
        }

        cout << "Task deleted successfully!\n" << endl;
        saveTasksToFile();
    } else {
        cout << "Task index not found." << endl;
    }
}

void editProgress()
{
    if (taskList.empty()) {
        cout << "No tasks to edit progress." << endl;
        return;
    }

    cout << "\nEDIT PROGRESS" << endl;
    int index;
    displayTasks();

    cout << "Enter the index of the task you want to edit progress, enter -1 to quit: ";
    cin >> index;

    if (index == -1)
    {
        return;
    }

    if (taskList.find(index) != taskList.end()) {
        int input;
        string task = taskList[index];
        cout << "Choose new progress: " << endl;

        while (1)
        {
            cout << "(1) Done" << endl;
            cout << "(2) Not Done" << endl;
            cout << "(3) In Progress" << endl;

            if (!(cin >> input)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input, try again" << endl;
                continue;
            }
            switch (input)
            {
                case 1:
                    progressMap[task].class_progress = "Done";
                    break;
                case 2:
                    progressMap[task].class_progress = "Not Done";
                    break;
                case 3:
                    progressMap[task].class_progress = "In Progress";
                    break;
                default:
                    cout << "Invalid input, try again" << endl;
                    continue;
            }
            cout << "Progress updated successfully!\n" << endl;
            saveTasksToFile();
            break;
        }
    } else {
        cout << "Task index not found." << endl;
    }
}

void Menu() 
{
    int input;
    while (true)
    {
        cout << "\nMENU" << endl;
        cout << "---------------------------------" << endl;
        cout << "(1) Add Task" << endl;
        cout << "(2) Edit Task" << endl;
        cout << "(3) Delete Task" << endl;
        cout << "(4) Edit Progress" << endl;
        cout << "(5) List All Tasks" << endl;
        cout << "(6) Exit" << endl;
        cout << "---------------------------------" << endl;
        cout << "Enter your choice: ";

            if (!(cin >> input)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input, try again" << endl;
                continue;
            }
            switch (input)
            {
                case 1:
                    addTask();
                    break;
                case 2:
                    editTask();
                    break;
                case 3:
                    deleteTask();
                    break;
                case 4:
                    editProgress();
                    break;
                case 5:
                    displayTasks();
                    _getch();
                    break;
                case 6:
                    return;
                default:
                    cout << "Invalid input, try again" << endl;
                    break;
            }        
    }
}

int main(int argc, char* argv[])
{
    loadTasksFromFile();
    Menu();
    return 0;
}
