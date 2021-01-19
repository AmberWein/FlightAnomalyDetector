// Amber Weiner 208783522

#include "SimpleAnomalyDetector.h"
#include <cmath>
#include "anomaly_detection_util.h"

// default constructor
SimpleAnomalyDetector::SimpleAnomalyDetector() = default;

// default destructor
SimpleAnomalyDetector::~SimpleAnomalyDetector() = default;

/**
 * The function find the name of a given feature and return it
 * @param ts - a given TimeSeries
 * @param feature - the feature's value
 * @return the name (string) of a given feature (int value)
 */
string SimpleAnomalyDetector::getFeature(const TimeSeries &ts, int feature) {
    vector<string> properties = ts.get_properties();
    return properties.at(feature);
}

/**
 * The function find the maximum deviation between each point and the given line
 * and return the maximum
 * @param points
 * @param size
 * @param regLine
 * @return the the max threshold value
 */
float SimpleAnomalyDetector::findThreshHold(vector<Point*> points, int size, Line regLine) {
    float max = 0;
    float current;
    for (int i = 0; i < size; i++) {
        current = dev(*points[i], regLine);
        if(current > max)
            max = current;
    }
    return max;
}

/**
 * The function create a vector of new points
 * each one made of two given vectors:
 * x's values- from the first feature
 * y's value- from the second feature
 * @param first - the first feature
 * @param second - the second feature
 * @param size
 * @return a vector of points made out of the two given vectors
 */
vector<Point*> SimpleAnomalyDetector::findPoints(vector<float> first, vector<float> second,int size) {
    vector<Point*> points;
    // create a vector out of the given features
    for(int i = 0; i < size; i++) {
        auto* current = new Point(first.at(i), second.at(i));
        points.push_back(current);
    }
    return points;
}

/**
 * The function check if the given pair of features are already existed in the cf
 * @param first - the first feature name
 * @param second - the second feature name
 * @return true if the pair already exist, false otherwise
 */
bool SimpleAnomalyDetector::isPairExist(const string& feature1, const string& feature2) {
    // go over the cf and check if both features are existed in there
    for(int i = 0; i < this->cf.size() ; i++) {
        if(((cf.at(i).feature1 == feature1) && (cf.at(i).feature2 == feature2)
           || (cf.at(i).feature2 == feature1) && cf.at(i).feature1 == feature2))
            return true;
    }
    return false;
}

/**
 * The function delete the given vector of points.
 * @param points - a vector of points.
 */
void SimpleAnomalyDetector::freePoints(vector<Point*> points) {
    for(int i = 0 ; i < points.size(); i++) {
        if (points[i] != nullptr)
            delete points[i];
        i++;
    }
}

/**
 * The function learn what the normal correlation between two objects
 * by a given timeSeries
 * @param ts - a given TimeSeries
 * @param first - the index value of the first feature
 * @param second - the index value of the second feature
 * @param max_pearson - the maximum value of the first feature
 */
 void SimpleAnomalyDetector::addCorrelatedFeatures(const TimeSeries &ts, int first, int second, float max_pearson) {
    vector<string> properties = ts.get_properties();
    map<int, vector<float>> flights = ts.get_flights();
    vector<float> feature1 = flights.at(first);
    vector<float> feature2 = flights.at(second);
    string feature1Name = properties.at(first);
    string feature2Name = properties.at(second);
    // check if the pair already exist- if not, then add it to the cf
    if(!isPairExist(feature1Name, feature2Name))
    {
        int size = ts.get_num_of_rows();
        vector<Point*> points = findPoints(feature1, feature2, size);
        Point** pointsArray = points.data();

        Line regLine = linear_reg(pointsArray, size);
        float threshold = this->findThreshHold(points, size, regLine);
        struct correlatedFeatures correlated_features = {feature1Name, feature2Name, max_pearson, regLine,
                threshold, true};
        this->cf.push_back(correlated_features);
        this->freePoints(points);
    }
 }

