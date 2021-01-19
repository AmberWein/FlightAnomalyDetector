// Amber Weiner 208783522

#ifndef CLI_H
#define CLI_H

#include <string.h>
#include "commands.h"

class CLI {
    DefaultIO* dio;
    SharedData* data;
    string welcomeMsg;
    string cont;
    vector<Command*> commands;
    //HybridAnomalyDetector* detector;
public:
    explicit CLI(DefaultIO* dio);
    void start();
    virtual ~CLI();
};

#endif /* CLI_H_ */
