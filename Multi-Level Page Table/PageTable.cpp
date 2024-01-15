#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

#define MemSize (65536)

class PhyMem    
{
  public:
    bitset<32> readdata;  
    PhyMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("pt_initialize.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open page table init file";
      dmem.close();

    }  
    bitset<32> outputMemValue (bitset<12> Address) 
    {    
      bitset<32> readdata;
      /**TODO: implement!
       * Returns the value stored in the physical address 
       */
      // readdata = DMem[Address.to_ulong()];
      string insmem;
      int addInt = Address.to_ulong();
      insmem.append(DMem[addInt].to_string());
      insmem.append(DMem[addInt+1].to_string());
      insmem.append(DMem[addInt+2].to_string());
      insmem.append(DMem[addInt+3].to_string());
      readdata = bitset<32>(insmem);
      return readdata;     
    }              

  private:
    vector<bitset<8> > DMem;

};  

int main(int argc, char *argv[])
{
    PhyMem myPhyMem;

    ifstream traces;
    ifstream PTB_file;
    ofstream tracesout;

    string outname;
    outname = "pt_results.txt";

    traces.open(argv[1]);
    PTB_file.open(argv[2]);
    tracesout.open(outname.c_str());

    //Initialize the PTBR
    bitset<12> PTBR;
    PTB_file >> PTBR;

    string line;
    bitset<14> virtualAddr;

    /*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/

    // Read a virtual address form the PageTable and convert it to the physical address - CSA23
    if(traces.is_open() && tracesout.is_open())
    {
        while (getline(traces, line))
        {
            virtualAddr = bitset<14>(line);
            bitset<4> outerPage4bit, innerPage4bit;
            bitset<6> offset;
            for (int i=0; i<4; i++){
              outerPage4bit[i] = virtualAddr[i+10];
              innerPage4bit[i] = virtualAddr[i+6];
              offset[i] = virtualAddr[i];
            }
            offset[4] = virtualAddr[4];
            offset[5] = virtualAddr[5];

            bitset<12> outerTableAddress = bitset<12>(PTBR.to_ulong()+outerPage4bit.to_ulong()*4);

            bitset<32> outerTableOutput = myPhyMem.outputMemValue(outerTableAddress);
            
            if(outerTableOutput[0]==0){
              tracesout<<"0, 0, 0x000, 0x00000000"<<endl;
            }else{
              bitset<12> innerPage12bit;
              for (int i=0; i<12; i++){
                innerPage12bit[i] = outerTableOutput[i+20];
              }
              bitset<12> innerTableAddress = bitset<12>(innerPage12bit.to_ulong()+innerPage4bit.to_ulong()*4);

              bitset<32> innerTableOutput = myPhyMem.outputMemValue(innerTableAddress);

              if(innerTableOutput[0]==0){
                tracesout<<"1, 0, 0x000, 0x00000000"<<endl;
              }else{
                bitset<6> frameNo;
                for (int i=0; i<6; i++){
                  frameNo[i] = innerTableOutput[i+26];
                }
                bitset<12> physicalAddress = bitset<12>(offset.to_ulong()+frameNo.to_ulong()*64);
                bitset<32> physicalMemValue = myPhyMem.outputMemValue(physicalAddress);

                string phyAddressString, phyValueString;

                stringstream res1, res2;
                res1 <<  hex  << physicalAddress.to_ulong();
                phyAddressString = res1.str();
                res2 << hex  << physicalMemValue.to_ulong();
                phyValueString = res2.str();

                if(phyAddressString.length()==1){
                  phyAddressString = "00" + phyAddressString;
                }else if (phyAddressString.length()==2){
                  phyAddressString = "0" + phyAddressString;
                }

                int count = 8 - phyValueString.length();
                while(count > 0){
                  phyValueString = "0" + phyValueString;
                  --count;
                }
                

                tracesout<<"1, 1, 0x"<<phyAddressString<<", 0x"<<phyValueString<<endl;
              }
            
            }
            //TODO: Implement!
            // Access the outer page table 

            // If outer page table valid bit is 1, access the inner page table 

            //Return valid bit in outer and inner page table, physical address, and value stored in the physical memory.
            // Each line in the output file for example should be: 1, 0, 0x000, 0x00000000

        }
        traces.close();
        tracesout.close();
    }

    else
        cout << "Unable to open trace or traceout file ";

    return 0;
}
