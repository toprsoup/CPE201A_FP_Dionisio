#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;

// Structure for a printing event
struct printingEvent {
    double printingHours;
    double relativeHumidity;
    double gramsUsed;
    string dateTime;
    double exposure;
};

// Structure for a filament
struct Filament {
    string name;
    string type;
    double weight;
    string dateOpened;

    vector<printingEvent> events;
};

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
    time_t now = time (0)
    cout << "Current Date and Time: " << ctime(&now);
}




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

// D, fil display and display details
void filamentDisplay(vector<Filament> filaments) {
    if (filaments.empty) {
        cout << "empty\n";
    }
 }
// show menu/main

int main () {
// var declaration: 
vector<Filament> filaments;

return 0;
}
