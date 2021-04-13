#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "WeatherTable.h"
#include "RidesTable.h"

#ifdef WINDOWS
#include <direct.h>
#define CurrentSysDir _getcwd
#else
#include <unistd.h>
#define CurrentSysDir getcwd
#endif

#define MAX_LENGTH 512

using namespace std;

string getCurrentDir (); // return string that contains the directory of the current .cpp file
void populateVectorFromLine(vector<string> &vect, string &line); // Populates vector from string values separated by ','

int main() {
    WeatherTable weatherTable = WeatherTable(); // to store data from weather.csv
    RidesTable ridesTable = RidesTable();   // to store data from cab_rides.csv

    /*------------------------------------------------------------------------------
    * File IO 1 - read and store weather data
    ------------------------------------------------------------------------------*/
    string cabRides = getCurrentDir() + "\\data\\cab_rides.csv";
    string weather = getCurrentDir() + "\\data\\weather.csv";
    fstream fin;
    vector<string> row;
    string line, temp;

    cout << "Opening " << weather << endl;
    fin.open(weather, ios::in);
    if (fin.fail()) {
        cout << "Failed to open File " << cabRides << endl;
    }

    getline(fin, temp); // skip the first line
    while (getline(fin, line)) {
        row.clear();
        populateVectorFromLine(row, line);
        if (row.at(4).empty()) {
            row.at(4) = "0.0";
        }

        weatherTable.insert(&row);
    }
    fin.close();

    /*------------------------------------------------------------------------------
    * File IO 2 - read and store rides data
    ------------------------------------------------------------------------------*/
    cout << "Opening " << cabRides << endl;
    fin.open(cabRides, ios::in);
    if (fin.fail()) {
        cout << "Failed to open File " << cabRides << endl;
    }

    getline(fin, temp); // Skip the first line (header row).
    while (getline(fin, line)) {
        row.clear();
        populateVectorFromLine(row, line);

        // Ignore rides that are shorter than 0.5 miles or the ones with no price data;
        if (stof(row.at(0)) < 0.5 || !row.at(5).size()) {
            continue;
        }
        ridesTable.insert(&row, &weatherTable);
    }
    fin.close();

    /*------------------------------------------------------------------------------
    * Process data and output results
    ------------------------------------------------------------------------------*/
    vector<deliverables> res = ridesTable.processData();
    for (int i = 0; i<res.size(); i++) {
        cout << res.at(i).weather << " - ";
        cout << res.at(i).rideType << " - ";
        cout << res.at(i).distGroup << " miles group - avg price/mile: $";
        cout << res.at(i).avgPricePerMiles;
        cout << " | range: $" << res.at(i).lowestPpm;
        cout << " - $" << res.at(i).highestPpm;
        cout << " | data count: " << res.at(i).dataCount << endl;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
Functions implementation for main()
----------------------------------------------------------------------------------------------------------------------*/
// return string that contains the directory of the current .exe file
string getCurrentDir () {
    char buff[512];
    CurrentSysDir(buff, MAX_LENGTH);
    string dir(buff);
    return dir;
}

// Populates a given vector using values from a string, separated by a ','
void populateVectorFromLine(vector<string> &vect, string &line) {
    string value;
    stringstream s(line);
    while (getline(s,value, ',')) {
        vect.push_back(value);
    }
}



