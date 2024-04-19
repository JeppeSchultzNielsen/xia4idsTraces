/*
XIA DGF Pixie-16 .ldf to GASPware and ROOT converter

Authors:
R. Lica, IFIN-HH - CERN, razvan.lica@cern.ch 2020-2021 
K. Phan, TUNI - CERN Summer student 2020 (.ldf readout integration from PAAAS)

https://github.com/rlica/xia4ids

*/

#include "xia4ids.hh"

int Xia4idsRunner::xia4ids(int argc, char **argv, int lastRead)
{

    printf("\n\t\t----------------------------------");
    printf("\n\t\t    XIA DGF Pixie-16 Converter");
    printf("\n\t\t           v04.2021  ");
    printf("\n\t\t https://github.com/rlica/xia4ids");
    printf("\n\t\t----------------------------------");
    printf("\n\n");

    read_config(argc, argv, true);

    read_cal(argc, argv);
    read_dig_daq_params(argc, argv);

    //Allocating memory
    DataArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));
    TempArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));

    if (gasp == 1 || list == 1) {
		EventArray = (struct Event *)calloc(memoryuse, sizeof(struct Event));
		GHeader = (struct GaspRecHeader *)calloc(1, sizeof(struct GaspRecHeader));
    }

    // Reading run by run
    for (runnumber = runstart; runnumber <= runstop; runnumber++)
    {
		raw_list_size = 0, good_list_size = 0;
        totEvt = 0;
        tref = 0;
        run_good_chunks = 0;
        run_missing_chunks = 0;
        
        bool first_cycle = true; // to keep track of total run time

        // Open output file
        if (corr == 0) {

            //GASPware format
            if (gasp == 1)
            {
                sprintf(outname, "Run%03d", runnumber);
                fp_out = fopen(outname, "wt");
                if (!fp_out)
                {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
                    exit(0);
                }
            }

            //Event List format
            else if (list == 1)
            {
                sprintf(outname, "Run%03d.list", runnumber);
                fp_out = fopen(outname, "wt");
                if (!fp_out)
                {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
                    exit(0);
                }
            }

            //ROOT format
            else if (root == 1 || stat == 1 || ausa == 1) {
                strcpy(outname, outputName);
                sprintf(outname, strcat(outname,"%03d.root"), runnumber);
                cout << "saving to " << outname << endl;

                rootfile = TFile::Open(outname, "recreate");
                if (!rootfile) {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
                    exit(0);
                }
                if(root == 1){
                    define_root();
                }
                else if(ausa == 1){
                    define_ausa();
                }
            }
        }    
        

			
		// Cycling over run partitions (a large run will be split into several files of 2.0 Gb each -> one run partition = one file)
		for (runpart = 0; runpart < 1000; runpart++) {

			start_clock = (double)clock();
               
			//Ratemeter mode, analysing only the last RATE_EOF_MB megabytes from a file
			if (rate == 1) {
				if (argc < 3) { //Rate mode takes the input file as the second argument
					printf("Config file and input file required as arguments: ...$xia4ids config_file_name input file [calibration] \n");
					exit(0);
				}
				sprintf(filename, "%s", argv[2]);
				fp_in = fopen(argv[2], "rb");
				if (!fp_in) {
					printf("ERROR: Unable to open %s \n", filename);
					exit(0);
				}
				if (runpart > 0) break;
			}				
               
			// Normal mode analysing the full file
			if (rate == 0) {
				if (runpart == 0)
					sprintf(filename, "%s%03d.ldf", runname, runnumber);
				else
					sprintf(filename, "%s%03d-%d.ldf", runname, runnumber, runpart);
				fp_in = fopen(filename, "rb");
				if (!fp_in) {
					printf("File parsing completed: %s does not exist\n", filename);
					break;
				}
			}
			
			
			//Initializing the binary file object
			LDF_file ldf(filename);
			DATA_buffer data;
			int ldf_pos_index = 0;
			float progress = 0.0;

			// Set file length
			ldf.GetFile().open(ldf.GetName().c_str(), std::ios::binary);
			ldf.GetFile().seekg(0, ldf.GetFile().end);
			ldf.SetLength(ldf.GetFile().tellg());
			ldf.GetFile().seekg(0, ldf.GetFile().beg);
			ldf.GetFile().close();
			printf("Filename:  %s \nFile size: %.2f MB \n", filename, float(ldf.GetFileLength())/1048576);


			// Start of a reading cycle:
			while (data.GetRetval() != 2 && ldf_pos_index <= ldf.GetFileLength()) {// ldf_pos_index <= ldf.GetFileLength()) {

				// iData will be the last data index.
				iData=0, iEvt=0;
                   
				// Initializing the data array to zero
				memset(DataArray,0,memoryuse);  
				memset(TempArray,0,memoryuse); //Used only when sorting
				
				// Displaying the progress bar
				progress = float(ldf_pos_index) / float(ldf.GetFileLength());
				printProgress(progress);
									
				// read_ldf() will readfa fixed number of spills at once from the binary file
				// if ratemode is enabled, read_ldf() will process only the last part of the file
                old_iData = iData;
				raw_list_size  += read_ldf(ldf, data, ldf_pos_index);
				good_list_size += iData;
				if (raw_list_size == 0)	continue;

                /*for(int v = old_iData; v < iData; v++){
                    //clean traces from memory
                    std::vector<unsigned int>().swap(DataArray[v].trace);
                }*/

				// Sorting the data chronologically.
				MergeSort(DataArray, TempArray, 0, iData);

				// Extract first and last time stamps 
				if (first_cycle) { 
					if (DataArray[1].time > 0)
						first_ts = DataArray[1].time;
					else
						printf("ERROR: Cannot read first timestamp \n");
					first_cycle = false;
				}                    
				if (DataArray[iData-1].time > 0)
					last_ts = DataArray[iData-1].time;
								
				// Writing statistics and skipping the event builder, will sort everything twice as fast
				if (stat == 1) continue;
				                       
				// Looking for correlations
				if (corr > 0) 
					correlations();

				// Writing to GASPWare
				else if (gasp == 1) {
					event_builder();
					write_gasp();
					totEvt += iEvt;
					printf(" %3d events written to %s ", totEvt, outname);
					write_time(ldf_pos_index, ldf.GetFileLength());
				}

				// Writing event lists
				else if (list == 1) {
					event_builder_list();
					write_list();
					totEvt += iEvt;
					printf(" %3d events written to %s ", totEvt, outname);
					write_time(ldf_pos_index, ldf.GetFileLength());
				}

				// Writing to a ROOT Tree
				else if (root == 1) {
                    event_builder_tree();
					totEvt += iEvt;
					printf(" %3d events written to %s ", totEvt, outname);
					write_time(ldf_pos_index, ldf.GetFileLength());

				}

                else if (ausa == 1) {
                    event_builder_ausa();
                    totEvt += iEvt;
                    printf(" %3d events written to %s ", totEvt, outname);
                    write_time(ldf_pos_index, ldf.GetFileLength());

                }

                for(int v = old_iData; v < iData; v++){
                    //clean traces from memory
                    std::vector<unsigned int>().swap(DataArray[v].trace);
                    std::vector<unsigned int>().swap(TempArray[v].trace);
                }
                                      
				// We break this loop after the entire file is read and parsed.
				if (data.GetRetval() == 2) { // last cycle.
					
					std::cout << std::endl;
					// std::cout << "First time stamp: " << first_ts << "\t Last time stamp: " << last_ts << std::endl;
					std::cout << "Finished reading complete file" << std::endl; 
                     
					// Checking file integrity 
					run_good_chunks += data.GetNumChunks(); 
					run_missing_chunks += data.GetNumMissing();
                                                     
                    break;
				}

                //cout << ldf_pos_index << endl;

				// We also break this loop when reaching the initially read file length. 
				// This allows for reading out incomplete files or files that are currently being written.
				if (ldf_pos_index > ldf.GetFileLength()) {
					
                    std::cout << std::endl;
                    // std::cout << "First time stamp: " << first_ts << "\t Last time stamp: " << last_ts << std::endl;
                    std::cout << "Finished reading incomplete file" << std::endl; 
                    break;
				}

				fflush(stdout);
				
			} // End of cycling over a partition
		
		} // End of cycling over all Run partitions

        // Printing statistics for each run if not in correlation mode
        // Writing the root file to disk
        if (corr == 0) {
            write_stats();
            memset(stats, 0, sizeof(stats));
			if (root == 1 || stat == 1 || ausa == 1) {
				rootfile->Write();
				rootfile->Save();
				rootfile->Close();
			}
        }  

		// Writing correlation statistics for each run to file
		if (corr > 0)
			write_correlations();
        
        std::cout << "Sorting completed!" << std::endl;
    
    } // end of all Runs

    free(DataArray);
    free(TempArray);
    exit(0);

} //end of main

