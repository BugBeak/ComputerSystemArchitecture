#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <limits>

using std::cout;
using std::endl;
using std::string;
using std::vector;

string inputtracename = "trace.txt";
// remove the ".txt" and add ".out.txt" to the end as output name
string outputtracename = inputtracename.substr(0, inputtracename.length() - 4) + ".out.txt";
string hardwareconfigname = "config.txt";

enum Operation
{
	ADD,
	SUB,
	MULT,
	DIV,
	LOAD,
	STORE
};
// The execute cycle of each operation: ADD, SUB, MULT, DIV, LOAD, STORE
const int OperationCycle[6] = {2, 2, 10, 40, 2, 2};
// const int OperationCycle[6] = {2, 2, 4, 40, 8, 2};

struct HardwareConfig
{
	int LoadRSsize;	 // number of load reservation stations
	int StoreRSsize; // number of store reservation stations
	int AddRSsize;	 // number of add reservation stations
	int MultRSsize;	 // number of multiply reservation stations
	int FRegSize;	 // number of fp registers
};

// We use the following structure to record the time of each instruction
struct InstructionStatus
{
	int cycleIssued;
	int cycleExecuted; // execution completed
	int cycleWriteResult;
};

// Register Result Status structure
struct RegisterResultStatus
{
	string ReservationStationName;
	bool dataReady;
};

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
class Instruction{
	public:
		Instruction(string text){
			string s;
			std::stringstream ss(text);
			getline(ss, s, ' ');
			if(s=="LOAD") 
				op = Operation::LOAD;
			else if (s=="STORE")
				op = Operation::STORE;
			else if (s=="ADD")
				op = Operation::ADD;
			else if (s=="SUB")
				op = Operation::SUB;
			else if (s=="MULT")
				op = Operation::MULT;
			else if (s=="DIV")
				op = Operation::DIV;
			// latency=getLatency(op);
			getline(ss, s, ' ');
			destination = s;
			getline(ss, s, ' ');
			source1 = s;
			getline(ss, s, ' ');
			source2 = s;
		}
		Operation op;
		string destination, source1, source2;
		// int latency;	
};

class RegisterResultStatuses
{
public:
	// ...
	RegisterResultStatuses(int fRegSizee){
		for(int i=0 ; i < fRegSizee ; i++){
			RegisterResultStatus registerDef = {"", false};
			_registers.push_back(registerDef);
		}
		fRegSize = fRegSizee;
	}
	void updateresult(string stationName){
		for (int i=0; i<fRegSize; i++){
			if (_registers[i].ReservationStationName==stationName){
				_registers[i].dataReady=true;
			}
		}
	}
	bool dataReadyAti(int index){
		return _registers[index].dataReady;
	}
	string reservStNameAti(int index){
		return _registers[index].ReservationStationName;
	}
	void assignReservStAti(int index, string name){
		_registers[index].ReservationStationName=name;
		_registers[index].dataReady = false;
	}
	void printAll(){
		for (int j=0;j<fRegSize;j++){
			cout<<"Reg "<<j<<": RES STA NAME: "<<_registers[j].ReservationStationName<<" DATAREADY: "<<_registers[j].dataReady<<endl;
		}
	}

/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/
	/*
	Print all register result status. It is called by PrintRegisterResultStatus4Grade() for grading.
	If you don't want to write such a class, then make sure you call the same function and print the register
	result status in the same format.
	*/
	string _printRegisterResultStatus() const
	{
		std::ostringstream result;
		for (int idx = 0; idx < _registers.size(); idx++)
		{
			result << "F" + std::to_string(idx) << ": ";
			result << _registers[idx].ReservationStationName << ", ";
			result << "dataRdy: " << (_registers[idx].dataReady ? "Y" : "N") << ", ";
			result << "\n";
		}
		return result.str();
	}
/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
private:
	vector<RegisterResultStatus> _registers;
	int fRegSize;
};
int getLatency(Operation op){
	if(op==Operation::ADD)
		return OperationCycle[0];
	else if (op==Operation::SUB)
		return OperationCycle[1];
	else if (op==Operation::MULT)
		return OperationCycle[2];
	else if (op==Operation::DIV)
		return OperationCycle[3];
	else if (op==Operation::LOAD)
		return OperationCycle[4];
	else return OperationCycle[5];
	
};
class CommonDataBus
{
public:
	// ...
	CommonDataBus(){
		stationName="";
		cycleissuediden=0;
	}
	
