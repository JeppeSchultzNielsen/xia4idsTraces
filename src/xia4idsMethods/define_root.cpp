#include "xia4ids.hh"

void Xia4idsRunner::define_root() {



    tree = new TTree("ids", "Isolde Decay Station Tree");

    tree->Branch("Multiplicity", &MULT_branch, "MULT/I");
    tree->Branch("Time_vs_ref", &TIME_REF_branch, "TIME_REF/l");
    tree->Branch("Timestamp", &TIME_RUN_branch, "TIMESTAMP/l"); //l : a 64 bit unsigned integer (ULong64_t)



    //saving the comment line for naming ROOT branches
    for (i=0; i<detnum; i++)
        if (comment_string[i][0] != '\0' )
            sprintf(root_string[config_coding[i][1]], "%s", comment_string[i]);
        else if (root_string[config_coding[i][1]][0] == '\0')
            sprintf(root_string[config_coding[i][1]], "%d", config_coding[i][1]);



    //Array for each branch
    for (i=1; i<=dettypes; i++) {
//    tree->Branch(Form("Energy_%s", root_string[i]), E_branch[i], Form("E_%s[%d]/I",root_string[i],maxnum[i]));
        tree->Branch(Form("Energy_%s", root_string[i]), E_branch[i], Form("E_%s[%d]/D",root_string[i],maxnum[i]));
        tree->Branch(Form("TraceIntegral_%s", root_string[i]), TI_branch[i], Form("TI_%s[%d]/D",root_string[i],maxnum[i]));
        tree->Branch(Form(  "Time_%s", root_string[i]), T_branch[i], Form("T_%s[%d]/D",root_string[i],maxnum[i]));
        tree->Branch(Form(  "Mult_%s", root_string[i]),&M_branch[i], Form("M_%s/I"    ,root_string[i]            ));
        tree->Branch(Form(  "Flag_%s", root_string[i]),&F_branch[i], Form("F_%s[%d]/I" ,root_string[i],maxnum[i]));
        if(savetraces){
            tree->Branch(Form("TraceLen_%s", root_string[i]), TRACELEN_branch[i], Form("TL_%s[%d]/I",root_string[i],maxnum[i]));
            tree->Branch(Form("Trace_%s", root_string[i]), TRACE_branch[i], Form("Trace_%s[%d][130]",root_string[i],maxnum[i]));
        }
    }



    //define histograms for each input defined in the config file
    //they are incremented in read_ldf.hh
/*
    hStats = new TH1F("hStats", "Statistics", detnum, 0, detnum);
    for (i = 0; i < detnum; i++)
        h[i] = new TH1F(Form("h%d_%s", i, comment_string[i]),
                        Form("%s_Singles", comment_string[i]),
                        65535, 1, 65536);
*/



}
