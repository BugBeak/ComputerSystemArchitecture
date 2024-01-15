#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>

using namespace std;

int main (int argc, char** argv) {
	ifstream config;
	config.open(argv[1]);

	int m, w, h;
	config >> m;
	config >> h;
	config >> w;
 
	config.close();

	ofstream out;
	string out_file_name = string(argv[2]) + ".out";
	out.open(out_file_name.c_str());
	
	ifstream trace;
	trace.open(argv[2]);

	// TODO: Implement a two-level branch predictor 
	string bhtEntryDef(w,'0');
	vector<string> bhtTable;
	for(int i = 0; i<pow(2, h); i++){
		bhtTable.push_back(bhtEntryDef);
	}
	bitset<2> satCounterDef = bitset<2>(2);//weak taken
	vector<bitset<2>> phtTable;
	for(int i = 0; i<pow(2, m); i++){
		phtTable.push_back(satCounterDef);
	}


	// int count = 1;
	while (!trace.eof()) {
		string line, hexString;
		getline(trace, line);
		// cout<<"line number "<<count++<<endl;
		bool branchTaken = line.substr(11,1)=="0" ? false : true;

		// hexString = line.substr(2,8);
		// cout<<hexString;
		// istringstream iss(hexString); // hex string to hex istringstream
		// unsigned long long hexValue; 
    	// iss >> hex >> hexValue; //hex istringstream to long
		// bitset<32> branchAdd(hexValue);  // long to bitset

		bitset<4 * sizeof(unsigned long long)> bits(stoull(line.substr(2,8), nullptr, 16));
    	string branchAddressString = bits.to_string(); // bitset to a binary string

		int bhtIndex = bitset<32>(branchAddressString.substr(branchAddressString.size()-2-h, h)).to_ulong();
		// cout<<"\t bhtIndex: "<<bhtIndex<<endl;

		string bhtContribution = bhtTable[bhtIndex];
		// cout<<"\t bhtContribution: "<<bhtContribution<<endl;
		string branchAddressContribution = branchAddressString.substr(branchAddressString.size()-2-m+w, m-w);
		// cout<<"\t branchAddressContribution: "<<branchAddressContribution<<endl;

		string phtIndexStr = branchAddressContribution + bhtContribution;
		// cout<<"\t phtIndexStr: "<<phtIndexStr<<endl;

		int phtindex = bitset<32>(phtIndexStr).to_ulong();
		bitset<2> prediction = phtTable[phtindex];
		out << prediction[1] << endl; 
		// cout<<"\t prediction: "<<prediction[0]<<endl;


		//now we update our tables
		//PHT first
		if(branchTaken){
			if(prediction.to_ulong() == 0)
				phtTable[phtindex] = bitset<2>(1);
			else if(prediction.to_ulong() == 1)
				phtTable[phtindex] = bitset<2>(2);
			else if(prediction.to_ulong() == 2)
				phtTable[phtindex] = bitset<2>(3);
		}else{
			if(prediction.to_ulong() == 3)
				phtTable[phtindex] = bitset<2>(2);
			else if(prediction.to_ulong() == 2)
				phtTable[phtindex] = bitset<2>(1);
			else if(prediction.to_ulong() == 1)
				phtTable[phtindex] = bitset<2>(0);
		}
		// cout<<"\t change to pht: "<<phtTable[phtindex] <<endl;
		//BHT update
		string bhtEntryUpdated = bhtContribution.substr(1,w-1);
		bhtEntryUpdated.append(branchTaken ? "1" : "0");
		bhtTable[bhtIndex] = bhtEntryUpdated;
		// cout<<"\t change to bht: "<<bhtTable[bhtIndex] <<endl;
		// cout<<endl;

	}
	 
	trace.close();	
	out.close();
}

// Path: branchsimulator_skeleton_23.cpp