	void addStationName(string name, int index){
		stationName=name;
		cycleissuediden=index;
	}
	void empty(){
		stationName="";
		cycleissuediden=0;
	}
	string getName(){
		return stationName;
	}
	int getCycleIssued(){
		return cycleissuediden;
	}
	bool isEmpty(){
		return stationName=="";
	}
private:
	string stationName;//we actually don't need to save values
	int cycleissuediden;
	//just one string because CDB can only process one data each cycle
};
// Define your Reservation Station structure
struct ReservationStation
{
	string name;
	bool busy;
	Operation operation;
	// float Vj, Vk;
	bool Vj, Vk;
	string Qj, Qk;
	int remainCycle, cycleIssued;

};
class ReservationStations
{
public:
	// ...
	ReservationStations(HardwareConfig hardwareConfigg){
		ReservationStation reservStation = {"", false, Operation::ADD, false, false, "", "", -1, 0};
		for(int i=0;i<hardwareConfigg.AddRSsize ;  i++){
			reservStation.name = "Add"+std::to_string(i);
			// reservStation.operation = Operation::ADD;
			_stations.push_back(reservStation);
		}
		for(int i=0;i< hardwareConfigg.MultRSsize; i++){
			reservStation.name = "Mult"+std::to_string(i);
			// reservStation.operation = Operation::MULT;
			_stations.push_back(reservStation);
		}
		for(int i=0;i<hardwareConfigg.LoadRSsize; i++){
			reservStation.name = "Load"+std::to_string(i);
			reservStation.operation = Operation::LOAD;
			_stations.push_back(reservStation);
		}
		for(int i=0;i< hardwareConfigg.StoreRSsize; i++){
			reservStation.name = "Store"+std::to_string(i);
			reservStation.operation = Operation::STORE;
			_stations.push_back(reservStation);
		}
		hardwareConfig = hardwareConfigg;
	}

