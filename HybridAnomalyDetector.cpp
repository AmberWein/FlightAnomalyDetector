// Amber Weiner 208783522

#include "HybridAnomalyDetector.h"
#include "minCircle.h"

// default constructor
HybridAnomalyDetector::HybridAnomalyDetector() = default;

HybridAnomalyDetector::~HybridAnomalyDetector() {
    for (const correlatedFeatures& c: this->cf) {
    //for (int i = 0; i < this->cf.size(); i++) {
        // make sure to free the non null value points
        if (!c.isRegCorrelated) {
            if (c.center != nullptr)
                delete c.center;
        }
    }
}

/**
* The function return the minimum correlation
* @return the minimum correlation
*/
float HybridAnomalyDetector::getSimpleCorrelation() {
    return this->simple_correlation;
}

/**
* The function set the minimum correlation by a given value
* @param newCorrelation- a given value to update the minimum correlation value
*/

void HybridAnomalyDetector::setSimpleCorrelation(float newCorrelation) {
    this->simple_correlation = newCorrelation;
}

/**
 * overriding method
 */
void HybridAnomalyDetector::checkPearson(float max_pearson, const TimeSeries &ts, int first, int second) {
    // check if simple case
    if (max_pearson >= this->simple_correlation) {
        return SimpleAnomalyDetector::checkPearson(max_pearson, ts, first, second);
    }
    // check if hybrid case
    if (max_pearson >= this->hybrid_correlation) {
        string firstName = this->getFeature(ts, first);
        string secondName = this->getFeature(ts, second);
        // check if the given pair is not already in cf
        // if not- then add it
        if (!this->isPairExist(firstName, secondName)) {
            map<int, vector<float>> flights = ts.get_flights();
            vector<float> feature1 = flights.at(first);
            vector<float> feature2 = flights.at(second);
            int num_of_rows = ts.get_num_of_rows();
            vector<Point *> points = findPoints(feature1, feature2, num_of_rows);
            Point **pointsArray = points.data();
            Circle min = findMinCircle(pointsArray, points.size());
            Line regLine = Line(0, 0);
            float threshold = min.radius;
            Point* center = new Point(min.center.x, min.center.y);
            struct correlatedFeatures correlated = {firstName, secondName, max_pearson,
                                                    regLine, threshold, false, center};
            this->cf.push_back(correlated);
            return;
        }
    }
}

/**
* The function check if the given two points' distance
* is not bigger than the minimum radius
* @param current - a current point
* @param cf - a pair of correlated features
* @return if the distance is not bigger than the radius- true, or else false.
*/
bool HybridAnomalyDetector::isInCircle(Point current, const correlatedFeatures &correlated) {
    Point center = *correlated.center;
    float distance = dist(current, center);
    if (distance <= (1.1 * correlated.threshold))
        return true;
    return false;
}

/**
 * overriding method
 */
bool HybridAnomalyDetector::isAnomaly(Point current, const correlatedFeatures &correlated) {
    // check if simple case
    if (correlated.isRegCorrelated) {
            return SimpleAnomalyDetector::isAnomaly(current, correlated);
    }
        // this (hybrid) case
    else {
        // check if the two points are not an anomaly
        if (this->isInCircle(current, correlated))
            return false;
        return true;
    }
}

