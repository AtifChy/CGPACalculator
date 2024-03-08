#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using namespace std;
using namespace std::chrono;
using json = nlohmann::json;

enum courseCredit {
    ONE_CREDIT = 1,
    TWO_CREDIT = 2,
    THREE_CREDIT = 3
};

unordered_map<string, float> gradeMap = {
        {"A+", 4.0},
        {"A",  3.75},
        {"B+", 3.5},
        {"B",  3.25},
        {"C+", 3.0},
        {"C",  2.75},
        {"D+", 2.5},
        {"D",  2.25},
        {"F",  0.0}
};

template<typename T>
void userInput(T &input) {
    while (!(cin >> input)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cerr << "Invalid input. Please enter a valid input: ";
    }
}

void validGrade(string &grade) {
    while (cin >> grade) {
        transform(grade.begin(), grade.end(), grade.begin(), ::toupper);
        if (gradeMap.find(grade) == gradeMap.end()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid grade entered. Please try again: ";
        } else {
            break;
        }
    }
}

void calculateCourseCreditsAndPoints(
        int &totalCredits,
        float &totalPoints,
        int courseCredit,
        int totalCourses
) {
    for (int i = 0; i < totalCourses; i++) {
        string grade;
        cout << "Enter grade for course " << i + 1 << ": ";
        validGrade(grade);
        totalCredits += courseCredit;
        totalPoints += gradeMap[grade] * (float) courseCredit;
    }
}

void getCourseInfoAndCalculate(
        int &totalCredits, float &totalPoints,
        int courseCredit
) {
    int courses;
    cout << "Number of " << courseCredit << " credit courses: ";
    userInput<int>(courses);
    calculateCourseCreditsAndPoints(totalCredits, totalPoints, courseCredit, courses);
}

time_t getTimeStamp() {
    auto now = system_clock::now();
    time_t now_c = system_clock::to_time_t(now);
    return now_c;
}

string formatTimeStamp(time_t timestamp) {
    tm *ltm = localtime(&timestamp);
    stringstream ss;
    ss << put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

json loadData(const string &fileName) {
    json data;
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        ofstream createFile(fileName);
        if (!createFile.is_open()) {
            cerr << "Error creating file." << endl;
            exit(1);
        }
        createFile.close();
        inputFile.open(fileName);
        if (!inputFile.is_open()) {
            cerr << "Error reading file." << endl;
            exit(1);
        }
    }
    if (inputFile.peek() == ifstream::traits_type::eof()) {
        data = json::array();
    } else {
        data = json::parse(inputFile);
    }
    inputFile.close();
    return data;
}

void saveData(const json &data, const string &fileName) {
    ofstream outFile(fileName);
    outFile << setw(4) << data << endl;
    outFile.close();
}

int main() {
    json data = loadData("data.json");

    float currentCGPA = 0;
    int creditFinished = 0;

    if (!data.empty()) {
        int selection;
        cout << "Previous entries:" << endl;
        int index = 1;
        for (const auto &entry: data) {
            time_t timestamp = entry["Timestamp"];
            cout << fixed
                 << setprecision(2)
                 << index++ << ". "
                 << formatTimeStamp(timestamp) << " - "
                 << "CGPA: " << (float) entry["CGPA"] << " - "
                 << "Credits: " << entry["Credits"]
                 << "\n";
        }
        cout << "0 to enter new entry" << endl;
        cout << "Select: ";
        userInput<int>(selection);

        if (selection > 0 && selection <= data.size()) {
            currentCGPA = data[selection - 1]["CGPA"];
            creditFinished = data[selection - 1]["Credits"];
        }
    }

    if (creditFinished == 0) {
        cout << "Credit finished: ";
        userInput<int>(creditFinished);

        if (creditFinished > 0) {
            cout << "Current CGPA: ";
            userInput<float>(currentCGPA);

            if (currentCGPA < 0 || currentCGPA > 4) {
                cerr << "Invalid CGPA entered. Please try again." << endl;
                return 1;
            }
        }
    }

    int totalCredits = creditFinished;
    float totalPoints = currentCGPA * (float) creditFinished;

    for (const courseCredit &credit: {THREE_CREDIT, TWO_CREDIT, ONE_CREDIT}) {
        getCourseInfoAndCalculate(totalCredits, totalPoints, credit);
    }

    float totalCGPA = totalPoints / (float) totalCredits;

    cout << fixed << setprecision(2) << "Your CGPA is: " << totalCGPA << endl;
    cout << "You have completed " << totalCredits << " credits." << endl;

    json newEntry = {
            {"CGPA",      totalCGPA},
            {"Credits",   totalCredits},
            {"Timestamp", getTimeStamp()}
    };
    data.push_back(newEntry);
    saveData(data, "data.json");

    return 0;
}


