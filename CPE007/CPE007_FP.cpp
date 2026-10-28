#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

// Data Structures

struct printingEvent {
    double printingHours;
    double relativeHumidity;
    double gramsUsed;
    string dateTime;
    double exposure;
};

struct Filament {
    string name;
    string type;
    double weight;
    string dateOpened;

    vector<printingEvent> events;
};

// Date and Time

string getCurrentDateTime() {
    time_t now = time(0);
    string dateTime = ctime(&now);

    if (dateTime.length() > 0) {
        dateTime.erase(dateTime.length() - 1);
    }

    return dateTime;
}

// Auto Input Functions

string strInput() {
    string input;
    getline(cin, input);
    return input;
}

int intInput(string message) {
    int number;

    cout << message;

    while (!(cin >> number)) {
        cout << "Invalid input. Enter a whole number: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    cin.ignore(10000, '\n');
    return number;
}

double doubleInput(string message) {
    double number;

    cout << message;

    while (!(cin >> number) || number < 0) {
        cout << "Invalid input. Enter a number that is zero or higher: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    cin.ignore(10000, '\n');
    return number;
}

// Risk Calculation
// Out in the open to adjust easier

double calculateExposure(double printingHours, double relativeHumidity) {
    return printingHours * (relativeHumidity / 100.0);
}

// File Handling Functions

// All filament information and printing events are saved in one text file.
// Each piece of information is written on its own line.

bool saveData(vector<Filament>& filaments) {
    ofstream file("filament_data.txt");

    if (!file.is_open()) {
        cout << "Error: Could not save the data.\n";
        return false;
    }

    file << filaments.size() << endl;

    for (int i = 0; i < filaments.size(); i++) {
        file << filaments[i].name << endl;
        file << filaments[i].type << endl;
        file << filaments[i].weight << endl;
        file << filaments[i].dateOpened << endl;
        file << filaments[i].events.size() << endl;

        for (int j = 0; j < filaments[i].events.size(); j++) {
            file << filaments[i].events[j].printingHours << endl;
            file << filaments[i].events[j].relativeHumidity << endl;
            file << filaments[i].events[j].gramsUsed << endl;
            file << filaments[i].events[j].dateTime << endl;
            file << filaments[i].events[j].exposure << endl;
        }
    }

    file.close();
    return true;
}

bool loadData(vector<Filament>& filaments) {
    ifstream file("filament_data.txt");

    if (!file.is_open()) {
        ofstream newFile("filament_data.txt");

        if (!newFile.is_open()) {
            cout << "Error: Could not create the data file.\n";
            return false;
        }

        newFile << 0 << endl;
        newFile.close();
        return true;
    }

    int filamentCount;
    file >> filamentCount;
    file.ignore(10000, '\n');

    for (int i = 0; i < filamentCount; i++) {
        Filament filament;
        int eventCount;

        getline(file, filament.name);
        getline(file, filament.type);
        file >> filament.weight;
        file.ignore(10000, '\n');
        getline(file, filament.dateOpened);

        file >> eventCount;
        file.ignore(10000, '\n');

        for (int j = 0; j < eventCount; j++) {
            printingEvent event;

            file >> event.printingHours;
            file.ignore(10000, '\n');

            file >> event.relativeHumidity;
            file.ignore(10000, '\n');

            file >> event.gramsUsed;
            file.ignore(10000, '\n');

            getline(file, event.dateTime);

            file >> event.exposure;
            file.ignore(10000, '\n');

            filament.events.push_back(event);
        }

        filaments.push_back(filament);
    }

    file.close();
    return true;
}

// Filament Display Functions

bool displayFilaments(vector<Filament>& filaments) {
    if (filaments.size() == 0) {
        cout << "\nNo filaments in the inventory.\n";
        return false;
    }

    cout << "\n--- AVAILABLE FILAMENTS ---\n";

    for (int i = 0; i < filaments.size(); i++) {
        cout << i + 1 << ". "
             << filaments[i].name
             << " (" << filaments[i].type << ")"
             << " - Weight: " << filaments[i].weight << " g\n";
    }

    return true;
}

void displayFilamentDetails(vector<Filament>& filaments) {
    if (!displayFilaments(filaments)) {
        return;
    }

    int choice = intInput("Enter the filament number: ");

    if (choice < 1 || choice > filaments.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    Filament& filament = filaments[choice - 1];

    cout << "\n--- FILAMENT DETAILS ---\n";
    cout << "Name: " << filament.name << endl;
    cout << "Type: " << filament.type << endl;
    cout << "Remaining weight: " << filament.weight << " g\n";
    cout << "Date opened: " << filament.dateOpened << endl;
    cout << "Number of printing events: " << filament.events.size() << endl;

    double totalExposure = 0;

    for (int i = 0; i < filament.events.size(); i++) {
        totalExposure = totalExposure + filament.events[i].exposure;
    }

    cout << "Total exposure: " << totalExposure << endl;

    for (int i = 0; i < filament.events.size(); i++) {
        cout << "\nEvent " << i + 1 << endl;
        cout << "Date/time: " << filament.events[i].dateTime << endl;
        cout << "Printing hours: " << filament.events[i].printingHours << endl;
        cout << "Relative humidity: " << filament.events[i].relativeHumidity << "%\n";
        cout << "Grams used: " << filament.events[i].gramsUsed << " g\n";
        cout << "Exposure: " << filament.events[i].exposure << endl;
    }
}

// Filament Inventory Manager

void addFilament(vector<Filament>& filaments) {
    Filament filament;

    cout << "\n--- ADD FILAMENT ---\n";

    cout << "Enter filament name: ";
    getline(cin, filament.name);

    cout << "Enter filament type: ";
    getline(cin, filament.type);

    filament.weight = doubleInput("Enter filament weight in grams: ");

    cout << "Current date/time: " << getCurrentDateTime() << endl;
    cout << "Enter date/time opened, or press ENTER to use current date/time: ";
    getline(cin, filament.dateOpened);

    if (filament.dateOpened == "") {
        filament.dateOpened = getCurrentDateTime();
    }

    filament.events.clear();
    filaments.push_back(filament);

    if (saveData(filaments)) {
        cout << "Filament added and saved.\n";
    }
}

void removeFilament(vector<Filament>& filaments) {
    if (!displayFilaments(filaments)) {
        return;
    }

    int choice = intInput("Select the filament number: ");

    if (choice < 1 || choice > filaments.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    int index = choice - 1;
    double amount = doubleInput("Enter the amount to remove in grams: ");

    if (amount > filaments[index].weight) {
        cout << "You cannot remove more than the remaining weight.\n";
        return;
    }

    filaments[index].weight = filaments[index].weight - amount;

    if (filaments[index].weight <= 0) {
        cout << "The filament has been fully used and will be removed.\n";
        filaments.erase(filaments.begin() + index);
    } else {
        cout << "Remaining weight: " << filaments[index].weight << " g\n";
    }

    if (saveData(filaments)) {
        cout << "Inventory saved.\n";
    }
}

// Printing Events

void addPrintingEvent(vector<Filament>& filaments) {
    if (!displayFilaments(filaments)) {
        return;
    }

    int choice = intInput("Select the filament number: ");

    if (choice < 1 || choice > filaments.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    int index = choice - 1;
    printingEvent event;

    cout << "\n--- ADD PRINTING EVENT ---\n";

    event.printingHours = doubleInput("Enter printing hours: ");
    event.relativeHumidity = doubleInput("Enter relative humidity (0-100): ");

    if (event.relativeHumidity > 100) {
        cout << "Relative humidity must be between 0 and 100.\n";
        return;
    }

    event.gramsUsed = doubleInput("Enter grams used: ");

    if (event.gramsUsed > filaments[index].weight) {
        cout << "Grams used cannot exceed the remaining filament weight.\n";
        return;
    }

    cout << "Current date/time: " << getCurrentDateTime() << endl;
    cout << "Enter event date/time, or press ENTER to use current date/time: ";
    getline(cin, event.dateTime);

    if (event.dateTime == "") {
        event.dateTime = getCurrentDateTime();
    }

    event.exposure = calculateExposure(
        event.printingHours,
        event.relativeHumidity
    );

    filaments[index].events.push_back(event);
    filaments[index].weight = filaments[index].weight - event.gramsUsed;

    cout << "\nEvent exposure: " << event.exposure << endl;
    cout << "Remaining weight: " << filaments[index].weight << " g\n";

    if (filaments[index].weight <= 0) {
        cout << "The filament has been fully used and will be removed.\n";
        filaments.erase(filaments.begin() + index);
    }

    if (saveData(filaments)) {
        cout << "Printing event and inventory saved.\n";
    }
}

void displayPrintingHistory(vector<Filament>& filaments) {
    bool foundEvent = false;

    for (int i = 0; i < filaments.size(); i++) {
        if (filaments[i].events.size() > 0) {
            foundEvent = true;

            cout << "\nFilament: " << filaments[i].name << endl;

            for (int j = 0; j < filaments[i].events.size(); j++) {
                cout << "\nEvent " << j + 1 << endl;
                cout << "Date/time: " << filaments[i].events[j].dateTime << endl;
                cout << "Printing hours: " << filaments[i].events[j].printingHours << endl;
                cout << "Relative humidity: " << filaments[i].events[j].relativeHumidity << "%\n";
                cout << "Grams used: " << filaments[i].events[j].gramsUsed << " g\n";
                cout << "Exposure: " << filaments[i].events[j].exposure << endl;
            }
        }
    }

    if (!foundEvent) {
        cout << "\nNo printing events have been recorded.\n";
    }
}

// Menu and Main

void showMenu() {
    cout << "\n===== 3D FILAMENT MOISTURE TRACKER =====\n";
    cout << "1. Add / Remove Filament\n";
    cout << "2. Add Printing Event\n";
    cout << "3. Display Filaments\n";
    cout << "4. Display Filament Details\n";
    cout << "5. Display Printing History\n";
    cout << "6. Finish\n";
}

int main() {
    vector<Filament> filaments;

    cout << "Current date/time: " << getCurrentDateTime() << endl;

    if (!loadData(filaments)) {
        cout << "Unable to load inventory. Program ended.\n";
        return 1;
    }

    int choice = 0;

    while (choice != 6) {
        showMenu();
        choice = intInput("Enter your choice: ");

        if (choice == 1) {
            int subChoice;

            cout << "\n1. Add Filament\n";
            cout << "2. Remove Used Filament\n";
            cout << "3. Return to Main Menu\n";

            subChoice = intInput("Enter your choice: ");

            if (subChoice == 1) {
                addFilament(filaments);
            } else if (subChoice == 2) {
                removeFilament(filaments);
            } else if (subChoice != 3) {
                cout << "Invalid choice.\n";
            }
        } else if (choice == 2) {
            addPrintingEvent(filaments);
        } else if (choice == 3) {
            displayFilaments(filaments);
        } else if (choice == 4) {
            displayFilamentDetails(filaments);
        } else if (choice == 5) {
            displayPrintingHistory(filaments);
        } else if (choice == 6) {
            if (saveData(filaments)) {
                cout << "Inventory saved.\n";
            }
            cout << "Thank you for using the tracker.\n";
        } else {
            cout << "Invalid choice. Please select 1-6.\n";
        }
    }

    return 0;
}
