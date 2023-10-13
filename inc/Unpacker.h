#ifndef UNPACKER_H
#define UNPACKER_H

#pragma once
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include "XiaData.h"
#include "XiaListModeDataMask.h"
#include "dig_daq_param.hh"
#include "Trace.hh"

using namespace std;

class Unpacker {
public:
	/// Default constructor.
	Unpacker() {
		InitializeMaskMap(); }

	bool ReadSpill(std::vector<XiaData*>& decodedList, unsigned int* data, unsigned int nWords, bool is_verbose, bool& debug_mode,DigDaqParam* dig_daq_params[][17]);
	int DecodeBuffer(std::vector<XiaData*>& result, unsigned int* buf, const unsigned int& vsn, bool& debug_mode, DigDaqParam* dig_daq_params[][17]);
	std::pair<unsigned int, unsigned int> DecodeWordZero(const unsigned int& word, XiaData& data,
		const XiaListModeDataMask& mask);
	void DecodeWordTwo(const unsigned int& word, XiaData& data, const XiaListModeDataMask& mask);
    void DecodeTraceWord(const unsigned int& word, XiaData& data, const XiaListModeDataMask& mask);
    unsigned int DecodeWordThree(const unsigned int& word, XiaData& data,
		const XiaListModeDataMask& mask);
	/// Method calculates time stamp from event times and mask info (frequency).
	/// returns <incomplete time stamp (without Cfd fractional time), time stamp>
	pair<double, double> CalculateTimeInSamples(const XiaListModeDataMask& mask,
		XiaData& data);
private:
	map<unsigned int, pair<string, unsigned int> > maskMap_;///< Maps firmware/frequency to module number.
	void InitializeMaskMap();
};

#endif