	void decrRemainCycles(CommonDataBus &commonDataBus, vector<InstructionStatus> &instructionStatusList, int thiscyclee){
		vector<std::pair<int,int>> indicesToBeBroadcasted;
		for(int i=0 ; i < _stations.size() ; i++){
			if(_stations[i].busy){
				if(_stations[i].remainCycle==-1){
					//check if sources are ready and assign remaincycles if yes
					if(_stations[i].Vj&&_stations[i].Vk){
						// cout<<"starting execution of station "<<i<<endl;
						_stations[i].remainCycle = getLatency(_stations[i].operation);
					}
				}else if(_stations[i].remainCycle==1){
					int cyclee=_stations[i].cycleIssued;
					for (int j=0;j<instructionStatusList.size();j++){
						if(instructionStatusList[j].cycleIssued==cyclee){
							if(instructionStatusList[j].cycleExecuted==0){
								instructionStatusList[j].cycleExecuted=thiscyclee;
								break;
							}
						}
					}
					indicesToBeBroadcasted.push_back(std::make_pair(_stations[i].cycleIssued, i));
				}else if(_stations[i].remainCycle==0){
					_stations[i].busy=false;
					_stations[i].remainCycle=-1;
				}else{
					//just decrease remincycle
					_stations[i].remainCycle -=1;
				}
			}
			else if(_stations[i].remainCycle==0) _stations[i].remainCycle=-1;
		}
		if(!indicesToBeBroadcasted.empty()){
			if(indicesToBeBroadcasted.size()>1){
			//process result of instr issued first
				sort(indicesToBeBroadcasted.begin(), indicesToBeBroadcasted.end());//by default, c++ sorts vector of pairs by the first element. i ahve stored cycleIssued as the first element
			}
			int chosenIndex = indicesToBeBroadcasted[0].second; 
			commonDataBus.addStationName(_stations[chosenIndex].name, _stations[chosenIndex].cycleIssued);
			// cout<<"commondatabus addition "<<commonDataBus.getName()<<endl;
			_stations[chosenIndex].remainCycle=0;
			//if there were more indices to be broadcasted, their remainCycle is still 0 and they will get processed next turn
		}
	}
	void writeresult(string name){
		for(int i=0 ; i < _stations.size() ; i++){
			// cout<<"          station "<<i<<" name: "<<_stations[i].Qj<< " "<<_stations[i].Qk<<endl;
			if(_stations[i].Qj==name){
				_stations[i].Qj="";
				_stations[i].Vj=true;
				// cout<<"          station "<<i<<" name: "<<_stations[i].Qj<< " "<<_stations[i].Qk<<endl;
			}
			if(_stations[i].Qk==name){
				_stations[i].Qk="";
				_stations[i].Vk=true;
				// cout<<"          station "<<i<<" name: "<<_stations[i].Qj<< " "<<_stations[i].Qk<<endl;
			}
			// cout<<"          station "<<i<<" name: "<<_stations[i].Qj<< " "<<_stations[i].Qk<<endl;
			
		}
	}
	int whichReservStOfTypeIsAvailable(Operation opp){
		int start, end;
		if(opp==Operation::ADD || opp==Operation::SUB){
			start = 0;
			end = hardwareConfig.AddRSsize-1;
		}else if(opp==Operation::MULT || opp==Operation::DIV){
			start = hardwareConfig.AddRSsize;
			end = start + hardwareConfig.MultRSsize-1;
		}else if(opp==Operation::LOAD){
			start = hardwareConfig.AddRSsize + hardwareConfig.MultRSsize;
			end = start + hardwareConfig.LoadRSsize-1;
		}else{
			start = hardwareConfig.AddRSsize + hardwareConfig.MultRSsize + hardwareConfig.LoadRSsize;
			end = start + hardwareConfig.StoreRSsize-1;
		}
		int avail = -1;
		while(start<=end && avail==-1){
			if (!_stations[start].busy)
				avail = start;
			start++;
		}
		return avail;
	};
	string nameOfReservStAti(int index){
		return _stations[index].name;
	}
	void issueInstr(int index, Operation op, string Qj, string Qk, int cycleIssuedd){
		_stations[index].busy=true;
		_stations[index].operation = op;
		_stations[index].Qj = Qj;
		_stations[index].Qk = Qk;
		if(Qj==""){
			_stations[index].Vj=true;
		} else{
			_stations[index].Vj=false;
		}
		if(Qk==""){
			_stations[index].Vk=true;
		} else{
			_stations[index].Vk=false;
		}
		if(_stations[index].Vj&&_stations[index].Vk){
			_stations[index].remainCycle=getLatency(op);
		}
		_stations[index].cycleIssued=cycleIssuedd;
		// cout<<"ISSUED          station "<<index<<" name: "<<_stations[index].Qj<< " "<<_stations[index].Qk<<endl;
	}
	void printRemainingCyclesOfAllStations(){
		for(int i=0 ; i < _stations.size() ; i++){
			cout<<"Station "<<i<<" name: "<<_stations[i].name<<" cycleissues: "<<_stations[i].cycleIssued<<" remainCycle: "<<_stations[i].remainCycle<<endl;
		}
	}
private:
	vector<ReservationStation> _stations;
	HardwareConfig hardwareConfig;
};

void PrintRegisterResultStatus4Grade(const string &filename,
									 const RegisterResultStatuses &registerResultStatus,
									 const int thiscycle);

