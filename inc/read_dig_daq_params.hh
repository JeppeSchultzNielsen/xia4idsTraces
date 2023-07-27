void read_dig_daq_params(int argc, char **argv) {

    int status, module, channel, count=0;
    double cal[MAX_CAL];
    FILE *dig_daq_file;
    char line[10000];

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
        cout << line << endl;
    }
}