#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <cctype>
using namespace std;

struct PrintingEvent {
    double printingHours = 0;
    double relativeHumidity = 0;
    double gramsUsed = 0;
    string dateTime;
    double exposure = 0;
};

struct Filament {
    string name;
    string type;
    double weight = 0;
    string dateOpened;
    vector<PrintingEvent> events;
};

// Date and Time Functiosn

string twoDigits(int number) {
    if (number < 10) {
        return "0" + to_string(number);
    }
    return to_string(number);
}
string getCurrentDateTime() {
    time_t now = time(0);
    tm* local = localtime(&now);

    if (local == nullptr) {
        return "Unknown date/time";
    }

    string dateTime = to_string(local->tm_year + 1900) + "-";
    dateTime += twoDigits(local->tm_mon + 1) + "-";
    dateTime += twoDigits(local->tm_mday) + " ";
    dateTime += twoDigits(local->tm_hour) + ":";
    dateTime += twoDigits(local->tm_min) + ":";
    dateTime += twoDigits(local->tm_sec);

    return dateTime;
}
string getCurrentDate() {
    time_t now = time(0);
    tm* local = localtime(&now);

    if (local == nullptr) {
        return "Unknown date";
    }

    string date = to_string(local->tm_year + 1900) + "-";
    date += twoDigits(local->tm_mon + 1) + "-";
    date += twoDigits(local->tm_mday);

    return date;
}

// Input Functions

string strInput(const string& message) {
    cout << message;
    string input;
    getline(cin, input);
    return input;
}

int intInput(const string& message) {
    int number;
    cout << message;
    while (!(cin >> number)) {
        cout << "Invalid input. Enter a whole number: ";

        cin.clear();
        cin.ignore(9999, '\n');
    }
    cin.ignore(99999, '\n');
    return number;
}

double doubleInput(const string& message) {
    double number;

    cout << message;

    while (!(cin >> number) || number < 0) {
        cout << "Invalid input. Enter a number that is zero or higher: ";

        cin.clear();
        cin.ignore(99999, '\n');
    }

    cin.ignore(99999, '\n');

    return number;
}

// Date Validation Functions
int readNumber(const string& text, int start, int length) {
    int number = 0;

    for (int i = start; i < start + length; i++) {
        if (i >= static_cast<int>(text.length())) {
            return -1;
        }

        if (!isdigit(text[i])) {
            return -1;
        }

        number = number * 10 + (text[i] - '0');
    }

    return number;
}

bool isLeapYear(int year) {
    if (year % 400 == 0) {
        return true;
    }

    if (year % 100 == 0) {
        return false;
    }

    return year % 4 == 0;
}

bool parseDate(const string& input, tm& result) {
    if (input.length() != 10) {
        return false;
    }

    if (input[4] != '-' || input[7] != '-') {
        return false;
    }

    int year = readNumber(input, 0, 4);
    int month = readNumber(input, 5, 2);
    int day = readNumber(input, 8, 2);

    if (year < 1900 || month < 1 || month > 12) {
        return false;
    }

    if (day < 1) {
        return false;
    }

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (isLeapYear(year)) {
        daysInMonth[1] = 29;
    }

    if (day > daysInMonth[month - 1]) {
        return false;
    }

    result = {};

    result.tm_year = year - 1900;
    result.tm_mon = month - 1;
    result.tm_mday = day;
    result.tm_hour = 0;
    result.tm_min = 0;
    result.tm_sec = 0;
    result.tm_isdst = -1;

    return true;
}

double daysSinceOpened(const string& dateOpened) {
    tm opened = {};

    if (!parseDate(dateOpened, opened)) {
        return -1;
    }

    time_t openedTime = mktime(&opened);
    time_t nowTime = time(0);

    if (openedTime == static_cast<time_t>(-1)) {
        return -1;
    }

    if (nowTime < openedTime) {
        return -1;
    }

    double seconds = difftime(nowTime, openedTime);

    return seconds / (60.0 * 60.0 * 24.0);
}

// Moisture-Risk Calculations

string toLowerString(string type) {
    for (size_t i = 0; i < type.length(); i++) {
        type[i] = tolower(static_cast<unsigned char>(type[i]));
    }
    return type;
}

double getRiskThresholdDays(const string& type) {
    string material = toLowerString(type);

    if (material.find("petg") != string::npos) {
        return 7.0;
    }
    if (material.find("tpu") != string::npos) {
        return 14.0;
    }
    if (material.find("pla") != string::npos) {
        return 30.0;
    }
    return 30.0;
}

double calculateExposure(double printingHours, double relativeHumidity) {
    return printingHours * (relativeHumidity / 100.0);
}