// Function to simulate the Tomasulo algorithm
void simulateTomasulo(vector <Instruction> instructionList, vector<InstructionStatus> &instructionStatusList, ReservationStations reservStations,RegisterResultStatuses registerResultStatus, CommonDataBus commonDataBus)
{

	int thiscycle = 1; // start cycle: 1
	// RegisterResultStatuses registerResultStatus;
	int instrNumber = 0, instrListSize = instructionList.size();
	Instruction instr = instructionList[instrNumber];
	bool allresultswritten=false;
	while (thiscycle < 100000000 && !allresultswritten)
	{
		// cout<<"CYCLE: "<<thiscycle<<endl;

		//Read the Common Data Bus (CDB), and update registers and reservation stations.
		if(!commonDataBus.isEmpty()){
			string resultToProcess = commonDataBus.getName();
			registerResultStatus.updateresult(resultToProcess);
			reservStations.writeresult(resultToProcess);
			for (int j=0;j<instructionStatusList.size();j++){
				if(instructionStatusList[j].cycleIssued==commonDataBus.getCycleIssued()){
					instructionStatusList[j].cycleWriteResult=thiscycle;
					break;
				}
			}
			commonDataBus.empty();
		}
		

		bool newallresultswritten=true;
		for (int j=0;j<instructionStatusList.size();j++){
			if(instructionStatusList[j].cycleWriteResult==0){
				newallresultswritten=false;
				break;
			}
		}
		allresultswritten=newallresultswritten;
		// Reservation Stations should be updated every cycle, and broadcast to Common Data Bus
		reservStations.decrRemainCycles(commonDataBus, instructionStatusList, thiscycle);
		// ...
		
		// Issue new instruction in each cycle
		// but only if a free reservation station is available.
		// reservStations.printRemainingCyclesOfAllStations();
		if(instrNumber<instrListSize){
			// cout<<"     instrNumber "<<instrNumber<<endl;
			// cout<<"     instrOp ";
			// switch (instr.op) {
	        // case ADD:
	        //     cout << "ADD" <<endl;
	        //     break;
	        // case SUB:
	        //     cout << "SUB" <<endl;
	        //     break;
	        // case MULT:
	        //     cout << "MULT" <<endl;
	        //     break;
	        // case DIV:
	        //     cout << "DIV" <<endl;
	        //     break;
	        // case LOAD:
	        //     cout << "LOAD" <<endl;
	        //     break;
	        // case STORE:
	        //     cout << "STORE" <<endl;
	        //     break;
	        // default:
	        //     cout << "Unknown" <<endl;
	        //     break;
	    	// }
			int availReservSt = reservStations.whichReservStOfTypeIsAvailable(instr.op);
			// cout<<"      availReservSt "<<availReservSt<<endl;
			if(availReservSt>=0){
				instructionStatusList[instrNumber].cycleIssued=thiscycle;
				string Qj="", Qk="";
				//Examine the register result status.
				string destination, source1, source2;
				if (instr.op==Operation::STORE){
					string source1indexString = instr.destination;
					source1indexString.erase(0,1);
					// cout<<"      source1indexString "<<source1indexString<<endl;
					int source1index = std::stoi(source1indexString);
					if(!registerResultStatus.dataReadyAti(source1index)){
						Qj=registerResultStatus.reservStNameAti(source1index);
					}
					
					reservStations.issueInstr(availReservSt, instr.op, Qj, "", thiscycle);
				}
				else if (instr.op!=Operation::LOAD && instr.op!=Operation::STORE){
					string source1indexString = instr.source1;
					source1indexString.erase(0,1);
					// cout<<"      source1indexString "<<source1indexString<<endl;
					int source1index = std::stoi(source1indexString);
					if(!registerResultStatus.dataReadyAti(source1index)){
						Qj=registerResultStatus.reservStNameAti(source1index);
					}
					string source2indexString = instr.source2;
					source2indexString.erase(0,1);
					// cout<<"      source2indexString "<<source2indexString<<endl;
					int source2index = std::stoi(source2indexString);
					if(!registerResultStatus.dataReadyAti(source2index)){
						Qk=registerResultStatus.reservStNameAti(source2index);
					}
					reservStations.issueInstr(availReservSt, instr.op, Qj, Qk, thiscycle);

					string destindexString = instr.destination;
					destindexString.erase(0,1);
					// cout<<"destindexString "<<destindexString<<endl;
					registerResultStatus.assignReservStAti(std::stoi(destindexString), reservStations.nameOfReservStAti(availReservSt));
				}else{
					reservStations.issueInstr(availReservSt, instr.op, Qj, Qk, thiscycle);
					string destindexString = instr.destination;
					destindexString.erase(0,1);
					// cout<<"destindexString "<<destindexString<<endl;
					registerResultStatus.assignReservStAti(std::stoi(destindexString), reservStations.nameOfReservStAti(availReservSt));
				}
				instrNumber=instrNumber+1;
				if(instrNumber<instrListSize)
					instr = instructionList[instrNumber];
				
			}
		}
		// reservStations.printRemainingCyclesOfAllStations();
		// for (int j=0;j<instructionStatusList.size();j++){
		// 	cout<<"Instr "<<j+1<<": ISS: "<<instructionStatusList[j].cycleIssued<<" EXE: "<<instructionStatusList[j].cycleExecuted<<" WRI: "<<instructionStatusList[j].cycleWriteResult<<endl;
		// }
		// registerResultStatus.printAll();
		
		// At the end of this cycle, we need this function to print all registers status for grading
		PrintRegisterResultStatus4Grade(outputtracename, registerResultStatus, thiscycle);

		++thiscycle;

		// The simulator should stop when all instructions are finished.
		// ...
	}
};


