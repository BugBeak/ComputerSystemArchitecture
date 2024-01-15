#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR  (5)
#define NOR (7)

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!  
      if (WrtEnable[0]==1) {
        Registers[(int)(WrtReg.to_ulong())] = WrtData;
      } else {
        ReadData1 = Registers[(int)RdReg1.to_ulong()];
        ReadData2 = Registers[(int)RdReg2.to_ulong()];
      }
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      /**
       * @brief Implement the ALU operation here.
       *
       * ALU operation depends on the ALUOP, which are definded as ADDU, SUBU, etc. 
       */
      // TODO: implement!
      if (ALUOP== bitset<3>(string("001"))){//addu
        ALUresult = bitset<32>(oprand1.to_ulong()+ oprand2.to_ulong());
      }else if (ALUOP== bitset<3>(string("011"))){//subu
        ALUresult = bitset<32>(oprand1.to_ulong()- oprand2.to_ulong());
      }else if (ALUOP== bitset<3>(string("100"))){//and
        ALUresult = oprand1&=oprand2;
      }else if (ALUOP== bitset<3>(string("101"))){//or
        ALUresult = oprand1 |= oprand2;
      }else if (ALUOP== bitset<3>(string("111"))){//nor
        ALUresult = ~(oprand1 |= oprand2);
      }else if (ALUOP== bitset<3>(string("110"))){//implement other operations!
        ALUresult = oprand1 == oprand2;
      }
      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {      
          IMem[i] = bitset<8>(line);
          i++;
        }

      }
      else cout<<"Unable to open file";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      /**
       * @brief Read Instruction Memory (IMem).
       *
       * Read the byte at the ReadAddress and the following three byte,
       * and return the read result. 
       */
      int readLine = (int) (ReadAddress.to_ulong());
      int len = 8;
      if(readLine == IMem.size()-1) {
        Instruction = ( IMem[readLine].to_ullong() << len*3 );
      } else if(readLine == IMem.size()-2){
        Instruction = ( IMem[readLine].to_ullong() << len*3 ) + (IMem[readLine+1].to_ullong() << len*2);
      } else if(readLine == IMem.size()-3){
        Instruction = ( IMem[readLine].to_ullong() << len*3 ) + (IMem[readLine+1].to_ullong() << len*2) + (IMem[readLine+2].to_ullong() << len);
      } else {
        Instruction = ( IMem[readLine].to_ullong() << len*3 ) + (IMem[readLine+1].to_ullong() << len*2) + (IMem[readLine+2].to_ullong() << len) + IMem[readLine+3].to_ullong();
      }
      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      /**
       * @brief Reads/writes data from/to the Data Memory.
       *
       * This function is used to read/write data from/to the DataMem, depending on the readmem and writemem.
       * First, if writemem enabled, WriteData should be written to DMem, clear or ignore the return value readdata,
       * and note that 32-bit WriteData will occupy 4 continious Bytes in DMem. 
       * If readmem enabled, return the DMem read result as readdata.
       */
      // TODO: implement!
      if (writemem[0]==1){
        int readLine = (int) Address.to_ulong();
        if(readLine == DMem.size()-1) {
          DMem[readLine] = getSubBitset(WriteData,1);
        } else if(readLine == DMem.size()-2){
          DMem[readLine] = getSubBitset(WriteData,1);
          DMem[readLine+1] = getSubBitset(WriteData,2);
        } else if(readLine == DMem.size()-3){
          DMem[readLine] = getSubBitset(WriteData,1);
          DMem[readLine+1] = getSubBitset(WriteData,2);
          DMem[readLine+2] = getSubBitset(WriteData,3);
        } else {
          DMem[readLine] = getSubBitset(WriteData,1);
          DMem[readLine+1] = getSubBitset(WriteData,2);
          DMem[readLine+2] = getSubBitset(WriteData,3);
          DMem[readLine+3] = getSubBitset(WriteData,4);
        }
      }
      if (readmem[0]==1){
        int readLine = (int) Address.to_ulong();
        int len = 8;
        if(readLine == DMem.size()-1) {
          readdata = ( DMem[readLine].to_ullong() << len*3 );
        } else if(readLine == DMem.size()-2){
          readdata = ( DMem[readLine].to_ullong() << len*3 ) + (DMem[readLine+1].to_ullong() << len*2);
        } else if(readLine == DMem.size()-3){
          readdata = ( DMem[readLine].to_ullong() << len*3 ) + (DMem[readLine+1].to_ullong() << len*2) + (DMem[readLine+2].to_ullong() << len);
        } else {
          readdata = ( DMem[readLine].to_ullong() << len*3 ) + (DMem[readLine+1].to_ullong() << len*2) + (DMem[readLine+2].to_ullong() << len) + DMem[readLine+3].to_ullong();
        }
      }
      return readdata;     
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

    bitset<8> getSubBitset (bitset<32> given, int section){
      bitset<8> result = bitset<8>(0);  
      int shift;
      if (section == 1){
        shift = 24;
      } else if (section == 2){
        shift = 16;
      } else if (section == 3){
        shift = 8;
      } else {
        shift = 0;
      }
      for (int i = 0; i<8; i++){
        result[i] = given[i+shift];
      }
      return result;
    }

};  



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;

  bitset<32> programCounter = bitset<32> (0);
  bitset<32> counterIncrement = bitset<32> (4);
  bitset<32> instr = myInsMem.ReadMemory(programCounter);
  string halt_string = "11111111111111111111111111111111";
  bitset<32> halt_bitset(halt_string);   
  // int count=0;
  while (instr!=halt_bitset)  // TODO: implement!
  {
    // printf("Instruction %d \n",count++);
    // printf("Program Counter  %d \n",programCounter.to_ulong());
   
    // Fetch: fetch an instruction from myInsMem.

    // If current instruction is "11111111111111111111111111111111", then break; (exit the while loop)

    // decode(Read RF): get opcode and other signals from instruction, decode instruction
    bitset<1> readmem, writemem, iType, WrtEnable, jType, isBranch;
    readmem = bitset<1>(0);
    writemem = bitset<1>(0);
    WrtEnable = bitset<1>(0);
    isBranch = bitset<1>(0);
    bitset<3> ALUOP = bitset<3>(0);
    bitset<6> opcode = bitset<6>(0);

    //load opcode
    for(int i=0; i<6; i++){
      opcode[i]=instr[i+26];
    }

    if (opcode == bitset<6>(string("100011"))){
      //i type - load
      readmem[0] = 1;
      writemem[0] = 0;
      jType[0] = 0;
      iType[0] = 1;
      ALUOP = bitset<3>(string("001"));//recheck
    }else if (opcode == bitset<6>(string("101011"))){
      //i type - store
      readmem[0] = 0;
      writemem[0] = 1;
      jType[0] = 0;
      iType[0] = 1;
      ALUOP = bitset<3>(string("001"));
    }else if(opcode == bitset<6>(string("000000"))){
      //r type
      jType[0] = 0;
      iType[0] = 0;
      ALUOP[2] = instr[2];
      ALUOP[1] = instr[1];
      ALUOP[0] = instr[0];
    }else{
      if((opcode == bitset<6>(string("000010")))||opcode == bitset<6>(string("000011"))){
        //j type
        //000010 j, 000011 jal
        jType[0] = 1;
        iType[0] = 0;
      }else{
        //smth else
        //like i-type branch instruction
        jType[0] = 0;
        iType[0] = 1;
      }
      ALUOP[2] = instr[28];
      ALUOP[1] = instr[27];
      ALUOP[0] = instr[26];
    }
    if(opcode == bitset<6>(string("000100"))){
      isBranch[0] = 1;
    }
    // printf("i-type? %d j-type? %d \n",static_cast<int>(iType[0]),static_cast<int>(jType[0]));

    if (jType[0]==0){
    
      bitset<5> RdReg1, RdReg2, WrtReg;
      bitset<32> WrtData;
      //load RdReg1, RdReg2
      for(int i=0; i<5; i++){
        RdReg1[i]=instr[i+21];
        RdReg2[i]=instr[i+16];
        if(iType[0]==0)
          WrtReg[i]=instr[i+11];
      }
      if(iType[0]==1)
          WrtReg = RdReg2;

      // if(isBranch[0]==1 || writemem[0]==1 || jType[0]==1)
      //           WrtEnable[0] = 0;
      // else WrtEnable[0] = 1;

      myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);//wrtenable can be 0 by default here?

    // Execute: after decoding, ALU may run and return result
      if(isBranch==1){
        // printf("BRANCH!!!!!!!!!!!!!!!!\n");
        if (myRF.ReadData1 == myRF.ReadData2){
          // printf("    EQUAL\n");
          bitset<32> signExtendImm = bitset<32>(0);
          for(int i=0; i<16; i++){
            signExtendImm[i+2]=instr[i];
          }
          for(int i=18; i<32; i++){
            signExtendImm[i]=instr[15];
          }
          signExtendImm[0] = 0;
          signExtendImm[1] = 0;
          programCounter = bitset<32>(programCounter.to_ulong() + counterIncrement.to_ulong() + signExtendImm.to_ulong());
        }else{
          // printf("    NOT EQUAL\n");
          programCounter = bitset<32>(programCounter.to_ulong() + counterIncrement.to_ulong());
        }
      }else{
        programCounter = bitset<32>(programCounter.to_ulong() + counterIncrement.to_ulong());
      
        bitset<32> oprand1, oprand2;
        oprand1 = myRF.ReadData1;
        if(iType[0]==0)
          oprand2 = myRF.ReadData2;
        else {
          oprand2 = bitset<32>(0);
          for(int i=0; i<16; i++){
            oprand2[i]=instr[i];
          }
          for(int i=16; i<32; i++){
            oprand2[i]=instr[15];;
          }
        }

        bitset<32> aLUresult = myALU.ALUOperation (ALUOP, oprand1, oprand2);

        // Read/Write Mem: access data memory (myDataMem)
        bitset<32>  datamemresult = myDataMem.MemoryAccess (aLUresult, myRF.ReadData2, readmem, writemem) ;

        if(readmem[0]==0){ 
          WrtData = aLUresult;
        }else{
          WrtData = datamemresult;
        }

        // Write back to RF: some operations may write things to RF
        if(writemem[0]==1)// we already know it's not j type of branch i type
          WrtEnable[0] = 0;
        else WrtEnable[0] = 1;

        myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
      }
    }
    else{
      //j type
      // printf("     Program Counter  %d \n",programCounter.to_ulong());
      programCounter = bitset<32>(programCounter.to_ulong() + counterIncrement.to_ulong());
      if(opcode==bitset<6>(string("000011"))){//jal
        myRF.ReadWrite(bitset<5>(0), bitset<5>(0), bitset<5>(31), programCounter, bitset<1>(1));
      }
      programCounter[1] = 0;
      programCounter[0] = 0;
      for(int i=0; i<26; i++){
        programCounter[i+2]=instr[i];
      }
      // printf("\n     Program Counter  %d \n",programCounter.to_ulong());
    }


    instr = myInsMem.ReadMemory(programCounter);
    /**** You don't need to modify the following lines. ****/
    myRF.OutputRF(); // dump RF; 

  }

  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