double totalExposure(const Filament& filament) {
    double total = 0;

    for (size_t i = 0; i < filament.events.size(); i++) {
        total += filament.events[i].exposure;
    }
    return total;
}

double moistureRiskPercent(const Filament& filament) {
    double daysOpen = daysSinceOpened(filament.dateOpened);
    if (daysOpen < 0) {
        return -1;
    }
    double exposureDays = totalExposure(filament) / 24.0;
    double estimatedDays = daysOpen + exposureDays;
    double threshold = getRiskThresholdDays(filament.type);
    double risk = (estimatedDays / threshold) * 100.0;

    if (risk < 0) {
        risk = 0;
    }

    if (risk > 100) {
        risk = 100;
    }

    return risk;
}

string riskLabel(double risk) {
    if (risk < 0) {
        return "Unable to Estimate, Verify Date";
    }
    if (risk >= 100) {
        return "HIGH: drying is recommended";
    }
    if (risk >= 70) {
        return "ELEVATED: May result in minor defects";
    }
    return "LOW: Safe for printing";
}

void displayRiskWarning(const Filament& filament) {
    double risk = moistureRiskPercent(filament);
    cout << "\nMoisture risk estimate: " << riskLabel(risk);

    if (risk >= 0) {
        cout << " (" << risk << "%)";
    }

    cout << '\n';

    if (risk >= 100) {
        cout << "WARNING: This filament has reached its estimated "
             << "moisture-risk threshold.\n";
        cout << "Consider drying it before your next print.\n";
    }
}

// File Storage Functions

bool saveData(const vector<Filament>& filaments) {
    ofstream file("filament_data.txt");

    if (!file.is_open()) {
        cout << "Error: Could not save the data.\n";
        return false;
    }

    file << filaments.size() << '\n';

    for (size_t i = 0; i < filaments.size(); i++) {
        const Filament& filament = filaments[i];

        file << filament.name << '\n';
        file << filament.type << '\n';
        file << filament.weight << '\n';
        file << filament.dateOpened << '\n';
        file << filament.events.size() << '\n';

        for (size_t j = 0; j < filament.events.size(); j++) {
            const PrintingEvent& event = filament.events[j];

            file << event.printingHours << '\n';
            file << event.relativeHumidity << '\n';
            file << event.gramsUsed << '\n';
            file << event.dateTime << '\n';
            file << event.exposure << '\n';
        }
    }

    if (!file) {
        cout << "Error: A problem occurred while saving the data.\n";
        return false;
    }

    return true;
}

bool loadData(vector<Filament>& filaments) {
    ifstream file("filament_data.txt");

    if (!file.is_open()) {
        ofstream newFile("filament_data.txt");

        if (!newFile.is_open()) {
            cout << "Error: Could not create the data file\n";
            return false;
        }

        newFile << 0 << '\n';
        cout << "New inventory file created.\n";
        return true;
    }

    int filamentCount = 0;

    if (!(file >> filamentCount) || filamentCount < 0) {
        cout << "Error: The inventory file is damaged or invalid.\n";
        return false;
    }
    file.ignore(10000, '\n');

    for (int i = 0; i < filamentCount; i++) {
        Filament filament;
        int eventCount = 0;

        if (!getline(file, filament.name)) {
            cout << "Error: Could not read filament name.\n";
            return false;
        }
        if (!getline(file, filament.type)) {
            cout << "Error: Could not read filament type.\n";
            return false;
        }
        if (!(file >> filament.weight) || filament.weight < 0) {
            cout << "Error: Invalid filament weight in data file.\n";
            return false;
        }
        file.ignore(10000, '\n');
        if (!getline(file, filament.dateOpened)) {
            cout << "Error: Could not read filament opening date.\n";
            return false;
        }
        if (!(file >> eventCount) || eventCount < 0) {
            cout << "Error: Invalid printing event count.\n";
            return false;
        }
        file.ignore(10000, '\n');
        for (int j = 0; j < eventCount; j++) {
            PrintingEvent event;

            if (!(file >> event.printingHours) ||
                event.printingHours < 0) {
                cout << "Error: Invalid printing hours in data file.\n";
                return false;
            }
            file.ignore(10000, '\n');
            if (!(file >> event.relativeHumidity) ||
                event.relativeHumidity < 0 ||
                event.relativeHumidity > 100) {
                cout << "Error: Invalid humidity in data file.\n";
                return false;
            }
            file.ignore(10000, '\n');

            if (!(file >> event.gramsUsed) || event.gramsUsed < 0) {
                cout << "Error: Invalid grams used in data file.\n";
                return false;
            }
            file.ignore(10000, '\n');

            if (!getline(file, event.dateTime)) {
                cout << "Error: Could not read event date/time.\n";
                return false;
            }
            if (!(file >> event.exposure) || event.exposure < 0) {
                cout << "Error: Invalid exposure score in data file.\n";
                return false;
            }
            file.ignore(10000, '\n');

            filament.events.push_back(event);
        }

        filaments.push_back(filament);
    }

    cout << "Inventory loaded successfully.\n";

    return true;
}

