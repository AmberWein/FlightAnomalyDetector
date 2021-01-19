// Amber Weiner 208783522

#include "CLI.h"

CLI::CLI(DefaultIO *dio) {
    this->dio = dio;
    this->data = new SharedData();
    this->data->detector = new HybridAnomalyDetector();
    this->welcomeMsg = "Welcome to the Anomaly Detection Server.\n";
    this->cont = "Please choose an option:\n";
}

void CLI::start() {
    this->commands.push_back(new UploadCSVFileCommand(this->dio, this->data));
    this->commands.push_back(new AlgorithmSettingsCommand(this->dio, this->data));
    this->commands.push_back(new AnomalyDetectionCommand(this->dio, this->data));
    this->commands.push_back(new DisplayResultCommand(this->dio, this->data));
    this->commands.push_back(new AnaliseResultCommand(this->dio, this->data));
    this->commands.push_back(new ExitCommand(this->dio, this->data));

    string optionSelectedStr;
    int optionSelected = 0;
    while (optionSelected < 5) {
        this->dio->write(this->welcomeMsg);
        this->dio->write(this->cont);
        // complete the opening menu, write the commands description
        for (int i = 0; i < commands.size(); i++) {
            string optNum = to_string(i+1);
            this->dio->write(optNum + ".");
            this->dio->write(this->commands.at(i)->description);
        }
        // get the number of the required command
        optionSelectedStr = this->dio->read();
        optionSelected = stoi(optionSelectedStr);
        optionSelected--;
        // activate the required command
            this->commands.at(optionSelected)->execute();
    }

    // delete all the new commands
    for (int i = 0; i < commands.size(); i++)
    delete this->commands.at(i);
}

CLI::~CLI() {
    delete this->data->detector;
    delete this->data;
}