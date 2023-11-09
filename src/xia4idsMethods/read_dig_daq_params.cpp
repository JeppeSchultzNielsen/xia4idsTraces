#include "xia4ids.hh"

void xia4idsRunner::read_dig_daq_params(int argc, char **argv) {

    int status, module, channel, count=0;
    double cal[MAX_CAL];
    FILE *dig_daq_file;
    char line[10000];

    //initialize dig_daq_params
    for(int i = 0; i < MAX_NUM_MOD; i++){
        for(int j = 0; j < MAX_NUM_CHN; j++){
            dig_daq_params[i][j] = new DigDaqParam();
        }
    }

    if ( rate == 0 && (argc < 3 || !fopen(argv[3], "r")) ) {
        printf("No calibrations will be used: ...$xia4ids [config_file_name] [cal_file_name] [dig_daq_param_file]\n");
        return;
    }
    else if ( rate == 1 && (argc < 4 || !fopen(argv[4], "r")) ) {
        printf("No calibrations will be used: ...$xia4ids [config_file_name] [input_file] [cal_file] [dig_daq_param_file]\n");
        return;
    }
    else if (rate == 0 ) dig_daq_file = fopen(argv[3], "r");
    else if (rate == 1 ) dig_daq_file = fopen(argv[4], "r");

    while( fgets(line, 10000, dig_daq_file) != NULL ) {
        //split the line by whitespace
        vector<string> splitted = {};
        std::istringstream iss(line);

        std::string s;
        while (iss >> s) {
            splitted.push_back(s);
        }
        int module;
        int channel;
        string detType;
        if(splitted.size() >= 4){
            module = stoi(splitted[1]);
            channel = stoi(splitted[2]);
            detType = splitted[3];
        }
        else{
            printf("File with digital DAQ parameters is malformed, specifically line \n");
            cout << line << endl;
            return;
        }
        if(detType == "INDiE"){
            vector<double> betaParams = {};
            vector<double> gammaParams = {};
            bool freeBetaGamma = false;
            if(splitted.size() >= 6){
                if(splitted[4].find("-beta=") != std::string::npos){
                    string betaString = splitted[4].substr(6);
                    vector<string> betaStringSplit = {};
                    std::istringstream iss(betaString);

                    std::string s;
                    //split string by comma
                    while (getline(iss, s, ',')) {
                        betaStringSplit.push_back(s);
                    }
                    for(int i = 0; i < betaStringSplit.size(); i++){
                        betaParams.push_back(stod(betaStringSplit[i]));
                    }
                }
                else{
                    printf("Need beta and gamma parameters for INDiE detectors. \n");
                    return;
                }
                if(splitted[5].find("-gamma=") != std::string::npos){
                    string gammaString = splitted[5].substr(7);
                    vector<string> gammaStringSplit = {};
                    std::istringstream iss(gammaString);

                    std::string s;
                    //split string by comma
                    while (getline(iss, s, ',')) {
                        gammaStringSplit.push_back(s);
                    }
                    for(int i = 0; i < gammaStringSplit.size(); i++){
                        gammaParams.push_back(stod(gammaStringSplit[i]));
                    }
                }
                if(splitted[5].find("-freebetagamma=true") != std::string::npos){
                    freeBetaGamma = true;
                }
            }
            else{
                printf("Need beta and gamma parameters for INDiE detectors. \n");
                return;
            }
            delete dig_daq_params[module][channel];
            dig_daq_params[module][channel] = new DigDaqParamINDiE(module, channel, detType, betaParams, gammaParams, freeBetaGamma);
        }
        else if(detType == "Beta"){
            vector<double> betaParams = {};
            vector<double> gammaParams = {};
            bool freeBetaGamma = false;
            if(splitted.size() >= 6){
                if(splitted[4].find("-beta=") != std::string::npos){
                    string betaString = splitted[4].substr(6);
                    vector<string> betaStringSplit = {};
                    std::istringstream iss(betaString);

                    std::string s;
                    //split string by comma
                    while (getline(iss, s, ',')) {
                        betaStringSplit.push_back(s);
                    }
                    for(int i = 0; i < betaStringSplit.size(); i++){
                        betaParams.push_back(stod(betaStringSplit[i]));
                    }
                }
                else{
                    printf("Need beta and gamma parameters for beta detectors. \n");
                    return;
                }
                if(splitted[5].find("-gamma=") != std::string::npos){
                    string gammaString = splitted[5].substr(7);
                    vector<string> gammaStringSplit = {};
                    std::istringstream iss(gammaString);

                    std::string s;
                    //split string by comma
                    while (getline(iss, s, ',')) {
                        gammaStringSplit.push_back(s);
                    }
                    for(int i = 0; i < gammaStringSplit.size(); i++){
                        gammaParams.push_back(stod(gammaStringSplit[i]));
                    }
                }
                if(splitted[5].find("-freebetagamma=true") != std::string::npos){
                    freeBetaGamma = true;
                }
            }
            else{
                printf("Need beta and gamma parameters for INDiE detectors. \n");
                return;
            }
            delete dig_daq_params[module][channel];
            dig_daq_params[module][channel] = new DigDaqParamBeta(module, channel, detType, betaParams, gammaParams, freeBetaGamma);
        }
        else{
            cout << "Detector type " << detType << " not supported" << endl;
        }
    }
}