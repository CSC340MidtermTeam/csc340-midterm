#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cfloat>

#include "WeatherTable.h"

#ifndef MAIN_CPP_RIDESTABLE_H
#define MAIN_CPP_RIDESTABLE_H

struct rideData{
    double price;
    float distance;
    string type;
};

struct deliverables{
    string weather;
    float distGroup;
    string rideType;

    double avgPricePerMiles;
    double lowestPpm;
    double highestPpm;
    int dataCount;
};

typedef struct rideData rideData;
typedef struct deliverables deliverables;

// Class to store all cab_rides data.
class RidesTable {
    /* 3x nested vector that contains rides data by:
     * (a) weather (0=clear, 1=rain) (b) rideType; (c) distance group */
    vector<vector<vector<vector<rideData>>>> data;

public:
    vector<string> rideType;
    vector<float> distGroup;

    RidesTable(); // Default constructor to initialize data structure
    vector<deliverables> processData(); // process stored data and return vector of deliverables
    void insert(const vector<string> *cabRidesRow, WeatherTable *weatherTable); // insert data from row vector


};


#endif //MAIN_CPP_RIDESTABLE_H
