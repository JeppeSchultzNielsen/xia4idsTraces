//
// Created by jeppe on 3/19/24.
//
#include "xia4ids.hh"

void xia4idsRunner::define_ausa() {
    tree = new TTree("h101", "h101");

    tree->Branch("TPROTONS", &TIME_REF_branch);
    tree->Branch("TSTAMP", &TIME_RUN_branch);

    //saving the comment line for naming ROOT branches
    for (i=0; i<detnum; i++)
        if (comment_string[i][0] != '\0' )
            sprintf(root_string[config_coding[i][1]], "%s", comment_string[i]);
        else if (root_string[config_coding[i][1]][0] == '\0')
            sprintf(root_string[config_coding[i][1]], "%d", config_coding[i][1]);

    //initialize the detectorBranches
    for (i = 0; i < dettypes+1; i++) {
        //cout << "i: " << i << " " << root_string[i] <<  endl;
        detectorMuls[i] = 0;
        tree -> Branch(root_string[i],&detectorMuls[i]);
        DET_E[i] = make_unique<AUSA::DynamicBranchVector<UInt_t>>(*tree, Form("%s_E", root_string[i]), root_string[i]);
        DET_T[i] = make_unique<AUSA::DynamicBranchVector<UInt_t>>(*tree, Form("%s_T", root_string[i]), root_string[i]);
        DET_I[i] = make_unique<AUSA::DynamicBranchVector<UInt_t>>(*tree, Form("%sI", root_string[i]), root_string[i]);
    }

    //define histograms for each input defined in the config file
    //they are incremented in read_ldf.hh

    hStats = new TH1F("hStats", "Statistics", detnum, 0, detnum);
    for (i = 0; i < detnum; i++)
        h[i] = new TH1F(Form("h%d_%s", i, comment_string[i]),
                        Form("%s_Singles", comment_string[i]),
                        65535, 1, 65536);

}