// Inventory functions

bool displayFilaments(const vector<Filament>& filaments) {
    if (filaments.empty()) {
        cout << "\nNo filaments in the inventory.\n";
        return false;
    }

    cout << "\n----> FILAMENT INVENTORY\n";

    for (size_t i = 0; i < filaments.size(); i++) {
        const Filament& filament = filaments[i];

        double risk = moistureRiskPercent(filament);

        cout << "\n" << i + 1 << ". " << filament.name << '\n';
        cout << "   Type: " << filament.type << '\n';
        cout << "   Remaining weight: " << filament.weight << " g\n";
        cout << "   Date opened: " << filament.dateOpened << '\n';
        cout << "   Moisture risk: " << riskLabel(risk);

        if (risk >= 0) {
            cout << " (" << risk << "%)";
        }

        cout << '\n';

        if (risk >= 100) {
            cout << "   WARNING: Drying is recommended\n";
        }
    }

    return true;
}

bool selectFilament(const vector<Filament>& filaments, int& index) {
    if (!displayFilaments(filaments)) {
        return false;
    }

    int choice = intInput("\nEnter the filament number: ");

    if (choice < 1 || choice > static_cast<int>(filaments.size())) {
        cout << "Invalid selection.\n";
        return false;
    }

    index = choice - 1;
    return true;
}

void displayFilamentDetails(const vector<Filament>& filaments) {
    int index;

    if (!selectFilament(filaments, index)) {
        return;
    }
    const Filament& filament = filaments[index];

    cout << "\n----> FILAMENT DETAILS\n";
    cout << "Name: " << filament.name << '\n';
    cout << "Type: " << filament.type << '\n';
    cout << "Remaining weight: " << filament.weight << " g\n";
    cout << "Date opened: " << filament.dateOpened << '\n';
    cout << "Printing events: " << filament.events.size() << '\n';
    cout << "Total humidity-weighted exposure: "
         << totalExposure(filament) << '\n';
    cout << "Estimated risk interval: "
         << getRiskThresholdDays(filament.type) << " days\n";

    displayRiskWarning(filament);

    for (size_t i = 0; i < filament.events.size(); i++) {
        const PrintingEvent& event = filament.events[i];

        cout << "\n--- Printing Event " << i + 1 << " ---\n";
        cout << "Date/time: " << event.dateTime << '\n';
        cout << "Printing hours: " << event.printingHours << '\n';
        cout << "Relative humidity: "
             << event.relativeHumidity << "%\n";
        cout << "Grams used: " << event.gramsUsed << " g\n";
        cout << "Exposure score: " << event.exposure << '\n';
    }
}

void addFilament(vector<Filament>& filaments) {
    Filament filament;

    cout << "\n----> ADD FILAMENT \n";

    filament.name = strInput("Enter filament name/brand: ");

    if (filament.name.empty()) {
        cout << "Name cannot be empty.\n";
        return;
    }

    cout << "Supported types: PLA, PETG, TPU\n";

    filament.type = strInput("Enter filament type: ");

    if (filament.type.empty()) {
        cout << "Type cannot be empty.\n";
        return;
    }

    filament.weight = doubleInput("Enter current filament weight in grams: ");

    cout << "Current date: " << getCurrentDate() << '\n';

    filament.dateOpened = strInput(
        "Enter date opened (YYYY-MM-DD), or press ENTER for today: "
    );

    if (filament.dateOpened.empty()) {
        filament.dateOpened = getCurrentDate();
    } else {
        tm test = {};

        if (!parseDate(filament.dateOpened, test)) {
            cout << "Invalid date. Use YYYY-MM-DD and enter a valid date\n";
            cout << "Filament was not added\n";
            return;
        }

        time_t openedTime = mktime(&test);
        time_t nowTime = time(0);

        if (openedTime == static_cast<time_t>(-1) ||
            openedTime > nowTime) {
            cout << "Opening date cannot be in the future\n";
            cout << "Filament was not added.\n";
            return;
        }
    }

    filaments.push_back(filament);

    if (saveData(filaments)) {
        cout << "Filament added and saved successfully\n";
    }
}

void removeFilament(vector<Filament>& filaments) {
    int index;

    if (!selectFilament(filaments, index)) {
        return;
    }

    double amount = doubleInput(
        "Enter grams to remove from inventory: "
    );

    if (amount > filaments[index].weight) {
        cout << "You cannot remove more than the remaining weight\n";
        return;
    }

    filaments[index].weight -= amount;

    if (filaments[index].weight <= 0.000001) {
        cout << "Filament fully used; removing it from inventory\n";
        filaments.erase(filaments.begin() + index);
    } else {
        cout << "Remaining weight: "
             << filaments[index].weight << " g\n";
    }

    if (saveData(filaments)) {
        cout << "Inventory saved\n";
    }
}

