// COLLEGE EVENT MANAGEMENT SYSTEM (C++, OOP) - Dev-C++ 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <limits>
using namespace std;

// small helpers to replace C++11 stoi/to_string
int toInt(string s) { return atoi(s.c_str()); }
string toStr(int n) { stringstream ss; ss << n; return ss.str(); }

class Person { // abstract base -> Inheritance/Abstraction
protected:
    int id; string name;
public:
    Person() { id = 0; name = ""; }
    Person(int id_, string name_) { id = id_; name = name_; }
    virtual void display() const = 0; // Polymorphism
    int getId() const { return id; }
    string getName() const { return name; }
    virtual ~Person() {}
};

class Student : public Person {
    string department;
    vector<int> registeredEvents;
public:
    Student() {}
    Student(int id_, string name_, string dept) : Person(id_, name_) { department = dept; }
    void display() const {
        cout << left << setw(8) << id << setw(20) << name << setw(15) << department
             << "Events: " << registeredEvents.size() << endl;
    }
    void registerForEvent(int e) { registeredEvents.push_back(e); }
    bool isRegisteredFor(int e) const {
        return find(registeredEvents.begin(), registeredEvents.end(), e) != registeredEvents.end();
    }
    string serialize() const {
        stringstream ss; ss << id << "|" << name << "|" << department << "|";
        for (size_t i = 0; i < registeredEvents.size(); i++) {
            ss << registeredEvents[i];
            if (i + 1 < registeredEvents.size()) ss << ",";
        }
        return ss.str();
    }
};

class Coordinator : public Person {
    string department;
public:
    Coordinator() {}
    Coordinator(int id_, string name_, string dept) : Person(id_, name_) { department = dept; }
    void display() const { cout << left << setw(8) << id << setw(20) << name << department << endl; }
    string serialize() const { return toStr(id) + "|" + name + "|" + department; }
};

class Venue { // Encapsulation
    int venueId, capacity; string venueName; bool allocated;
public:
    Venue() { venueId = 0; capacity = 0; allocated = false; }
    Venue(int id_, string name_, int cap) { venueId = id_; venueName = name_; capacity = cap; allocated = false; }
    int getId() const { return venueId; }
    string getName() const { return venueName; }
    bool isAllocated() const { return allocated; }
    void allocate() { allocated = true; }
    void display() const {
        cout << left << setw(8) << venueId << setw(20) << venueName << setw(10) << capacity
             << (allocated ? "Allocated" : "Available") << endl;
    }
    string serialize() const { return toStr(venueId) + "|" + venueName + "|" + toStr(capacity) + "|" + toStr(allocated); }
};

class Event {
    int eventId, venueId, maxParticipants;
    string eventName, eventDate, coordinatorName;
    vector<int> registeredStudents, presentStudents;
public:
    Event() { eventId = 0; venueId = -1; maxParticipants = 0; }
    Event(int id_, string name_, string date, string coord, int maxP) {
        eventId = id_; eventName = name_; eventDate = date; coordinatorName = coord;
        maxParticipants = maxP; venueId = -1;
    }
    int getId() const { return eventId; }
    string getName() const { return eventName; }
    string getDate() const { return eventDate; }
    string getCoordinatorName() const { return coordinatorName; }
    void setVenueId(int v) { venueId = v; }
    int getMaxParticipants() const { return maxParticipants; }
    int getRegisteredCount() const { return (int)registeredStudents.size(); }
    vector<int> getRegisteredStudents() const { return registeredStudents; }
    vector<int> getPresentStudents() const { return presentStudents; }

