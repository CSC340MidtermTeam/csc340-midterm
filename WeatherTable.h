#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cfloat>
#include <algorithm>

#ifndef MAIN_CPP_WEATHERTABLE_H
#define MAIN_CPP_WEATHERTABLE_H

using namespace std;

typedef struct weatherData weatherData;
typedef unordered_map<string, int>::iterator iter;
typedef pair<iter, bool> insertReturn;
bool operator<(weatherData a, weatherData b);

struct weatherData {
    float temp;
    string location;
    float clouds;
    float pressure;
    float rain;
    time_t time_stamp;
    float humidity;
    float wind;
};

class WeatherTable {
    vector<vector<weatherData>> dataTable;
    unordered_map<string, int> locationIndex;
    int sorted;

public:
    WeatherTable(); // Default constructor.
    weatherData getWeather(string location, time_t time);   // Get weatherData object at the clsoest time and location
    void insert(vector<string> *data);  // Insert data from .csv row vector.

};

#endif //MAIN_CPP_WEATHERTABLE_H