void addPrintingEvent(vector<Filament>& filaments) {
    int index;

    if (!selectFilament(filaments, index)) {
        return;
    }

    PrintingEvent event;

    cout << "\n----> ADD PRINTING JOB \n";

    event.printingHours = doubleInput(
        "Enter printing/exposure hours: "
    );

    while (true) {
        event.relativeHumidity = doubleInput(
            "Enter relative humidity (0-100%): "
        );

        if (event.relativeHumidity <= 100) {
            break;
        }

        cout << "Relative humidity must be between 0 and 100.\n";
    }

    event.gramsUsed = doubleInput("Enter grams used: ");

    if (event.gramsUsed > filaments[index].weight) {
        cout << "Grams used cannot exceed remaining filament weight\nInput Cancelled\n";
        return;
    }

    event.dateTime = strInput(
        "Enter event date/time, or press ENTER for now: "
    );

    if (event.dateTime.empty()) {
        event.dateTime = getCurrentDateTime();
    }

    event.exposure = calculateExposure(
        event.printingHours,
        event.relativeHumidity
    );

    filaments[index].events.push_back(event);
    filaments[index].weight -= event.gramsUsed;

    cout << "\nEvent recorded successfully.\n";
    cout << "Exposure prediction: " << event.exposure << '\n';
    cout << "Remaining weight: " << filaments[index].weight << " g\n";

    displayRiskWarning(filaments[index]);

    if (filaments[index].weight <= 0.000001) {
        cout << "Filament fully used\n will be automatically removed\n";
        filaments.erase(filaments.begin() + index);
    }
    if (saveData(filaments)) {
        cout << "Printing event and inventory saved\n";
    }
}

void displayPrintingHistory(const vector<Filament>& filaments) {
    bool found = false;

    cout << "\n===== PRINTING HISTORY =====\n";

    for (size_t i = 0; i < filaments.size(); i++) {
        const Filament& filament = filaments[i];

        if (filament.events.empty()) {
            continue;
        }

        found = true;

        cout << "\nFilament: " << filament.name
             << " (" << filament.type << ")\n";

        for (size_t j = 0; j < filament.events.size(); j++) {
            const PrintingEvent& event = filament.events[j];

            cout << "\nEvent " << j + 1 << '\n';
            cout << "Date/time: " << event.dateTime << '\n';
            cout << "Printing hours: " << event.printingHours << '\n';
            cout << "Relative humidity: "
                 << event.relativeHumidity << "%\n";
            cout << "Grams used: " << event.gramsUsed << " g\n";
            cout << "Exposure score: " << event.exposure << '\n';
        }
    }

    if (!found) {
        cout << "No printing events have been recorded\n";
    }
}

//menu part

void showMenu() {
    cout << "\n===== 3D FILAMENT MOISTURE TRACKER =====\n";
    cout << "1. Add filament\n";
    cout << "2. Remove grams or used filament\n";
    cout << "3. Add printing event\n";
    cout << "4. Display inventory and moisture risk estimates\n";
    cout << "5. Display filament details\n";
    cout << "6. Display printing history\n";
    cout << "7. Save and finish\n";
}

// Main Program

int main() {
    vector<Filament> filaments;

    cout << "Current date/time: " << getCurrentDateTime() << '\n';

    if (!loadData(filaments)) {
        cerr << "Unable to load inventory\n";
        return 1;
    }

    if (!filaments.empty()) {
        cout << "\n===== MOISTURE-RISK CHECK =====\n";

        for (size_t i = 0; i < filaments.size(); i++) {
            cout << "\n" << filaments[i].name
                 << " (" << filaments[i].type << ")\n";

            displayRiskWarning(filaments[i]);
        }
    }

    int choice = 0;

    while (choice != 7) {
        showMenu();

        choice = intInput("Enter your choice: ");

        switch (choice) {
            case 1:
                addFilament(filaments);
                break;

            case 2:
                removeFilament(filaments);
                break;

            case 3:
                addPrintingEvent(filaments);
                break;

            case 4:
                displayFilaments(filaments);
                break;

            case 5:
                displayFilamentDetails(filaments);
                break;

            case 6:
                displayPrintingHistory(filaments);
                break;

            case 7:
                if (saveData(filaments)) {
                    cout << "Inventory saved.\n";
                }

                cout << "Thank you for using the tracker.\n";
                break;

            default:
                cout << "Invalid choice. Please select 1-7.\n";
        }
    }

    return 0;
}
