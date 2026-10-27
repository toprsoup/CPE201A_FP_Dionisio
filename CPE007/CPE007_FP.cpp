#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;

// Structures for filaments and printing event
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

// Input cutter functions

string strInput() {    
    string word;
    getline(cin, word);
    return word;
}

int intInput(string messageinput) {   
    while (true) {                                        
        cout << messageinput;
        int integer;
        if (cin >> integer) {                              
            return integer;
        }
        else {
            cout << "> Invalid input. Please enter a valid integer." << endl;
            cin.clear();                                 
            cin.ignore(100000, '\n');
        }
    }
}


// Filament Management

static int __put_filament_content(ofstream &f, struct Filament *fil){
    f << "--- FILAMENT INFORMATION ---" << endl
      << "Name: " << fil->name << endl
      << "Type: " << fil->type << endl
      << "Weight: " << fil->weight << endl
      << "Date Opened: " << fil->dateOpened << endl;
}

static int __put_printevent_content(ofstream &f, struct printingEvent &pe, int i){
    f << "--- PRINTING EVENT NUMBER #" << i << " INFORMATION ---" << endl
      << "Printing Hours:" << pe.printingHours << endl
      << "Relative Humidity:" << pe.relativeHumidity << endl
      << "Grams Used:" << pe.gramsUsed << endl
      << "Date Time:" << pe.dateTime << endl
      << "Exposure:" << pe.exposure << endl
}

// Creates a file describing a filament.
int addFilament(struct Filament *fil){
    ofstream f(fil->name);
    if(!f.is_open()){
        return -1;
    }
    __put_filament_content(f, fil);
    for(int i = 0; i < fil->events.size(); i++){
        __put_printevent_content(f, (fil->events[i]), i);
    }
    return 0;
}

// Get current time and date
void getCurrentTimeDate () {
    time_t now = time (0);
    cout << "Current Date and Time: " << ctime(&now);
}

// filament display and display details
bool displayFilaments(const vector<Filament>& filaments) {
    if (filaments.empty()) {
        cout << "\n[!] No filaments currently in inventory.\n";
        return false;
    }
    
    cout << "\n--- Available Filaments ---\n";
    for (size_t i = 0; i < filaments.size(); ++i) {
        cout << i + 1 << ". " << filaments[i].name << " (" << filaments[i].type 
             << ") - Remaining Weight: " << filaments[i].weight << "g\n";
    }
    return true;
}

void displayFilamentDetails(const vector<Filament>& filaments) {
    if (!displayFilaments(filaments)) return;

    int choice;
    cout << "\nEnter the number of the filament to view details: ";
    cin >> choice;

    if (choice >= 1 && choice <= static_cast<int>(filaments.size())) {
        const Filament& f = filaments[choice - 1];
        cout << "\n========================================\n";
        cout << " FILAMENT DETAILS: " << f.name << "\n";
        cout << "========================================\n";
        cout << "Type: " << f.type << "\n";
        cout << "Current Weight: " << f.weight << "g\n";
        cout << "Date Opened: " << f.dateOpened << "\n";
        cout << "Printing Events Recorded: " << f.events.size() << "\n";

        if (!f.events.empty()) {
            cout << "\n--- Event History ---\n";
            for (size_t i = 0; i < f.events.size(); ++i) {
                cout << "  Event " << i + 1 << ":\n";
                // wait for event structure
            //    cout << "    - Date/Time: " << f.events[i].date_time << "\n";
            //    cout << "    - Hours Printed: " << f.events[i].hours << " hrs\n";
            //    cout << "    - Relative Humidity (RH): " << f.events[i].rh << "%\n";
            //    cout << "    - Grams Used: " << f.events[i].grams_used << "g\n";
            //    cout << "    - Calculated Exposure: " << fixed << setprecision(2) << f.events[i].exposure << "\n";
            }
        }
        cout << "========================================\n";
    } else {
        cout << "[!] Invalid selection.\n";
    }
}

// show menu/main (this is literally just on main choice of pick 1 or 2 bruh)

int main () {
// var declaration: 
vector<Filament> filaments;

return 0;
}