Xia4idsRunner::Xia4idsRunner(int argc, char **argv){
    //initialize a xia4idsRunner object that is ready to read spills
    read_config(argc, argv, false);
    read_cal(argc, argv);
    read_dig_daq_params(argc, argv);

    //Allocating memory
    DataArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));
    TempArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));

    memset(DataArray,0,memoryuse);
    memset(TempArray,0,memoryuse); //Used only when sorting

    if(root == 1){
        define_root();
    }
    else if(ausa == 1){
        define_ausa();
    }
}

void Xia4idsRunner::prepareFile(int runNumber, int fileNumber, BufferInfo bufferInfo, string fileDestinationStem){
    // Normal mode analysing the full file
    if (rate == 0) {
        if (fileNumber == 0)
            sprintf(filename, "%s%03d.ldf", runname, runNumber);
        else
            sprintf(filename, "%s%03d-%d.ldf", runname, runNumber, fileNumber);
        fp_in = fopen(filename, "rb");
        if (!fp_in) {
            printf("File parsing completed: %s does not exist\n", filename);
        }
    }

    if (root == 1 || stat == 1 || ausa == 1) {
        string outputName = fileDestinationStem + Form("%03d_%03d_%d.root", runNumber, fileNumber, bufferInfo.spillEnd);

        rootfile = TFile::Open(outputName.c_str(), "recreate");
        if (!rootfile) {
            fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
            exit(0);
        }
        if(root == 1){
            define_root();
        }
        else if(ausa == 1){
            define_ausa();
        }
    }
}

