//
// Created by micha on 4/13/2021.
//

#include "WeatherTable.h"

// Class to store all weather data.
WeatherTable::WeatherTable() {
        sorted = 0;
        dataTable = {};
        locationIndex = unordered_map<string, int>();
    };

void WeatherTable::insert(vector<string> *row) {
    int i = dataTable.size();
    // Convert time string into time_t
    const char *time_cstr = row->at(5).c_str();
    time_t time_ = strtoull(time_cstr, NULL, 0)*1000;

    // Populate weatherData struct
    weatherData d;
    d.temp = stof(row->at(0));
    d.location = row->at(1);
    d.rain = stof(row->at(4));
    d.time_stamp = time_;

    // Check return value of insert() . If insert() succeeds, create a new weatherData vector with the
    // data in the argument as the first element. Otherwise push back the data to the appropriate vector.
    insertReturn p = locationIndex.insert(pair<string, int> (d.location, i));
    if(p.second) {
        dataTable.push_back({d});
        i++;
    }
    else {
        int index = p.first->second;
        dataTable.at(index).push_back(d);
        sorted = 0;
    }
}

weatherData WeatherTable::getWeather(string location, time_t time) {
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

// Comparison operator to make weatherData a comparable data type
bool operator<(weatherData a, weatherData b) {
    return a.time_stamp < b.time_stamp;
}