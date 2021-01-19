// Amber Weiner 208783522

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <iostream>
#include <sstream>
#include <string.h>

#include <fstream>
#include <vector>
#include <unistd.h>
#include "HybridAnomalyDetector.h"
#include "timeseries.h"

using namespace std;

class DefaultIO{
public:
    virtual string read()=0;
    virtual void write(string text)=0;
    virtual void write(float f)=0;
    virtual void read(float* f)=0;
    virtual ~DefaultIO()= default;
};

//class StandardIO : public DefaultIO {
//public:
//    StandardIO() = default;
//    string read() override {
//        string input;
//        cin >> input;
//        return input;
//    }
//    void write(string text) override {
//        cout << text << endl;
//    }
//    void write(float f) override {
//        cout << f << endl;
//    }
//    void read(float* f) override {
//        cin >> *f;
//    }
//};

class SocketIO : public DefaultIO {
    int userID;
public:
    explicit SocketIO (int newUserId) {
        this->userID = newUserId;
    }
    string read() override {
        string input;
        char letter = ' ';
        int buffer = ::read(this->userID, &letter, sizeof(char));
        while (letter != '\n') {
            input += letter;
            buffer = ::read(this->userID, &letter, sizeof(char));
        }
        return input;
    }
    void write(string text) override{
        ::write(this->userID, text.c_str(), text.length());
    }
    void write(float f) override{
        string floatString = to_string(f);
        ::write(this->userID, floatString.c_str(), floatString.length());
    }
    void read(float* f) override{
        cin >> *f;
    }
};

// this struct holds shared data for Command objects
struct SharedData {
    TimeSeries *trainTS = nullptr;
    TimeSeries *testTS = nullptr;
    HybridAnomalyDetector* detector = nullptr;
    vector<AnomalyReport> anomalies;

//    ~SharedData()
//    {
//        delete this->trainTS;
//        delete this->testTS;
//        //delete this->detector;
//    }
};


class Command{
protected:
    DefaultIO* dio;
public:
    string description;
    struct SharedData* data;
    Command(DefaultIO* newDIO, SharedData* newData) : dio(newDIO), data(newData) {
    };
    virtual void execute()=0;
    virtual ~Command() = default;
};

// this command represent the 1st option in CLI menu
class UploadCSVFileCommand:public Command {
public:
    UploadCSVFileCommand(DefaultIO *newDIO, SharedData* newData) : Command(newDIO, newData) {
        this->description = "upload a time series csv file\n";
    }
    /**
    * This function upload a file using a given file name and it's type.
    * @param fileName - a given file name
    * @param type - the file's type
    */
    void upload(const string& fileName, const string& type) {
         // create a file
        ofstream file(fileName);

        string line = "";
        while (line != "done") {
            line = this->dio->read();
            // read the next line
            if (line != "done") {
                file << line << endl;
            }
        }
        // finish
        this->dio->write("Upload complete.\n");
        // check which kind of file is required
        if (type == "train")
            this->data->trainTS = new TimeSeries("train.csv");
        else
            this->data->testTS = new TimeSeries("test.csv");
        file.close();
    }

    void execute() override {
        // print the requested input from the client
        this->dio->write("Please upload your local train CSV file.\n");
        this->upload("train.csv", "train");
        // print the requested input from the client
        this->dio->write("Please upload your local test CSV file.\n");
        this->upload("test.csv", "test");
    }
};

// this command represent the 2nd option in CLI menu
class AlgorithmSettingsCommand:public Command {
public:
    AlgorithmSettingsCommand(DefaultIO* newDIO, SharedData* newData) : Command(newDIO, newData) {
        this->description = "algorithm settings\n";
    }
    void execute() override {
        float threshold = this->data->detector->getSimpleCorrelation();
        // print the requested input from the client
        string thresholdStr = to_string(threshold).substr(0,3);
        this->dio->write("The current correlation threshold is " + thresholdStr + '\n');
        bool isInValid = true;
        float newThreshold;
        while (isInValid) {
            this->dio->write("Type a new threshold\n");
            this->dio->read(&newThreshold);
            // check validity of the input threshold
            if ((newThreshold > 0.0) && (newThreshold < 1.0)) {
                isInValid = false;
            }
        }
        threshold = newThreshold;
        this->data->detector->setSimpleCorrelation(threshold);
    }
};

// this command represent the 4th option in CLI menu
class AnomalyDetectionCommand : public Command {
public:
    AnomalyDetectionCommand(DefaultIO *newDIO, SharedData* newData) : Command(newDIO, newData){
        this->description = "detect anomalies\n";
    }
    void execute() override {
        this->data->detector->learnNormal(*this->data->trainTS);
        this->data->anomalies = this->data->detector->detect(*this->data->testTS);
        this->dio->write("anomaly detection complete.\n");
    }
};

class DisplayResultCommand : public Command {
public:
    DisplayResultCommand(DefaultIO *newDIO, SharedData* newData) : Command(newDIO, newData) {
        this->description = "display results\n";
    }
    void execute() override {
        for (const AnomalyReport& ar: this->data->anomalies) {
            string timeStepStr = to_string(ar.timeStep);
            this->dio->write(timeStepStr + "\t" + " " + ar.description + "\n");
        }
        this->dio->write("Done.\n");
    }
};

