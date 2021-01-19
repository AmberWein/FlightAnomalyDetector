// Amber Weiner 208783522

#ifndef SIMPLEANOMALYDETECTOR_H
#define SIMPLEANOMALYDETECTOR_H

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>

struct correlatedFeatures {
    string feature1,feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    float threshold;
    bool isRegCorrelated;
    Point* center;
    correlatedFeatures(string feat1, string feat2, float corr, Line linReg, float tHold, bool flagRegCorrlated = true, Point* c = nullptr) {
        this->feature1 = feat1;
        this->feature2 = feat2;
        this->corrlation = corr;
        this->lin_reg = linReg;
        this-> threshold = tHold;
        this->isRegCorrelated = flagRegCorrlated;
        this->center = c;
    }
};

class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
    float findThreshHold(vector<Point*> points, int size, Line regLine);
    int getFeatureColumn(const TimeSeries &ts, const string& feature);
protected:
    float simple_correlation = 0.9;
    vector<correlatedFeatures> cf;
    string getFeature(const TimeSeries &ts, int feature);
    vector<Point*> findPoints(vector<float> first, vector<float> second,int size);
    bool isPairExist(const string& feature1, const string& feature2);
    void freePoints(vector<Point*> points);
    virtual void checkPearson(float max_pearson, const TimeSeries &ts, int first, int second);
    void addCorrelatedFeatures(const TimeSeries &ts, int first, int second, float max_pearson);
    virtual bool isAnomaly(Point current, const correlatedFeatures &correlated);
public:
    SimpleAnomalyDetector();
    ~SimpleAnomalyDetector() override;
    void learnNormal(const TimeSeries& ts) override;
    vector<AnomalyReport> detect(const TimeSeries& ts) override;

    vector<correlatedFeatures> getNormalModel(){
        return cf;
    }
};

#endif /* SIMPLEANOMALYDETECTOR_H */
