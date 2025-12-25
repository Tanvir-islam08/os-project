#include <bits/stdc++.h>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int completion;
    int turnaround;
    int waiting;
};

/* Utility function */
void calculateTimes(vector<Process>& p) {
    for (auto &x : p) {
        x.turnaround = x.completion - x.arrival;
        x.waiting = x.turnaround - x.burst;
    }
}

void printResult(vector<Process> p, string title) {
    double avgWT = 0, avgTAT = 0;

    cout << "\n=== " << title << " ===\n";
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto x : p) {
        cout << x.pid << "\t"
             << x.arrival << "\t"
             << x.burst << "\t"
             << x.completion << "\t"
             << x.turnaround << "\t"
             << x.waiting << "\n";
        avgWT += x.waiting;
        avgTAT += x.turnaround;
    }

    cout << "Average Waiting Time: " << avgWT / p.size() << endl;
    cout << "Average Turnaround Time: " << avgTAT / p.size() << endl;
}

/* First Come First Serve (FCFS)*/
void FCFS(vector<Process> p) {
    sort(p.begin(), p.end(), [](auto a, auto b) {
        return a.arrival < b.arrival;
    });

    int time = 0;
    for (auto &x : p) {
        time = max(time, x.arrival);
        time += x.burst;
        x.completion = time;
    }

    calculateTimes(p);
    printResult(p, "First Come First Serve (FCFS)");
}

/* SPN (Non-preemptive SJF)  Shortest Process Next (SPN)*/
void SPN(vector<Process> p) {
    int n = p.size(), completed = 0, time = 0;
    vector<bool> done(n, false);

    while (completed < n) {
        int idx = -1;
        int minBurst = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival <= time && p[i].burst < minBurst) {
                minBurst = p[i].burst;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        time += p[idx].burst;
        p[idx].completion = time;
        done[idx] = true;
        completed++;
    }

    calculateTimes(p);
    printResult(p, "Shortest Process Next (SPN)");
}

/* SRT (Preemptive SJF) Shortest Remaining Time (SRT) */
void SRT(vector<Process> p) {
    int n = p.size(), completed = 0, time = 0;
    for (auto &x : p) x.remaining = x.burst;

    while (completed < n) {
        int idx = -1;
        int minRemain = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0 &&
                p[i].remaining < minRemain) {
                minRemain = p[i].remaining;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            p[idx].completion = time;
            completed++;
        }
    }

    calculateTimes(p);
    printResult(p, "Shortest Remaining Time (SRT)");
}

/* Round Robin with varying time quantum (RR)*/
void RoundRobin(vector<Process> p, int tq) {
    queue<int> q;
    int time = 0, completed = 0, n = p.size();
    vector<bool> visited(n, false);

    for (auto &x : p) x.remaining = x.burst;

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (!visited[i] && p[i].arrival <= time) {
                q.push(i);
                visited[i] = true;
            }
        }

        if (q.empty()) {
            time++;
            continue;
        }

        int idx = q.front();
        q.pop();

        int exec = min(tq, p[idx].remaining);
        p[idx].remaining -= exec;
        time += exec;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && p[i].arrival <= time) {
                q.push(i);
                visited[i] = true;
            }
        }

        if (p[idx].remaining > 0)
            q.push(idx);
        else {
            p[idx].completion = time;
            completed++;
        }
    }

    calculateTimes(p);
    printResult(p, "Round Robin (TQ = " + to_string(tq) + ")");
}

bool readFromFile(vector<Process>& processes) {
    ifstream file("input.txt");

    if (!file) {
        cout << "Error: input.txt not found!\n";
        return false;
    }

    int n;
    file >> n;

    if (n <= 0) {
        cout << "Invalid number of processes in file.\n";
        return false;
    }

    processes.resize(n);

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        file >> processes[i].arrival >> processes[i].burst;
    }

    file.close();
    return true;
}


int main() {
    vector<Process> processes;
    int choice;

    cout << "CPU Scheduling Algorithms Simulator\n";
    cout << "1. Use input from file\n";
    cout << "2. Enter input manually\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        if (!readFromFile(processes)) {
            return 0;
        }
    }
    else if (choice == 2) {
        int n;
        cout << "Enter number of processes: ";
        cin >> n;

        processes.resize(n);
        cout << "Enter Arrival Time and Burst Time:\n";

        for (int i = 0; i < n; i++) {
            processes[i].pid = i + 1;
            cin >> processes[i].arrival >> processes[i].burst;
        }
    }
    else {
        cout << "Invalid choice.\n";
        return 0;
    }

    FCFS(processes);
    SPN(processes);
    SRT(processes);

    int tq;
    cout << "\nEnter Time Quantum for Round Robin: ";
    cin >> tq;
    RoundRobin(processes, tq);

    return 0;
}