// this command represent the 5th option in CLI menu
class AnaliseResultCommand : public Command {
public:
    AnaliseResultCommand(DefaultIO *newDIO, SharedData* newData) : Command(newDIO, newData) {
        this->description = "upload anomalies and analyze results\n";
    }

    /**
    * This function return a vector of pairs representing start and end timeStep of
    * each reported anomaly.
    * @return a vector of pairs of long representing timStep of anomalies.
    */
    vector<pair<long, long>> getAnomaliesOccurrences() {
        vector<pair<long, long>> result;
        long start, end;
        string line, value;
        // read the anomalies file
        while (line != "done") {
            line = this->dio->read();
            stringstream s(line);
            if (line != "done") {
                vector<string> timeSteps;
                // get the anomaly's start and end times value
                while (getline(s, value, ',')) {
                    timeSteps.push_back(value);
                }
                start = stol(timeSteps.front());
                end = stol(timeSteps.back());
                result.emplace_back(start, end);
                timeSteps.clear();
            }
        }
        return result;
    }

    /**
    * This function return a vector of pairs representing start and end timeStep of
     * each reported anomaly.
     * This vector is with no repetition.
    * @return a non repetition vector of pairs of long representing timStep of anomalies.
    */
    vector<pair<long, long>> getAnomaliesNoRepetition() {
        vector<pair<long, long>> result;
        int i = 0;
        int size = data->anomalies.size();
        // go over all the anomalies
        while (i < size) {
            long start = this->data->anomalies[i].timeStep;
            long end = this->data->anomalies[i].timeStep;
            i++;
            // find consecutive occurrences of the same anomaly
            while ((data->anomalies[i-1].description == this->data->anomalies[i].description) &&
                   (data->anomalies[i].timeStep == end + 1))
            {
                end++;
                // check if reached to the end of the vector
                if (i == size - 1)
                    break;
                i++;
            }
            result.emplace_back(start, end);
            i++;
        }
        return result;
    }
    void execute() override {
        this->dio->write("Please upload your local anomalies file.\n");
        vector<pair<long, long>> anomaliesOccurrences = this->getAnomaliesOccurrences();
        this->dio->write("Upload complete.\n");
        vector<pair<long, long>> anomaliesNoRepetition = this->getAnomaliesNoRepetition();
        //calculate p and n:
        float P = anomaliesOccurrences.size(); // amount of anomalies in the file
        int totalLines = data->testTS->get_num_of_rows();
        int rowsOfAnomalies = 0;
        for (pair<long, long> pair: anomaliesOccurrences) {
            int end = int (pair.second);
            int start = int (pair.first);
            rowsOfAnomalies += (end - start + 1);
        }
        int N = totalLines - rowsOfAnomalies; // amount of timeStep where an anomaly was not found
        float TP = 0, FP = 0;
        bool flag;

        // a loop to go over each instance in anomaliesOccurrences
        // and check if they overlap with anomaliesNoRepetition.
        for (pair<long, long> singleOccurrence: anomaliesNoRepetition) {
            flag = false; // there is no overlap
            for (pair<long, long> anomalyOccurrence: anomaliesOccurrences) {
                    //option1 - left side of a is in range b
                if (singleOccurrence.first >= anomalyOccurrence.first &&
                         singleOccurrence.first <= anomalyOccurrence.second) {
                    flag = true;
                    break;
                } //option2 - right side of a is in range b
                else if (singleOccurrence.second <= anomalyOccurrence.second &&
                         singleOccurrence.second >= anomalyOccurrence.first) {
                    flag = true;
                    break;
                } //option3 - b is in range a
                else if (singleOccurrence.first <= anomalyOccurrence.first &&
                         singleOccurrence.second >= anomalyOccurrence.second) {
                    flag = true;
                    break;
                }
                    //option4 - a is in range b
                else if (singleOccurrence.first >= anomalyOccurrence.first &&
                         singleOccurrence.second <= anomalyOccurrence.second) {
                    flag = true;
                    break;
                }
            }
            // check TP/FP
            if (flag) {
                TP++;
            } else
                FP++;
        }
        float TPrate = TP / P;
        TPrate = TPrate * 1000;
        TPrate = floor(TPrate);
        TPrate = TPrate/1000;
        float FPrate = FP / float(N);
        FPrate = FPrate * 1000;
        FPrate = floor(FPrate);
        FPrate = FPrate/1000;
        dio->write("True Positive Rate: ");
        this->dio->write(TPrate);
        this->dio->write("\n");
        dio->write("False Positive Rate: ");
        this->dio->write(FPrate);
        this->dio->write("\n");
    };
};

// this command represent the 6th option in CLI menu
class ExitCommand : public Command {
public:
    ExitCommand(DefaultIO *newDIO, SharedData* newData) : Command(newDIO, newData) {
        this->description = "exit\n";
    }

    void execute() override {
        delete this->data->trainTS;
        delete this->data->testTS;
    }
};

#endif /* COMMANDS_H_ */