int Xia4idsRunner::readSpill(int runNumber, int fileNumber, BufferInfo bufferInfo, string fileDestinationStem) {
    //Initializing the binary file object
    LDF_file ldf(filename);
    ldf_pos_index_for_findSpills = 0;

    // Set file length
    ldf.GetFile().open(ldf.GetName().c_str(), std::ios::binary);
    ldf.GetFile().seekg(0, ldf.GetFile().end);
    ldf.SetLength(ldf.GetFile().tellg());
    ldf.GetFile().seekg(0, ldf.GetFile().beg);
    ldf.GetFile().close();
    // iData will be the last data index.
    iData=0, iEvt=0;

    // Initializing the data array to zero
    memset(DataArray,0,memoryuse);
    memset(TempArray,0,memoryuse); //Used only when sorting

    // read_ldf() will read a fixed number of spills at once from the binary file
    // if ratemode is enabled, read_ldf() will process only the last part of the file
    old_iData = iData;
    raw_list_size  += read_ldf_spill(ldf, dataForFindSpills, ldf_pos_index_for_findSpills,bufferInfo,1);
    good_list_size += iData;

    // Sorting the data chronologically.
    MergeSort(DataArray, TempArray, 0, iData);

    // Extract first and last time stamps
    if (DataArray[iData-1].time > 0)
        last_ts = DataArray[iData-1].time;

    // Writing statistics and skipping the event builder, will sort everything twice as fast

    // Looking for correlations
    if (corr > 0)
        correlations();

        // Writing to GASPWare
    else if (gasp == 1) {
        event_builder();
        write_gasp();
        totEvt += iEvt;
    }

        // Writing event lists
    else if (list == 1) {
        event_builder_list();
        totEvt += iEvt;
    }

        // Writing to a ROOT Tree
    else if (root == 1) {
        event_builder_tree();
        totEvt += iEvt;
    }

    else if (ausa == 1) {
        event_builder_ausa();
        totEvt += iEvt;
    }

    for(int v = old_iData; v < iData; v++){
        //clean traces from memory
        std::vector<unsigned int>().swap(DataArray[v].trace);
        std::vector<unsigned int>().swap(TempArray[v].trace);
    }

    if (corr == 0) {
        memset(stats, 0, sizeof(stats));
        if (root == 1 || stat == 1 || ausa == 1) {
            rootfile->Write();
            rootfile->Save();
            rootfile->Close();
        }
    }
    ldf.GetFile().close();
    fclose(fp_in);
    //fclose(fp_out);
    return ldf_pos_index_for_findSpills;
}

