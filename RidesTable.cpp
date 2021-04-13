//
// Created by micha on 4/13/2021.
//

#include "RidesTable.h"


// Default constructor. Initiate empty nested vector to store data.
RidesTable::RidesTable() {
    rideType = {"UberX", "Lyft", "UberPool"};
    distGroup = {2.0, 5.0, 8.0};

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

// process stored data and return vector of "deliverables" struct.
vector<deliverables> RidesTable::processData() {
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

// Insert row vector from each row of cab_rides file into data vectors
void RidesTable::insert(const vector<string> *cabRidesRow, WeatherTable *weatherTable) {
    int i = 0;
    int j = 0;
    int k = 0;

    rideData ride;
    ride.distance = stof(cabRidesRow->at(0));
    ride.price = stod(cabRidesRow->at(5));
    ride.type = cabRidesRow->at(9);


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

    // Get weather data at the time the ride was requested.
    const char *time_str = cabRidesRow->at(2).c_str();
    const time_t time = strtoull(time_str, NULL, 0);
    weatherData weather = weatherTable->getWeather(cabRidesRow->at(4), time);

    // Group data based on rain condition (rain at <0.02 inch per hour is considered trace).
    if (weather.rain > 0.02) {i = 1;}
    else {i = 0;}

    data.at(i).at(j).at(k).push_back(ride);
}