#include "xia4ids.hh"



void Xia4idsRunner::event_builder_ausa() {



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
                energy[type][index] = DataArray[k+n].energy;
                traceIntegral[type][index] = DataArray[k+n].traceIntegral;
                double phase = 0;
                if( (onlyCoin && isCoin) or !onlyCoin ){
                    if(savetraces) {
                        trace[type][index] = DataArray[k+n].trace;
                    }
                    //cout << "type: " << type << " index: " << index << " energy: " << DataArray[k+n].energy << " time: " << DataArray[k+n].time << " cfdtime: " << DataArray[k+n].cfdtime << " evt_start: " << evt_start << endl;

                    if(DataArray[k+n].trace.size() > 0){
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
                    }
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

            if(detectorMulsCheck[type][index] > 0){
                //too many hits within time window - assume pileup
                int flag = 100;
                continue;
            }
            detectorMuls[type]++;
            detectorMulsCheck[type][index]++;
            DET_E[type] -> add((UInt_t)energy[type][index]);
            DET_T[type] -> add((UInt_t)4000*hrt[n]);
            DET_I[type] -> add((UInt_t)index);
        }
        tree->Fill();

        MULT_branch = 0;
        TIME_REF_branch = 0.0;
        TIME_RUN_branch = 0.0;
        memset(detectorMuls, 0, sizeof(detectorMuls));
        memset(detectorMulsCheck, 0, sizeof(detectorMulsCheck));
        for(int i = 0; i < dettypes; i++){
            AUSA::clear(*DET_E[i], *DET_T[i],*DET_I[i]);
        }
        iEvt++;
        k+=m;
    }
} //end of event_builder_tree.h