/**
* The function check if the given two features are correlated according to this correlation
* @param max_pearson - a given pearson
* @param ts - a given TimeSeries
* @param first - the first feature
* @param second - the second feature
*/
void SimpleAnomalyDetector::checkPearson(float max_pearson, const TimeSeries &ts, int first, int second) {
    if (max_pearson >= this->simple_correlation) {
        this->addCorrelatedFeatures(ts, first, second, max_pearson);
    }
 }

/**
 * The function learn what the normal correlation between two objects
 * by a given timeSeries
 * @param ts - a given TimeSeries
 */
void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    vector<string> properties = ts.get_properties();
    map<int, vector<float>> flights = ts.get_flights();
    float max_pearson, current;
    int num_of_col = properties.size();
    int num_of_rows = ts.get_num_of_rows();
    int first, second;
    // go over each column
    for (int outer = 0; outer < num_of_col; outer++) {
        // find the maximum for each line
        max_pearson = 0;
        // compare the current column with all the others
        for (int inner = 0; inner < num_of_col; inner++) {
            // check if reached to the same column as we check- if so, ignore it
            if (outer == inner)
                continue;
            float* feature1 = flights.at(outer).data();
            float* feature2 = flights.at(inner).data();
            current = pearson(feature1, feature2, num_of_rows);
            if (abs(current) > max_pearson) {
                max_pearson = abs(current);
                first = outer;
                second = inner;
            }
        }
        this->checkPearson(max_pearson,ts, first, second);

    }
}

/**
 * The function return the column number of a feature if existed
 * in the given TimeSeries or else return -1
 * @param first - the first feature name
 * @param second - the second feature name
 * @return the column number of a given feature or else (if not existed) -1
 */
int SimpleAnomalyDetector::getFeatureColumn(const TimeSeries &ts, const string& feature) {
    vector<string> properties = ts.get_properties();
    int columnIndex = 0;
    // go over each feature and check if it's name fits what we look for
    for (const string& property : properties) {
        if(property == feature)
            return columnIndex;
        columnIndex++;
    }
    // the feature doesn't exist
    return -1;
}

/**
 * The function learn what the normal correlation between two objects
 * by a given timeSeries
 * @param ts - a given TimeSeries
 * @param first - the index value of the first feature
 * @param second - the index value of the second feature
 * @param max_pearson - the maximum value of the first feature
 */
bool SimpleAnomalyDetector::isAnomaly(Point current, const correlatedFeatures &correlated) {
    if (dev(current, correlated.lin_reg) >= (1.1 * correlated.threshold))
        return true;
    return false;
}

/**
 * The function use cf and a given TimeSeries to look for anomaly to report.
 * Each pair that found anomaly being report
 * @param ts - a given TimeSeries
 * @return a vector of AnomalyReport based on cf and the given TimeSeries
 */
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts) {
    vector<string> properties = ts.get_properties();
    map<int, vector<float>> flights = ts.get_flights();
    vector<AnomalyReport> anomalies;
    long line;
    // go over cf and check each pair
    for (const correlatedFeatures& correlated : this->cf) {
        line = 0;
        int feature1Col = this->getFeatureColumn(ts, correlated.feature1);
        int feature2Col = this->getFeatureColumn(ts, correlated.feature2);
        // check if both features are existed in the given TimeSeries
        // if so, then check for anomaly to report
        if((feature1Col != -1) && (feature2Col != -1)) {
            vector<float> feature1 = flights.at(feature1Col);
            vector<float> feature2 = flights.at(feature2Col);
            // compare each time step for each features
            int size = feature1.size();
            for(int i = 0; i < size; i++) {
                line++;
                if (line == 8) {
                    int k;
                    k=0;
                }
                Point current = Point(feature1.at(i), feature2.at(i));
                // check if anomaly was found,
                // if so then add it to the AnomalyReport vector
                if (this->isAnomaly(current, correlated)) {
                    string description = properties.at(feature1Col) + "-" + properties.at(feature2Col);
                    anomalies.push_back(AnomalyReport(description,line));
                }
            }
        }
    }
    return anomalies;
}