/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/

/*
print the instruction status, the reservation stations and the register result status
@param filename: output file name
@param instructionStatus: instruction status
*/
void PrintResult4Grade(const string &filename, const vector<InstructionStatus> &instructionStatus)
{
	std::ofstream outfile(filename, std::ios_base::app); // append result to the end of file
	outfile << "Instruction Status:\n";
	for (int idx = 0; idx < instructionStatus.size(); idx++)
	{
		outfile << "Instr" << idx << ": ";
		outfile << "Issued: " << instructionStatus[idx].cycleIssued << ", ";
		outfile << "Completed: " << instructionStatus[idx].cycleExecuted << ", ";
		outfile << "Write Result: " << instructionStatus[idx].cycleWriteResult << ", ";
		outfile << "\n";
	}
	outfile.close();
}

/*
print the register result status each 5 cycles
@param filename: output file name
@param registerResultStatus: register result status
@param thiscycle: current cycle
*/
void PrintRegisterResultStatus4Grade(const string &filename,
									 const RegisterResultStatuses &registerResultStatus,
									 const int thiscycle)
{
	if (thiscycle % 5 != 0)
		return;
	std::ofstream outfile(filename, std::ios_base::app); // append result to the end of file
	outfile << "Cycle " << thiscycle << ":\n";
	outfile << registerResultStatus._printRegisterResultStatus() << "\n";
	outfile.close();
}

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		hardwareconfigname = argv[1];
		inputtracename = argv[2];
	}

	HardwareConfig hardwareConfig;
	std::ifstream config;
	config.open(hardwareconfigname);
	config >> hardwareConfig.LoadRSsize;  // number of load reservation stations
	config >> hardwareConfig.StoreRSsize; // number of store reservation stations
	config >> hardwareConfig.AddRSsize;	  // number of add reservation stations
	config >> hardwareConfig.MultRSsize;  // number of multiply reservation stations
	config >> hardwareConfig.FRegSize;	  // number of fp registers
	config.close();

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/

	// Read instructions from a file (replace 'instructions.txt' with your file name)
	// ...
	std::ifstream instructionFile;
	instructionFile.open(inputtracename);
	vector <Instruction> instructionList;
	while (!instructionFile.eof()) {
		string line;
		getline(instructionFile, line);
		// cout<<line<<endl;
		if(line!=""){
			Instruction myinstr(line);
			instructionList.push_back(myinstr);
		}
	}

	// Initialize the register result status
	// RegisterResultStatuses registerResultStatus();
	// ...
	RegisterResultStatuses registerResultStatus(hardwareConfig.FRegSize);
	ReservationStations reservStations(hardwareConfig);
	//we actually don't need to calculate the actual values
	CommonDataBus commonDataBus;
	// Initialize the instruction status table
	vector<InstructionStatus> instructionStatusList;
	for (int i = 0; i<instructionList.size(); i++){
		InstructionStatus instructionStatus = {0,0,0};
		instructionStatusList.push_back(instructionStatus);
	}
	// ...

	// Simulate Tomasulo:
	simulateTomasulo(instructionList, instructionStatusList, reservStations, registerResultStatus , commonDataBus);

/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/

	// At the end of the program, print Instruction Status Table for grading
	PrintResult4Grade(outputtracename, instructionStatusList);

	return 0;
}
