// Amber Weiner 208783522

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
    float hybrid_correlation = 0.5;
    bool isInCircle(Point current, const correlatedFeatures &correlated);
    bool isAnomaly(Point current, const correlatedFeatures &correlated) override;
    void checkPearson(float max_pearson, const TimeSeries &ts, int first, int second) override;
public:
	HybridAnomalyDetector();
	~HybridAnomalyDetector() override;
    float getSimpleCorrelation();
    void setSimpleCorrelation(float newCorrelation);

};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
