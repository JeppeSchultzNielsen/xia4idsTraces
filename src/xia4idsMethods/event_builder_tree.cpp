
/*

 HOW does the ROOT event builder work:


 1. The detectors are defined in the config file. Numbering starts from 1
 2. Each distinct type represents a branch in the tree. The index corresponds to the array index (each branch is an array)
 3. For ADDBACK there can be several detectors of the same type and index.
 They should be calibrated because all go in the same position.
 4. There will be at least 'fold' detectors in the event. Each individual index-type combination represents a detector.
 5. Note: in ROOT numbering starts from 0.

 There are the following branches in the tree:

 E_type1[index] - ADC data
 E_type2[]
 .
 .
 T_type1[index] - (HRT) timestamp difference between each branch and the beginning of the event with offset = 1000
 T_type2[]
 .
 .
 M_type1        - multiplicity for each branch
 M_type2
 .
 .
 MULT     - multiplicity of the event (sum of multiplicity of each branch)
 TIME     - (LRT) timestamp difference between the event and the reference - if defined (proton pulse)
 TIME_RUN - (LRT) timestamp in run_unit units (always)
 */

#include "xia4ids.hh"



void xia4idsRunner::event_builder_tree() {



    k=0; //the index of the data array 0<k<iData
    int type=0, index=0, mult=0, e=0;
    double evt_start=0, lrt_ref=0, lrt_run=0;

    double hrt[detnum+1];
    for(i=0; i<=detnum; i++)  hrt[i]=0;


    int detcount[dettypes+1];
    double energy[dettypes+1][detnum+1];
    double traceIntegral[dettypes+1][detnum+1];
    std::vector<unsigned int> trace[dettypes+1][detnum+1];

    bool isCoin = false;
    int noBetaHits = 0;
    int noIndieHits = 0;


    while ( k < iData) {
        m=1;
        evt_start=0;
        mult=0;

        for(i=1; i<=dettypes; i++) {
            for (j=1; j<=maxnum[i]; j++){
                energy[i][j]=0;
                traceIntegral[i][j]=0;
                if(savetraces){
                    trace[i][j].clear();
                }
            }
            detcount[i]=0;
        }


        //if we encounter a reference ADC
        if (reftype != 0 && tmc[DataArray[k].modnum][DataArray[k].chnum] == reftype) {
            tref = DataArray[k].time;
            k++;
        }


        //finding clusters of data
        //m = number of signals inside the cluster
        while ( m <= detnum && k+m < iData && DataArray[k+m].time - DataArray[k].time < timegate ) {
            m++;
            //if ref is inside an event we need to update the information about it
            if (reftype !=0 && tmc[DataArray[k+m].modnum][DataArray[k+m].chnum] == reftype) {
                tref = DataArray[k+m].time;
            }
        }



        //to be an INDIE TOF coincidence, there needs to be atleast 2 beta signals and 2 indie signals
        isCoin = false;
        noBetaHits = 0;
        noIndieHits = 0;

        //checking to see what is inside the event
        for(n=0; n<m; n++) {
            type = tmc[DataArray[k+n].modnum][DataArray[k+n].chnum];
            //looking for detector signals in order to calculate HRT
            //we do not calculate hrt for reftype
            if (   type  > 0  && type != reftype   )  {
                detcount[type]++;
                if(onlyCoin){
                    if(typeNames[type] == "Beta")
                        noBetaHits++;
                    if(typeNames[type] == "INDiE")
                        noIndieHits++;
                }
                e++;
                if(evt_start == 0)
                    evt_start = DataArray[k+n].time;
            }
        }

        if(onlyCoin){
            if(noBetaHits >= 2 && noIndieHits >= 2){
                isCoin = true;
            }
        }


        ////// Checking the multiplicity

        //there will be at least 'mult' detectors in the event
        for (i=1; i<=dettypes; i++)
            if (i != reftype) mult+=detcount[i];
        if (mult < fold) {
            k++;
            continue;
        }

        //extracting event information
        for (n=0; n<m; n++) {
            type =  tmc      [DataArray[k+n].modnum][DataArray[k+n].chnum];
            index = ntmc[type][DataArray[k+n].modnum][DataArray[k+n].chnum];
            if (   type > 0  /*&& index != reftype*/ )  {
                hrt[n] = 1000 + DataArray[k+n].time - evt_start ;
                energy[type][index] += DataArray[k+n].energy;
                traceIntegral[type][index] += DataArray[k+n].traceIntegral;
                double phase = 0;
                if( (onlyCoin && isCoin) or !onlyCoin ){
                    if(savetraces) {
                        trace[type][index] = DataArray[k+n].trace;
                    }
                    //cout << "type: " << type << " index: " << index << " energy: " << DataArray[k+n].energy << " time: " << DataArray[k+n].time << " cfdtime: " << DataArray[k+n].cfdtime << " evt_start: " << evt_start << endl;

                    /*if(DataArray[k+n].trace.size() > 0){
                        if(dig_daq_params[DataArray[k+n].modnum][DataArray[k+n].chnum]->isReady){
                            Trace *trace = new Trace(DataArray[k+n].trace);
                            trace -> findTraceParams();
                            trace -> subtractBaseline();
                            if(trace -> minGreaterThanMax){
                                hrt[n] = 1000 + DataArray[k+n].time - evt_start + phase; //if we use tracefitting we should not use the cfdtime.
                                traceIntegral[type][index] = -1000;
                            }
                            else{
                                if(dig_daq_params[DataArray[k+n].modnum][DataArray[k+n].chnum] -> detType == "INDiE"){
                                    //i believe that the trace starts at the "filter time", so simply adding the found phase should give the correct time now?
                                    auto phaseAlpha = static_cast<DigDaqParamINDiE*>(dig_daq_params[DataArray[k+n].modnum][DataArray[k+n].chnum])->calculatePhase(trace);
                                    phase = phaseAlpha.first;
                                    //cout << phase << endl ;
                                    hrt[n] = 1000 + DataArray[k+n].time - evt_start + phase; //if we use tracefitting we should not use the cfdtime.
                                    traceIntegral[type][index] = trace->qdc;
                                }
                                if(dig_daq_params[DataArray[k+n].modnum][DataArray[k+n].chnum] -> detType == "Beta"){
                                    auto phaseAlpha = static_cast<DigDaqParamBeta*>(dig_daq_params[DataArray[k+n].modnum][DataArray[k+n].chnum])->calculatePhase(trace);
                                    phase = phaseAlpha.first;
                                    //cout << phase << endl ;
                                    hrt[n] = 1000 + DataArray[k+n].time - evt_start + phase; //if we use tracefitting we should not use the cfdtime.
                                    traceIntegral[type][index] = trace->qdc;
                                }
                            }
                            trace->clear();
                            delete trace;
                        }
                    }*/
                }
                std::vector<unsigned int>().swap(DataArray[k+n].trace);
                //must release the memory from the trace -- no better way to do this?
            }
        }

        // LRT: low resolution time
        //TIMESTAMP - always record this
        lrt_run = 1 + DataArray[k].time/run_unit;

        //Predefined reference (proton pulse)
        if (reftype == 0)  // we don't define any reference (proton)
            lrt_ref = 0;

        else if (reftype > 0 && tref == 0) //signals at the beginning of data for which we don't have reference information
            lrt_ref = 30000;
        else  //signals for which we have ref
            lrt_ref = (DataArray[k].time-tref)/ref_unit;

        MULT_branch = mult;
        TIME_REF_branch = lrt_ref;
        TIME_RUN_branch = lrt_run;

        for(n=0; n<m; n++) {
            type =  tmc      [DataArray[k+n].modnum][DataArray[k+n].chnum];
            index = ntmc[type][DataArray[k+n].modnum][DataArray[k+n].chnum];

            E_branch[type][index-1] = energy[type][index];
            TI_branch[type][index-1] = traceIntegral[type][index];
            T_branch[type][index-1] = hrt[n];
            F_branch[type][index-1] = DataArray[k+n].flag;
            if(savetraces){
                for(int i = 0; i < trace[type][index].size(); i++){
                    TRACE_branch[type][index-1][i] = trace[type][index][i];
                }
                TRACELEN_branch[type][index-1] = trace[type][index].size();
            }
            if(savetraces){
                for(int i = 0; i < trace[type][index].size(); i++){
                    std::vector<unsigned int>().swap(trace[type][index]);
                    //must release the memory from the trace -- no better way to do this?
                }
            }


            M_branch[type] = detcount[type];
            hrt[n] = 0;

            //~}

        }
        tree->Fill();

        MULT_branch = 0;
        TIME_REF_branch = 0.0;
        TIME_RUN_branch = 0.0;
        memset(E_branch, 0, sizeof(E_branch));
        memset(TI_branch, 0, sizeof(TI_branch));
        memset(T_branch, 0, sizeof(T_branch));
        memset(M_branch, 0, sizeof(M_branch));
        memset(F_branch, 0, sizeof(F_branch));
        if(savetraces){
            memset(TRACE_branch, 0, sizeof(TRACE_branch));
            memset(TRACELEN_branch, 0, sizeof(TRACELEN_branch));
            memset(trace, 0, sizeof(trace));
        }

        iEvt++;
        k+=m;
    }
} //end of event_builder_tree.h

































