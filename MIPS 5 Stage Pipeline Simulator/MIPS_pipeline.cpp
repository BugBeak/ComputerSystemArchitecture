#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab csa23, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
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

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
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
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
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
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
            // ofstream debugger2;
            // debugger2.open("DEBUG_WTF2.txt", std::ios_base::app);
            // debugger2<<"Checkpost 1"<<endl;
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            // debugger2<<"Checkpost 2"<<endl;
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            // debugger2.close();
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
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
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{

    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
	stateStruct state;

    // bitset<32> counterIncrement = bitset<32> (32);

    //initialize
    state.IF.PC = bitset<32> (0);
    state.IF.nop=0;  

    state.ID.Instr = bitset<32> (0);
    state.ID.nop=1;  

    state.EX.Read_data1 = bitset<32> (0);
    state.EX.Read_data2 = bitset<32> (0);
    state.EX.Imm = bitset<16> (0);
    state.EX.Rs = bitset<5> (0);
    state.EX.Rt = bitset<5> (0);
    state.EX.Wrt_reg_addr = bitset<5> (0);
    state.EX.is_I_type = 0;
    state.EX.rd_mem = 0;
    state.EX.wrt_mem = 0; 
    state.EX.alu_op = 1;     //1 for addu, lw, sw, 0 for subu 
    state.EX.wrt_enable = 0;
    state.EX.nop=1;  

    state.MEM.ALUresult = bitset<32> (0);
    state.MEM.Store_data = bitset<32> (0);
    state.MEM.Rs = bitset<5> (0);
    state.MEM.Rt = bitset<5> (0);
    state.MEM.Wrt_reg_addr = bitset<5> (0);
    state.MEM.rd_mem = 0;
    state.MEM.wrt_mem = 0; 
    state.MEM.wrt_enable = 0;    
    state.MEM.nop=1;    

    state.WB.Wrt_data = bitset<32> (0);
    state.WB.Rs = bitset<5> (0);
    state.WB.Rt = bitset<5> (0);
    state.WB.Wrt_reg_addr = bitset<5> (0);
    state.WB.wrt_enable = 0;
    state.WB.nop=1;  

    int cycle = 0;

    
    while (1) {
        // printf("Starting CYCLE: %d \n",cycle); 

        stateStruct newState = state;
    
        bool stall = 0;
        /* --------------------- WB stage --------------------- */
        if (state.MEM.nop){
            newState.WB.nop = 1;
        }else{
            newState.WB.nop = 0;
        }
        if(!state.WB.nop && state.WB.wrt_enable){
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
        }
        // printf("WB stage over \n"); 

        /* --------------------- MEM stage --------------------- */
        
        if (state.EX.nop){
            newState.MEM.nop = 1;
        }else{
            newState.MEM.nop = 0;
        }
       //debugger<<"\tMEM stage checkpost 1"<<endl; 
        if(!state.MEM.nop){
            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = state.MEM.wrt_enable;
           //debugger<<"\tMEM stage checkpost 2"<<endl;
            //write in memory first to avoid structural hazard
            if(state.MEM.wrt_mem){//sw
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
                newState.WB.Wrt_data = state.MEM.Store_data;//not useful
            }else if(state.MEM.rd_mem){//lw 
                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);//ISSUEEEEEEEE
               //debugger<<"\tMEM stage checkpost 3"<<endl;
            }else if(state.MEM.wrt_enable){//r type (also branch but don't care about wrt_Data for branch)
                newState.WB.Wrt_data = state.MEM.ALUresult;
            }

        }
        // printf("MEM stage over \n"); 
        /* --------------------- EX stage --------------------- */
        //Muxes in EX stage for forwarding
        if (state.ID.nop){
            newState.EX.nop = 1;
        }else {
            newState.EX.nop = 0;
        }
        if(!state.EX.nop){
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem; 
            newState.MEM.wrt_enable = state.EX.wrt_enable;  


            // For ALU result
            bitset<32> operand1, operand2;
            operand1 = state.EX.Read_data1;
            operand2 = state.EX.Read_data2;


            //forwarding
            bitset<32> signExtendImm = bitset<32>(0);
            if(state.EX.is_I_type){
                if(state.EX.rd_mem || state.EX.wrt_mem){
                    //i type sw lw
                    //operand2 = immediate
                    
                    for(int i=0; i<16; i++){
                        signExtendImm[i]=state.EX.Imm[i];
                    }
                    for(int i=16; i<32; i++){
                        signExtendImm[i]=state.EX.Imm[15];
                    }
                    // operand2 = signExtendImm;
                    
                    //operand1
                    if(state.EX.Rs==state.MEM.Wrt_reg_addr && state.EX.Rs!=bitset<5>(0) && state.MEM.nop==0){//type 1
                        // printf("\t i forwarding 1a \n");
                        operand1 = state.MEM.ALUresult; //we check this first because this dependency is the latest and hence overrides the other
                    }else if(state.EX.Rs==state.WB.Wrt_reg_addr && state.EX.Rs!=bitset<5>(0) && state.WB.nop==0){//type 2 &&(state.MEM.rd_mem)
                         // printf("\t i forwarding 2a\n");
                        operand1 = state.WB.Wrt_data;
                    }
                    //operand2
                    if(state.EX.Rt==state.MEM.Wrt_reg_addr && state.EX.Rt!=bitset<5>(0) && state.MEM.nop==0){//type 1
                         // printf("\t i forwarding 1b\n");
                        operand2 = state.MEM.ALUresult;
                    }else if(state.EX.Rt==state.WB.Wrt_reg_addr && state.EX.Rt!=bitset<5>(0) && state.WB.nop==0){//type 2 &&(state.MEM.rd_mem)
                         // printf("\t i forwarding 2b\n");
                        operand2 = state.WB.Wrt_data;
                    }
                }
            }else{// r type
                //operand1
                if(state.EX.Rs==state.MEM.Wrt_reg_addr && state.EX.Rs!=bitset<5>(0) && state.MEM.nop==0){//type 1
                    // printf("\t r forwarding 1a \n");
                    operand1 = state.MEM.ALUresult;
                }else if(state.EX.Rs==state.WB.Wrt_reg_addr && state.EX.Rs!=bitset<5>(0) && state.WB.nop==0){//type 2 &&(state.MEM.rd_mem)
                    // printf("\t r forwarding 2a \n");
                    operand1 = state.WB.Wrt_data;
                }
                //operand2
                if(state.EX.Rt==state.MEM.Wrt_reg_addr && state.EX.Rt!=bitset<5>(0) && state.MEM.nop==0){//type 1
                    // printf("\t r forwarding 1b\n");
                    operand2 = state.MEM.ALUresult;
                }else if(state.EX.Rt==state.WB.Wrt_reg_addr && state.EX.Rt!=bitset<5>(0) && state.WB.nop==0){//type 2 &&(state.MEM.rd_mem)
                    // printf("\t r forwarding 2b\n");
                    operand2 = state.WB.Wrt_data;
                }
            }


            if(state.EX.is_I_type){
                if(state.EX.rd_mem || state.EX.wrt_mem){
                    //i type sw lw
                    newState.MEM.ALUresult = bitset<32>(operand1.to_ulong()+  signExtendImm.to_ulong());
                }else{ //branch
                    newState.MEM.ALUresult = bitset<32> (0);
                }
            }else{
                // r type
                if(state.EX.alu_op){//add
                    newState.MEM.ALUresult = bitset<32>(operand1.to_ulong()+  operand2.to_ulong());
                }else{//sub
                    newState.MEM.ALUresult = bitset<32>(operand1.to_ulong()-  operand2.to_ulong());
                }
            }
            

            newState.MEM.Store_data =  operand2;// will not be relevant in case state.EX.wrt_mem=0

        } 
        // printf("EX stage over \n"); 
        /* --------------------- ID stage --------------------- */
        if (state.IF.nop){
            newState.ID.nop = 1;
        }else{ 
            newState.ID.nop = 0;
        }

        if(state.ID.nop==0) {
            bitset<6> opcode = bitset<6>(0);

            //load opcode
            for(int i=0; i<6; i++){
              opcode[i]=state.ID.Instr[i+26];
            }

            // printf("ID stage opcode: %d \n",opcode);
            if (opcode == bitset<6>(string("100011"))) {
              //i type - load
                newState.EX.is_I_type = 1;
                newState.EX.wrt_mem = 0;
                newState.EX.rd_mem = 1;
                newState.EX.alu_op = 1;     //1 for addu, lw, sw, 0 for subu 
                newState.EX.wrt_enable = 1;

                for(int i=0; i<5; i++){
                    newState.EX.Rs[i]=state.ID.Instr[i+21];
                    newState.EX.Rt[i]=state.ID.Instr[i+16];
                    newState.EX.Imm[i]=state.ID.Instr[i];
                }   
                for(int i=5; i<16; i++){
                    newState.EX.Imm[i]=state.ID.Instr[i];
                }

                newState.EX.Wrt_reg_addr = newState.EX.Rt;

                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = bitset<32> (0);

            }else if (opcode == bitset<6>(string("101011"))) {
              //i type - store
                newState.EX.is_I_type = 1;
                newState.EX.wrt_mem = 1; 
                newState.EX.rd_mem = 0;
                newState.EX.alu_op = 1;     //1 for addu, lw, sw, 0 for subu 
                newState.EX.wrt_enable = 0;

                for(int i=0; i<5; i++){
                    newState.EX.Rs[i]=state.ID.Instr[i+21];
                    newState.EX.Rt[i]=state.ID.Instr[i+16];
                    newState.EX.Imm[i]=state.ID.Instr[i];
                }   
                for(int i=5; i<16; i++){
                    newState.EX.Imm[i]=state.ID.Instr[i];
                }

                newState.EX.Wrt_reg_addr = newState.EX.Rt;//bitset<5> (0); not useful

                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                newState.EX.Read_data1 = bitset<32> (0);

            }else if(opcode == bitset<6>(string("000000"))) {
              //r type
                newState.EX.is_I_type = 0;
                newState.EX.wrt_mem = 0;
                newState.EX.rd_mem = 0;
                if(state.ID.Instr[1]==0){//checking func
                    newState.EX.alu_op = 1; 
                }else{  
                    newState.EX.alu_op = 0;   //1 for addu, lw, sw, 0 for subu 
                }
                newState.EX.wrt_enable = 1;

                for(int i=0; i<5; i++){
                    newState.EX.Rs[i]=state.ID.Instr[i+21];
                    newState.EX.Rt[i]=state.ID.Instr[i+16];
                    newState.EX.Wrt_reg_addr[i]=state.ID.Instr[i+11];
                    newState.EX.Imm[i]=state.ID.Instr[i];//not useful
                }
                for(int i=5; i<16; i++){
                    newState.EX.Imm[i]=state.ID.Instr[i];//not useful
                }
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                // newState.EX.Imm = bitset<16> (0);
                
            }else{
                //i type
                //bne branch
                newState.EX.is_I_type = 1;
                newState.EX.wrt_mem = 0;
                newState.EX.rd_mem = 0;
                newState.EX.alu_op = 1; //branch?
                newState.EX.wrt_enable = 0;

                for(int i=0; i<5; i++){
                    newState.EX.Rs[i]=state.ID.Instr[i+21];
                    newState.EX.Rt[i]=state.ID.Instr[i+16];
                    newState.EX.Imm[i]=state.ID.Instr[i];
                }   
                for(int i=5; i<16; i++){
                    newState.EX.Imm[i]=state.ID.Instr[i];
                }
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                newState.EX.Wrt_reg_addr = bitset<5> (0);

                //resolving branch here
                if(!(newState.EX.Read_data1==newState.EX.Read_data2)){
                    newState.ID.nop = 1;
                   //debugger<<"\tID BRANCH stage checkpost 1 newState.EX.Imm: "<<newState.EX.Imm<<endl;
                    bitset<32> signExtendImm = bitset<32>(0);
                    for(int i=0; i<16; i++){
                        signExtendImm[i]=newState.EX.Imm[i];
                    }
                   //debugger<<"\tID BRANCH stage checkpost 2 signExtendImm: "<<signExtendImm<<endl;
                    
                    for(int i=16; i<32; i++){
                        signExtendImm[i]=signExtendImm[15];
                    }
                   //debugger<<"\tID BRANCH stage checkpost 3 signExtendImm: "<<signExtendImm<<endl;
                    
                    for(int i=29;i>=0;i--){
                        signExtendImm[i+2] = signExtendImm[i];
                    }
                   //debugger<<"\tID BRANCH stage checkpost 4 signExtendImm: "<<signExtendImm<<endl;
                    
                    signExtendImm[0] = 0;
                    signExtendImm[1] = 0;
                   //debugger<<"\tID BRANCH stage checkpost 5"<<endl;
                   //debugger<<"\tID BRANCH stage checkpost 6 state.IF.PC: "<<state.IF.PC<<" signExtendImm: "<<signExtendImm<<endl;
                    state.IF.PC = bitset<32>(state.IF.PC.to_ulong() + signExtendImm.to_ulong());
                    newState.IF.PC = bitset<32>(state.IF.PC.to_ulong()-4);
                   //debugger<<"\tID BRANCH stage checkpost 7 state.IF.PC: "<<state.IF.PC<<" signExtendImm: "<<signExtendImm<<endl;
                }
            }
            // printf("\t is it i type? %d \n",newState.EX.is_I_type);
            //stall check 
            if(state.EX.is_I_type && state.EX.rd_mem){
                if (state.EX.Rt==newState.EX.Rs){//assumption being there are no raw hazards for branches
                    // printf("\t stall\n");
                    stall=1;
                    newState.EX.nop = 1;
                }else if(state.EX.Rt==newState.EX.Rt && !(newState.EX.is_I_type && newState.EX.rd_mem)){//if lw follows lw, the second_lw.rt can be equal to first_lw.rt because it will get replaced anyway
                    // printf("\t stall\n");
                    stall=1;
                    newState.EX.nop = 1;
                }
            }

        }
        //what's going on
       // printf("ID stage over \n"); 
          
        /* --------------------- IF stage --------------------- */
        if(!state.IF.nop){  
           //debugger<<"\tIF stage checkpost 1, state.IF.PC: "<<state.IF.PC<<endl;
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
           //debugger<<"\tIF stage checkpost 2"<<endl;
            if(newState.ID.Instr==bitset<32>(string("11111111111111111111111111111111"))){
                //HALT
                newState.IF.nop = 1;
                newState.ID.nop = 1;
                // newState.IF.PC = state.IF.PC;
            }else{
               //debugger<<"\tIF stage checkpost 4"<<endl;
                newState.IF.nop = 0;
                newState.IF.PC = bitset<32>(newState.IF.PC.to_ulong() + 4);
            }
           //debugger<<"\tIF stage checkpost 5, newState.IF.PC: "<<newState.IF.PC<<endl;
        }
        if(stall){
            newState.IF.PC = bitset<32>(newState.IF.PC.to_ulong()-4);//assumption: no raw hazards for branches 
            newState.ID.Instr = state.ID.Instr;
            if(newState.ID.nop==1){//we have prematurely executed the halt
                newState.IF.nop = 0;
                newState.ID.nop = 0;   
            }
        }
        // printf("\t state.IF.PC %d \t newState.IF.PC: %d \n",state.IF.PC,newState.IF.PC);
        
        // printf("IF stage over\n"); 
        //printf("\tCYCLE: \t %d \tIF.nop: %d ID.nop: %d EX.nop: %d MEM.nop: %d WB.nop: %d\n",cycle, state.IF.nop,state.ID.nop,state.EX.nop,state.MEM.nop,state.WB.nop); 
        
             
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop){
            //printf("\t HELLO 2\n");
            break;
        }
        
       //printf("\t HELLO 3\n");
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
        
        cycle+=1;

       //printf("\t HELLO 4\n");

        state = newState; /*** The end of the cycle and updates the current state with the values calculated in this cycle. csa23 ***/ 
             	
       //printf("\t HELLO 5\n");
    }//what's up


   //printf("OVERRR \n"); 


    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
    return 0;
}
