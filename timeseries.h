// Amber Weiner 208783522

#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <vector>
#include <map>

using namespace std;

class TimeSeries {
private:
    map<int, vector<float>> flights;
    vector<string> properties;
    const char *file_name;
    int num_of_rows;

    void parseProperties(const string& line) ;
    void initialFlights();
    void read_csv();

public:
    explicit TimeSeries(const char *CSVfileName);

    map<int, vector<float>> get_flights() const;

    vector<string> get_properties() const;

    int get_num_of_rows() const;
};

#endif /*TIMESERIES_H */
