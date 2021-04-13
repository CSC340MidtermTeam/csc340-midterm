#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cfloat>
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
Structs
----------------------------------------------------------------------------------------------------------------------*/

typedef struct {
    float temp;
    string location;
    float clouds;
    float pressure;
    float rain;
    time_t time_stamp;
    float humidity;
    float wind;
} weatherData;

// Comparison operator to make weatherData a comparable data type
bool operator<(weatherData a, weatherData b) {
    return a.time_stamp < b.time_stamp;
}

typedef struct {
    double price;
    float distance;
    string type;
} rideData;

typedef struct {
    string weather;
    float distGroup;
    string rideType;

    double avgPricePerMiles;
    double lowestPpm;
    double highestPpm;
    int dataCount;
} deliverables;

// Typedef for unordered_map components
typedef unordered_map<string, int>::iterator iter;
typedef pair<iter, bool> insertReturn;

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

class RidesTable {
    /* 3x nested vector that contains rides data
     * -1st layer: weather (0=clear, 1=rain)
     * -2nd layer: rideType
     * -3rd layer: distance group */
    vector<vector<vector<vector<rideData>>>> data;

public:
    vector<string> rideType = {"UberX", "Lyft"};
    vector<float> distGroup = {2.0, 5.0, 8.0};

    // Default constructor. Initiate empty nested vector to store data.
    RidesTable() {
        data.clear();
        for (int i = 0; i < 2; i++) {
            // Initiate empty 3D vector
            vector<vector<vector<rideData>>> empty3D;
            for (int j = 0; j < rideType.size(); j++) {
                // Initiate empty 2D vector
                vector<vector<rideData>> empty2D;
                empty2D.clear();
                for (int k = 0; k < distGroup.size(); k++) {
                    // Initiate empty 1D vector
                    vector<rideData> empty1D;
                    empty1D.clear();
                    empty2D.push_back(empty1D);
                }
                empty3D.push_back(empty2D);
            }
            data.push_back(empty3D);
        }
    }

    // Insert row vector from each row of cab_rides file into data vectors
    void insert(const vector<string> *cabRidesRow, const weatherData *weather) {
        int i = 0;
        int j = 0;
        int k = 0;

        rideData ride;
        ride.distance = stof(cabRidesRow->at(0));
        ride.price = stod(cabRidesRow->at(5));
        ride.type = cabRidesRow->at(9);

        // Group data based on rain condition (rain at <0.02 inch per hour is considered trace).
        if (weather->rain > 0.02) {i = 1;}
        else {i = 0;}

        // Group data based on the type of ride. If the ride type is not in rideType vector, ignore the data.
        for (; j < rideType.size(); j++) {
            if (ride.type.compare(rideType.at(j)) == 0) {
                break;
            }
        }
        if (j >= rideType.size() ) {
            return;
        }

        // Group data based on ride distance
        for (k = 0; k < distGroup.size(); k++) {
            if (ride.distance < distGroup.at(k)) {
                break;
            }
        }
        data.at(i).at(j).at(k).push_back(ride);
    }

    // process stored data and return vector of "deliverables" struct.
    vector<deliverables> processData() {
        vector<deliverables> results;
        results.clear();

        // Process stored data by the following groupings: i = weather, j = ride type, k = distance.
        for (int i = 0; i < data.size(); i ++) {
            for (int j = 0; j < rideType.size(); j++) {
                for (int k = 0; k < distGroup.size(); k++) {
                    const vector<rideData> *v = &data.at(i).at(j).at(k);
                    double totalPpm = 0.0;

                    deliverables result;
                    result.lowestPpm = DBL_MAX;
                    result.highestPpm = 0;
                    for (int k = 0; k < v->size(); k++) {
                        double ppm = v->at(k).price / v->at(k).distance;
                        if (ppm < result.lowestPpm) {result.lowestPpm = ppm;}
                        if (ppm > result.highestPpm) {result.highestPpm = ppm;}
                        totalPpm += ppm;
                    }

                    result.dataCount = v->size();
                    if (i == 1) {result.weather = "rain";}
                    else {result.weather = "clear";}
                    result.avgPricePerMiles = totalPpm / result.dataCount;
                    result.distGroup = distGroup.at(k);
                    result.rideType = rideType.at(j);
                    results.push_back((result));
                }
            }
        }
        return results;
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
    RidesTable ridesTable = RidesTable();

    // Variables for file IO
    fstream fin;
    vector<string> row;
    string line, temp;

    /*------------------------------------------------------------------------------
    * read and store weather data
    ------------------------------------------------------------------------------*/
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

        // Convert time string into time_t
        const char *time_cstr = row.at(5).c_str();
        time_t time_ = strtoull(time_cstr, NULL, 0)*1000;

        // Populate weatherData struct
        weatherData d;
        d.temp = stof(row.at(0));
        d.location = row.at(1);
        d.clouds = stof(row.at(2));
        d.pressure = stof(row.at(3));
        d.rain = stof(row.at(4));
        d.time_stamp = time_;
        d.humidity = stof(row.at(6));
        d.wind = stof(row.at(7));

        weatherTable.insert(d);
    }
    fin.close();
    cout << "Finished!" << endl;

    /*------------------------------------------------------------------------------
    * read and store rides data
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

        // Ignore rides that are shorter than 0.5 miles or the ones without price data;
        if (stof(row.at(0)) < 0.5 || !row.at(5).size()) {
            continue;
        }

        const char *time_str = row.at(2).c_str();
        const time_t time = strtoull(time_str, NULL, 0);
        weatherData weather = weatherTable.getWeather(row.at(4), time);
        ridesTable.insert(&row, &weather);
    }
    fin.close();
    cout << "Finished!" << endl;

    // Process data and output results.
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