    bool addRegistration(int sid) {
        if ((int)registeredStudents.size() >= maxParticipants) return false;
        if (find(registeredStudents.begin(), registeredStudents.end(), sid) != registeredStudents.end()) return false;
        registeredStudents.push_back(sid);
        return true;
    }
    bool markPresent(int sid) {
        if (find(registeredStudents.begin(), registeredStudents.end(), sid) == registeredStudents.end()) return false;
        if (find(presentStudents.begin(), presentStudents.end(), sid) != presentStudents.end()) return false;
        presentStudents.push_back(sid);
        return true;
    }
    void display() const {
        string vidStr = (venueId == -1) ? string("--") : toStr(venueId);
        cout << left << setw(6) << eventId << setw(20) << eventName << setw(12) << eventDate << setw(18) << coordinatorName
             << setw(10) << vidStr << registeredStudents.size() << "/" << maxParticipants << endl;
    }
    string serialize() const {
        stringstream ss;
        ss << eventId << "|" << eventName << "|" << eventDate << "|" << coordinatorName << "|" << venueId << "|" << maxParticipants << "|";
        for (size_t i = 0; i < registeredStudents.size(); i++) {
            ss << registeredStudents[i];
            if (i + 1 < registeredStudents.size()) ss << ",";
        }
        ss << "|";
        for (size_t i = 0; i < presentStudents.size(); i++) {
            ss << presentStudents[i];
            if (i + 1 < presentStudents.size()) ss << ",";
        }
        return ss.str();
    }
};

vector<string> splitStr(const string &s, char d) {
    vector<string> t; stringstream ss(s); string item;
    while (getline(ss, item, d)) t.push_back(item);
    if (!s.empty() && s[s.size() - 1] == d) t.push_back("");
    return t;
}

class EventManagementSystem {
    vector<Student> students; vector<Coordinator> coordinators; vector<Venue> venues; vector<Event> events;
    int nextStudentId, nextCoordinatorId, nextVenueId, nextEventId;

    Student* findStudent(int id) {
        for (size_t i = 0; i < students.size(); i++) if (students[i].getId() == id) return &students[i];
        return NULL;
    }
    Event* findEvent(int id) {
        for (size_t i = 0; i < events.size(); i++) if (events[i].getId() == id) return &events[i];
        return NULL;
    }
    Venue* findVenue(int id) {
        for (size_t i = 0; i < venues.size(); i++) if (venues[i].getId() == id) return &venues[i];
        return NULL;
    }

public:
    EventManagementSystem() { nextStudentId = 1; nextCoordinatorId = 1; nextVenueId = 1; nextEventId = 1; }