pair<vector<BufferInfo>,int> Xia4idsRunner::findSpills(int runNumber, int fileNumber, BufferInfo bufferInfo, int nThreads, int spillsPerRead) {
    //find the starts of the next nThread spills in the specific file

    if (rate == 0) {
        if (fileNumber == 0)
            sprintf(filename, "%s%03d.ldf", runname, runNumber);
        else
            sprintf(filename, "%s%03d-%d.ldf", runname, runNumber, fileNumber);
        fp_in = fopen(filename, "rb");
        if (!fp_in) {
            printf("File parsing completed: %s does not exist\n", filename);
        }
    }

    LDF_file ldf(filename);
    int pos_index = bufferInfo.spillEnd; // position index in the file

    // Set file length
    ldf.GetFile().open(ldf.GetName().c_str(), std::ios::binary);
    ldf.GetFile().seekg(0, ldf.GetFile().end);
    ldf.SetLength(ldf.GetFile().tellg());
    ldf.GetFile().seekg(0, ldf.GetFile().beg);
    ldf.GetFile().close();

    cout << "finding spills starting from " << pos_index << " out of " << ldf.GetFileLength() << " in " << filename << endl;

    ldf.GetFile().open(ldf.GetName().c_str(), std::ios::binary);
    // Get length and rewind to read from beg
    ldf.GetFile().seekg(0, ldf.GetFile().beg);
    // Start reading ldf DATA buffers
    if (pos_index == 0)
    {
        ldf.GetFile().seekg(65552, ldf.GetFile().beg); //10010 (hex) offset of DATA buffer type
    }
    else
    {
        ldf.GetFile().seekg(pos_index, ldf.GetFile().beg); // resume reading the following spills
    }

    bool debug_mode = false; /// Set to true if the user wishes to display debug information.

    // variables for reading data buffer
    bool full_spill;
    bool bad_spill;
    unsigned int nBytes;
    std::stringstream status;

    // variable that stores data spill
    unsigned int* data_ = new unsigned int[memoryuse];

    vector<BufferInfo> bufferInfos = {};
    int retvalue = 0;
    while(bufferInfos.size() < nThreads){
        int nSpills = 0;
        BufferInfo bufferInfo;
        bufferInfo.spillStart = ldf.GetFile().tellg();
        while(nSpills < spillsPerRead) {
            if (!dataForFindSpills.Read(&ldf.GetFile(), (char *) data_, nBytes, 0, full_spill, bad_spill,
                                        debug_mode)) {     // Reading a spill from the binary file
                retvalue = dataForFindSpills.GetRetval();
                if (dataForFindSpills.GetRetval() == 1) {
                    if (debug_mode) {
                        std::cout << "debug: Encountered single EOF buffer (end of run).\n";
                    }
                } else if (dataForFindSpills.GetRetval() == 2) {
                    if (debug_mode) {
                        std::cout << "debug: Encountered double EOF buffer (end of file).\n";
                    }
                    break;
                } else if (dataForFindSpills.GetRetval() == 3) {
                    if (debug_mode) {
                        std::cout << "debug: Encountered unknown ldf buffer type.\n";
                    }
                } else if (dataForFindSpills.GetRetval() == 4) {
                    if (debug_mode) {
                        std::cout << "debug: Encountered invalid spill chunk.\n";
                    }
                } else if (dataForFindSpills.GetRetval() == 5) {
                    if (debug_mode) {
                        std::cout << "debug: Received bad spill footer size.\n";
                    }
                } else if (dataForFindSpills.GetRetval() == 6) {
                    if (debug_mode) {
                        std::cout << "debug: Failed to read buffer from input file.\n";
                    }
                    break;
                }
                continue;
            }

            if (debug_mode) {
                status << "\033[0;32m" << " [READ] " << "\033[0m" << nBytes / 4 << " words ("
                       << 100 * ldf.GetFile().tellg() / ldf.GetFileLength() << "%), ";
                status << "GOOD = " << dataForFindSpills.GetNumChunks() << ", LOST = "
                       << dataForFindSpills.GetNumMissing();
                std::cout << "\r" << status.str();
            }


            if (full_spill) {
                if (debug_mode) {
                    std::cout << std::endl << "full spill is true!" << std::endl;
                    std::cout << "debug: Retrieved spill of " << nBytes << " bytes (" << nBytes / 4 << " words)\n";
                    std::cout << "debug: Read up to word number " << ldf.GetFile().tellg() / 4 << " in input file\n";
                }
                if (!bad_spill) {
                } else {
                    std::cout << " WARNING: Spill has been flagged as corrupt, skipping (at word "
                              << ldf.GetFile().tellg() / 4
                              << " in file)!\n";
                }

            } else if (debug_mode) {
                std::cout << std::endl << "Not full spill!" << std::endl;
                std::cout << "debug: Retrieved spill fragment of " << nBytes << " bytes (" << nBytes / 4 << " words)\n";
                std::cout << "debug: Read up to word number " << ldf.GetFile().tellg() / 4 << " in input file\n";
                std::cout << std::endl << std::endl;
            }
            nSpills++;
        }
        bufferInfo.spillEnd = ldf.GetFile().tellg();
        bufferInfos.push_back(bufferInfo);
        if(retvalue == 2 or retvalue == 6){
            break;
        }
    } // Finished reading 'max_num_spill' spills
    ldf.GetFile().close();
    fclose(fp_in);
    //fclose(fp_out);
    delete[] data_;
    return make_pair(bufferInfos,retvalue);
}