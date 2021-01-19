// Amber Weiner 208783522

#include "timeseries.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>

/**
 * Instantiates a new TimeSeries.
 *
 * @param CSVFileName - the name of a file that this TimeSeries based on
 */

TimeSeries::TimeSeries(const char *CSVFileName) {
    this->file_name = CSVFileName;
        this->read_csv();
        // update the number of rows in the map
        if (!this->flights.empty()) {
            this->num_of_rows = this->flights.at(1).size();
        }
        // empty map case
        else
            this->num_of_rows = 0;
}

    /**
     * Gets flights.
     *
     * @return the map of flights for this TimeSeries.
     */
map<int, vector<float>> TimeSeries::get_flights() const {
    return this->flights;
}

    /**
     * Gets properties.
     *
     * @return the vector of properties for this TimeSeries.
     */
vector<string> TimeSeries::get_properties() const {
    return this->properties;
}

/**
 * The function check if a given CSV file is valid.
 * Meaning, if it was successfully opened or not.
 * @return true if valid or else false.
 */
int TimeSeries::get_num_of_rows() const {
    return this->num_of_rows;
}

/**
* Parse properties.
*
 * The function read a given line and update this properties accordingly.
 */
void TimeSeries::parseProperties(const string& line) {
    string current;
    stringstream s(line);
    while(getline(s, current, ',')){
        this->properties.push_back(current);
    }
}

/**
* Initial flights.
*
 * The function initial this flights map.
 */
void TimeSeries::initialFlights(){
    int size = this->properties.size();
    for (int i = 0; i < size; i++){
        vector<float> values;
        this->flights.insert({i, values});
    }
}

/**
* Read CSV.
* 
 * The function read a given CSV file and update the members
 */
void TimeSeries::read_csv() {
    fstream file(this->file_name);
    string line = "",word="";
    // read an entire row and store it in a string variable 'line'
    getline(file, line);
    // update properties vector
    this->parseProperties(line);
    // initiate this flights map
    this->initialFlights();
    int location;
    // parse all the given data from file to this flights
    while (getline(file, line)) {
        location=0;
        stringstream s(line);
        while (getline(s, word, ',')) {
            this->flights.at(location).push_back(stof(word));
            location++;
        }
    }
}