    void addStudent() {
        string name, dept;
        cout << "Enter student name: "; cin.ignore(); getline(cin, name);
        cout << "Enter department: "; getline(cin, dept);
        students.push_back(Student(nextStudentId, name, dept));
        cout << "Student added with ID: " << nextStudentId << "\n";
        nextStudentId++;
    }
    void addCoordinator() {
        string name, dept;
        cout << "Enter coordinator name: "; cin.ignore(); getline(cin, name);
        cout << "Enter department: "; getline(cin, dept);
        coordinators.push_back(Coordinator(nextCoordinatorId, name, dept));
        cout << "Coordinator added with ID: " << nextCoordinatorId << "\n";
        nextCoordinatorId++;
    }
    void displayStudents() const {
        if (students.empty()) { cout << "No students yet.\n"; return; }
        for (size_t i = 0; i < students.size(); i++) students[i].display();
    }
    void addVenue() {
        string name; int cap;
        cout << "Enter venue name: "; cin.ignore(); getline(cin, name);
        cout << "Enter capacity: "; cin >> cap;
        venues.push_back(Venue(nextVenueId, name, cap));
        cout << "Venue added with ID: " << nextVenueId << "\n";
        nextVenueId++;
    }
    void displayVenues() const {
        if (venues.empty()) { cout << "No venues yet.\n"; return; }
        for (size_t i = 0; i < venues.size(); i++) venues[i].display();
    }
    void addEvent() {
        string name, date, coord; int maxP;
        cout << "Enter event name: "; cin.ignore(); getline(cin, name);
        cout << "Enter date (DD-MM-YYYY): "; getline(cin, date);
        cout << "Enter coordinator name: "; getline(cin, coord);
        cout << "Enter max participants: "; cin >> maxP;
        events.push_back(Event(nextEventId, name, date, coord, maxP));
        cout << "Event created with ID: " << nextEventId << "\n";
        nextEventId++;
    }
    void displayEvents() const {
        if (events.empty()) { cout << "No events yet.\n"; return; }
        for (size_t i = 0; i < events.size(); i++) events[i].display();
    }
    void allocateVenueToEvent() {
        if (events.empty() || venues.empty()) { cout << "Need an event and a venue.\n"; return; }
        int eid, vid;
        displayEvents();
        cout << "Enter Event ID: "; cin >> eid;
        Event *e = findEvent(eid);
        if (!e) { cout << "Event not found.\n"; return; }
        displayVenues();
        cout << "Enter Venue ID: "; cin >> vid;
        Venue *v = findVenue(vid);
        if (!v) { cout << "Venue not found.\n"; return; }
        if (v->isAllocated()) { cout << "Venue already allocated.\n"; return; }
        v->allocate();
        e->setVenueId(vid);
        cout << "Venue allocated to event.\n";
    }
    void registerStudentForEvent() {
        if (students.empty() || events.empty()) { cout << "Need a student and an event.\n"; return; }
        int sid, eid;
        displayStudents();
        cout << "Enter Student ID: "; cin >> sid;
        Student *s = findStudent(sid);
        if (!s) { cout << "Student not found.\n"; return; }
        displayEvents();
        cout << "Enter Event ID: "; cin >> eid;
        Event *e = findEvent(eid);
        if (!e) { cout << "Event not found.\n"; return; }
        if (s->isRegisteredFor(eid)) { cout << "Already registered.\n"; return; }
        if (e->addRegistration(sid)) { s->registerForEvent(eid); cout << "Registered successfully.\n"; }
        else cout << "Registration failed: event full.\n";
    }
    void markAttendance() {
        if (events.empty()) { cout << "No events.\n"; return; }
        int eid, sid;
        displayEvents();
        cout << "Enter Event ID: "; cin >> eid;
        Event *e = findEvent(eid);
        if (!e) { cout << "Event not found.\n"; return; }
        cout << "Enter Student ID to mark present: "; cin >> sid;
        if (e->markPresent(sid)) cout << "Attendance marked.\n";
        else cout << "Failed: not registered or already marked.\n";
    }
    void generateEventReport() const {
        if (events.empty()) { cout << "No events.\n"; return; }
        int eid;
        displayEvents();
        cout << "Enter Event ID for report: "; cin >> eid;
        const Event *e = NULL;
        for (size_t i = 0; i < events.size(); i++) if (events[i].getId() == eid) { e = &events[i]; break; }
        if (!e) { cout << "Event not found.\n"; return; }
        cout << "\n== REPORT: " << e->getName() << " ==\n";
        cout << "Date: " << e->getDate() << " | Coordinator: " << e->getCoordinatorName() << "\n";
        cout << "Registered: " << e->getRegisteredCount() << "/" << e->getMaxParticipants() << "\n";
        vector<int> reg = e->getRegisteredStudents();
        vector<int> pres = e->getPresentStudents();
        for (size_t i = 0; i < reg.size(); i++) {
            for (size_t j = 0; j < students.size(); j++) {
                if (students[j].getId() == reg[i]) {
                    bool present = find(pres.begin(), pres.end(), reg[i]) != pres.end();
                    cout << students[j].getId() << " " << students[j].getName() << " - " << (present ? "Present" : "Absent") << "\n";
                }
            }
        }
    }
    void saveData() const {
        ofstream fs("students.txt");
        for (size_t i = 0; i < students.size(); i++) fs << students[i].serialize() << "\n";
        ofstream fc("coordinators.txt");
        for (size_t i = 0; i < coordinators.size(); i++) fc << coordinators[i].serialize() << "\n";
        ofstream fv("venues.txt");
        for (size_t i = 0; i < venues.size(); i++) fv << venues[i].serialize() << "\n";
        ofstream fe("events.txt");
        for (size_t i = 0; i < events.size(); i++) fe << events[i].serialize() << "\n";
        cout << "Data saved.\n";
    }
    void loadData() {
        string line;
        ifstream fs("students.txt");
        while (getline(fs, line)) {
            if (line.empty()) continue;
            vector<string> t = splitStr(line, '|');
            if (t.size() < 3) continue; // skip malformed/incompatible line
            Student s(toInt(t[0]), t[1], t[2]);
            if (t.size() > 3) {
                vector<string> ev = splitStr(t[3], ',');
                for (size_t i = 0; i < ev.size(); i++) if (!ev[i].empty()) s.registerForEvent(toInt(ev[i]));
            }
            students.push_back(s);
            if (s.getId() + 1 > nextStudentId) nextStudentId = s.getId() + 1;
        }
        ifstream fc("coordinators.txt");
        while (getline(fc, line)) {
            if (line.empty()) continue;
            vector<string> t = splitStr(line, '|');
            if (t.size() < 3) continue;
            Coordinator c(toInt(t[0]), t[1], t[2]);
            coordinators.push_back(c);
            if (c.getId() + 1 > nextCoordinatorId) nextCoordinatorId = c.getId() + 1;
        }
        ifstream fv("venues.txt");
        while (getline(fv, line)) {
            if (line.empty()) continue;
            vector<string> t = splitStr(line, '|');
            if (t.size() < 3) continue;
            Venue v(toInt(t[0]), t[1], toInt(t[2]));
            if (t.size() > 3 && t[3] == "1") v.allocate();
            venues.push_back(v);
            if (v.getId() + 1 > nextVenueId) nextVenueId = v.getId() + 1;
        }
        ifstream fe("events.txt");
        while (getline(fe, line)) {
            if (line.empty()) continue;
            vector<string> t = splitStr(line, '|');
            if (t.size() < 6) continue; // needs at least id|name|date|coord|venueId|maxP
            Event e(toInt(t[0]), t[1], t[2], t[3], toInt(t[5]));
            e.setVenueId(toInt(t[4]));
            if (t.size() > 6) {
                vector<string> reg = splitStr(t[6], ',');
                for (size_t i = 0; i < reg.size(); i++) if (!reg[i].empty()) e.addRegistration(toInt(reg[i]));
            }
            if (t.size() > 7) {
                vector<string> pres = splitStr(t[7], ',');
                for (size_t i = 0; i < pres.size(); i++) if (!pres[i].empty()) e.markPresent(toInt(pres[i]));
            }
            events.push_back(e);
            if (e.getId() + 1 > nextEventId) nextEventId = e.getId() + 1;
        }
    }
};

void printMenu() {
    cout << "\n== COLLEGE EVENT MANAGEMENT SYSTEM ==\n"
         << "1.Add Student 2.Add Coordinator 3.Add Venue 4.Create Event\n"
         << "5.Allocate Venue 6.Register Student 7.Mark Attendance\n"
         << "8.View Students 9.View Venues 10.View Events\n"
         << "11.Event Report 12.Save Data 13.Exit\nChoice: ";
}

int main() {
    EventManagementSystem ems;
    ems.loadData();
    int choice;
    do {
        printMenu();
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            continue;
        }
        switch (choice) {
            case 1: ems.addStudent(); break;
            case 2: ems.addCoordinator(); break;
            case 3: ems.addVenue(); break;
            case 4: ems.addEvent(); break;
            case 5: ems.allocateVenueToEvent(); break;
            case 6: ems.registerStudentForEvent(); break;
            case 7: ems.markAttendance(); break;
            case 8: ems.displayStudents(); break;
            case 9: ems.displayVenues(); break;
            case 10: ems.displayEvents(); break;
            case 11: ems.generateEventReport(); break;
            case 12: ems.saveData(); break;
            case 13: ems.saveData(); cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 13);
    return 0;
}
