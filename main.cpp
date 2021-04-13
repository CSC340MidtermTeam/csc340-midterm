#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#define MAX_LENGTH 512
#include <algorithm>

#ifdef WINDOWS
#include <direct.h>
#define CurrentSysDir _getcwd
#else
#include <unistd.h>
#define CurrentSysDir getcwd
#endif

using namespace std;

class WeatherTable;
string getCurrentDir ();

/*----------------------------------------------------------------------------------------------------------------------
Struct (to be refactored)
----------------------------------------------------------------------------------------------------------------------*/
struct rideData{

};


struct weatherData{
    float temp;
    string location;
    float clouds;
    float pressure;
    float rain;
    time_t time_stamp;
    float humidity;
    float wind;
};

typedef struct rideData rideData;
typedef struct weatherData weatherData;

// Typedef for unordered_map
typedef unordered_map<string, int>::iterator iter;
typedef pair<iter, bool> insertReturn;

// Comparison operator to make weatherData a comparable data type
bool operator<(weatherData a, weatherData b) {
    return a.time_stamp < b.time_stamp;
}

/*----------------------------------------------------------------------------------------------------------------------
class implementation
----------------------------------------------------------------------------------------------------------------------*/
// Class to store all weather data.
class WeatherTable {
    vector<vector<weatherData>> dataTable;
    unordered_map<string, int> locationIndex;
    int sorted = 0;

public:
    WeatherTable() {
        dataTable = {};
        locationIndex = unordered_map<string, int>();
    }

    void insert(weatherData data) {
        int i = dataTable.size();
        // Check return value of insert() . If insert() succeeds, create a new weatherData vector with the
        // data in the argument as the first element. Otherwise push back the data to the appropriate vector.
        insertReturn p = locationIndex.insert(pair<string, int> (data.location, i));
        if(p.second) {
            dataTable.push_back({data});
            i++;
        }
        else {
            int index = p.first->second;
            dataTable.at(index).push_back(data);
            sorted = 0;
        }
    }

    weatherData getWeather(string location, time_t time) {
        time_t closest =  time;
        // sort all weatherData vector (ascending).
        if (!sorted) {
            for (int i = 0; i < dataTable.size(); i++) {
                sort(dataTable.at(i).begin(), dataTable.at(i).end());
            }
            sorted = 1;
        }

        // Find the index of weatherData vector of the queried location in dataTable
        iter it = locationIndex.find(location);
        if (it == locationIndex.end()) {
            cout << "ERROR: " << location << " not found." << endl;
            throw;
        }
        int i = it->second;
        int j = 0;
        dataTable.at(i);

        // Perform linear search to find the weather data at the closest time_stamp as the queried argument. Since
        // vector is sorted, stop the search once the difference is larger than the previous difference.
        for (; j < dataTable.at(i).size(); j++) {
            time_t diff = (time - dataTable.at(i).at(j).time_stamp);
            if (diff < 0) {
                diff *= -1;
            }
            if (diff < closest) {
                closest = diff;
            } else {break;}
        }

        return dataTable.at(i).at(j-1);
    }
};



// return string that contains the directory of the current .cpp file
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

/*----------------------------------------------------------------------------------------------------------------------
Main
----------------------------------------------------------------------------------------------------------------------*/

int main() {
    typedef weatherData weatherData;
    string cabRides = getCurrentDir() + "\\data\\cab_rides.csv";
    string weather = getCurrentDir() + "\\data\\weather.csv";

    WeatherTable weatherTable = WeatherTable();

/*----------------------------------------------------------------------------------------------------------------------
File IO
----------------------------------------------------------------------------------------------------------------------*/

    // Open csv files and read data into vectors------------------------------------------------------------------------
    fstream fin;
    vector<string> row;
    string line, temp;

    /*------------------------------------------------------------------------------------------------------------------
    * read weather data
    ------------------------------------------------------------------------------------------------------------------*/

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

        weatherData d;
        d.temp = stof(row.at(0));
        d.location = row.at(1);
        d.clouds = stof(row.at(2));
        d.pressure = stof(row.at(3));
        d.rain = stof(row.at(4));

        const char *time_cstr = row.at(5).c_str();
        time_t time_ = strtoull(time_cstr, NULL, 0)*1000;

        d.time_stamp = time_;
        d.humidity = stof(row.at(6));
        d.wind = stof(row.at(7));

        weatherTable.insert(d);
    }
    fin.close();
    cout << "Finished!" << endl;

    /*------------------------------------------------------------------------------------------------------------------
    * read cab rides data
    ------------------------------------------------------------------------------------------------------------------*/
    cout << "Opening " << cabRides << endl;
    fin.open(cabRides, ios::in);
    if (fin.fail()) {
        cout << "Failed to open File " << cabRides << endl;
    }

    getline(fin, temp); // Skip the first line (header row).
    while (getline(fin, line)) {
        row.clear();
        populateVectorFromLine(row, line);

        // Skip rides that are shorter than 0.5 miles;
        if (stof(row.at(0)) < 0.5) {
            continue;
        }

        const char *time = row.at(2).c_str();
        weatherData weather = weatherTable.getWeather(row.at(4), strtoull(time, NULL, 0));

//            A bunch of cout to see if getWeather() is working properly
//            cout << weather.location << endl;
//            cout << row.at(4) << endl;
//            cout << weather.time_stamp << endl;
//            cout << row.at(2) << endl << endl;

        /*--------------------------------------------------------------------------------------------------------------
        * FIXME Group data into vectors based on weather and distance
        --------------------------------------------------------------------------------------------------------------*/
        // 1st-D: .at(0): < 2miles | .at(1): 2-5.0 miles | .at(2) > 5.0 miles
        // Add another dimension for rain/no rain or keep them in separate vectors.
        vector<vector<string>> lyft;
        vector<vector<string>> lyft_no_rain;
        vector<vector<string>> uber_rain;
        vector<vector<string>> uber_no_rain;

        if (weather.rain > 0.0) {
            // if type is uberX, insert to uberX vector
            // if type is Lyft, inset into Lyft vector

        } else {
            // if type is uberX, insert to uberX vector
            // if type is Lyft, inset into Lyft vector
        }
    }
        fin.close();
        cout << "Finished!" << endl;

}
