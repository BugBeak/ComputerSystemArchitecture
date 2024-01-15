/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size in bytes)  t=32-s-b
32 bit address (MSB -> LSB): TAG || SET || OFFSET

Tag Bits   : the tag field along with the valid bit is used to determine whether the block in the cache is valid or not.
Index Bits : the set index field is used to determine which set in the cache the block is stored in.
Offset Bits: the offset field is used to determine which byte in the block is being accessed.
*/

#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <tuple>
using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss
#define NOWRITEMEM 5 // no write to memory
#define WRITEMEM 6   // write to memory

struct config
{
    int L1blocksize;
    int L1setsize;
    int L1size;
    int L2blocksize;
    int L2setsize;
    int L2size;
};

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
// You need to define your cache class here, or design your own data structure for L1 and L2 cache

/*
A single cache block:
    - valid bit (is the data in the block valid?)
    - dirty bit (has the data in the block been modified by means of a write?)
    - tag (the tag bits of the address)
    - data (the actual data stored in the block, in our case, we don't need to store the data)
*/
struct CacheBlock
{
    // we don't actually need to allocate space for data, because we only need to simulate the cache action
    // or else it would have looked something like this: vector<number of bytes> Data; 
    bool valid, dirty;
    // vector<bool> tag;
    int tag;
};

/*
A CacheSet:
    - a vector of CacheBlocks
    - a counter to keep track of which block to evict next
*/
struct set
{
    // tips: 
    // Associativity: eg. resize to 4-ways set associative cache
    vector<CacheBlock> blocks;
    // int setSize;
    int counter;
};

struct MainMemory{

};
// You can design your own data structure for L1 and L2 cache; just an example here
// Tips: another example:


class Cache
{
    public:
    // set * CacheSet;
    int size, setSize, blockSize, offset_index, setSize_index;
    vector<set> sets;
    CacheBlock singleblock;
    set singleset;
    Cache(){};
    Cache(int sizee, int setSizee, int blockSizee){
        // initialize the cache according to cache parameters
        size = sizee;
        setSize = setSizee;
        blockSize = blockSizee;
        offset_index = std::log(blockSize)/std::log(2);
        setSize_index = std::log(size*1024/(setSize*blockSize))/std::log(2);
        singleblock.valid = false;
        singleblock.dirty = false;
        singleblock.tag = 0;
        singleset.counter = 0;
        for(int i=0; i<setSize;i++)
            singleset.blocks.push_back(singleblock);
        for(int i=0; i<size*1024/(setSize*blockSize);i++)
            sets.push_back(singleset);
    }
    int read_access(bitset<32> addr){
        int readState = RM;

        int tag, setindex;
        vector<bool> tag_vec, setindex_vec;
        for(int i = offset_index; i<offset_index + setSize_index; i++){
            setindex_vec.push_back(addr[i]);
        }
        for(int i = offset_index + setSize_index; i<addr.size(); i++){
            tag_vec.push_back(addr[i]);
        }
        setindex = accumulate(setindex_vec.rbegin(), setindex_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        tag = accumulate(tag_vec.rbegin(), tag_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        for(int i=0;i< setSize; i++){
            CacheBlock& it = sets[setindex].blocks[i];
        // }
        // for (CacheBlock& it : sets[setindex].blocks) { 
            if(it.valid){
                if(it.tag==tag){
                readState = RH;
                break;
                }
            }
        }

        return readState;
    };
    int write_access(bitset<32> addr){
        int writeState = WM;

        int tag, setindex;
        vector<bool> tag_vec, setindex_vec;
        for(int i = offset_index; i<offset_index + setSize_index; i++){
            setindex_vec.push_back(addr[i]);
        }
        for(int i = offset_index + setSize_index; i<addr.size(); i++){
            tag_vec.push_back(addr[i]);
        }
        setindex = accumulate(setindex_vec.rbegin(), setindex_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        tag = accumulate(tag_vec.rbegin(), tag_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        for(int i=0;i< setSize; i++){
            CacheBlock& it = sets[setindex].blocks[i];
        // }
        // for (CacheBlock& it : sets[setindex].blocks) { 
            if(it.valid){
                if(it.tag==tag){
                    sets[setindex].blocks[i].dirty = true;
                    writeState = WH;
                    break;
                }
            }
        }

        return writeState;
    };
    bool insert_first(bitset<32> addr, bool dirty){
        int tag, setindex;
        vector<bool> tag_vec, setindex_vec;
        for(int i = offset_index; i<offset_index + setSize_index; i++){
            setindex_vec.push_back(addr[i]);
        }
        for(int i = offset_index + setSize_index; i<addr.size(); i++){
            tag_vec.push_back(addr[i]);
        }
        setindex = accumulate(setindex_vec.rbegin(), setindex_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        tag = accumulate(tag_vec.rbegin(), tag_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        
        for (int i=0;i< setSize; i++){
            CacheBlock it = sets[setindex].blocks[i];
            if(!it.valid){
                sets[setindex].blocks[i].valid = true;
                sets[setindex].blocks[i].dirty = dirty;
                sets[setindex].blocks[i].tag = tag;
                return true;
            }
        }
        return false;
    };
    // tuple<bool, CacheBlock&> check_exist(bitset<32> addr){
    //     int tag, setindex;
    //     vector<bool> tag_vec, setindex_vec;
    //     for(int i = offset_index; i<offset_index + setSize_index; i++){
    //         setindex_vec.push_back(addr[i]);
    //     }
    //     for(int i = offset_index + setSize_index; i<addr.size(); i++){
    //         tag_vec.push_back(addr[i]);
    //     }
    //     setindex = accumulate(setindex_vec.rbegin(), setindex_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
    //     tag = accumulate(tag_vec.rbegin(), tag_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
    //     for(int i=0;i< setSize; i++){
    //         CacheBlock& it = sets[setindex].blocks[i];
    //     // }
    //     // for (CacheBlock& it : sets[setindex].blocks) { 
    //         if(it.valid){
    //             if(it.tag==tag){
    //             return std::make_tuple(true,std::ref(it));
    //             break;
    //             }
    //         }
    //     }
    //     CacheBlock& dummy = sets[setindex].blocks[0];
    //     return std::make_tuple(false,std::ref(dummy));
    // };
    bool evict(bitset<32> addr){
        int tag;
        int setindex;
        vector<bool> tag_vec;
        vector<bool> setindex_vec;
        for(int i = offset_index; i<offset_index + setSize_index; i++){
            setindex_vec.push_back(addr[i]);
        }
        for(int i = offset_index + setSize_index; i<addr.size(); i++){
            tag_vec.push_back(addr[i]);
        }
        // offset = accumulate(offset_vec.rbegin(), offset_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        setindex = accumulate(setindex_vec.rbegin(), setindex_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        tag = accumulate(tag_vec.rbegin(), tag_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });
        for (auto& it : sets[setindex].blocks) { 
            if(it.tag==tag && it.valid){
                it.valid = false;
                return it.dirty ;
            }
        }
        return false;
    };
    tuple<bitset<32>, bool> full_evict(bitset<32> addr){
        int setindex;
        vector<bool> setindex_vec;
        for(int i = offset_index; i<offset_index + setSize_index; i++){
            setindex_vec.push_back(addr[i]);
        }
        setindex = accumulate(setindex_vec.rbegin(), setindex_vec.rend(), 0, [](int x, int y) { return (x << 1) + y; });

        CacheBlock evictedBlock = sets[setindex].blocks[sets[setindex].counter];
        sets[setindex].blocks[sets[setindex].counter].valid = false;
        sets[setindex].counter++;
        if(sets[setindex].counter==setSize)
            sets[setindex].counter=0;
        bitset<32> addr_evict = addr;
        bitset<32> evictedBlock_tag = bitset<32>(evictedBlock.tag);
        int count= 0;
        for(int i = offset_index + setSize_index; i<addr.size(); i++){
            addr_evict[i] = evictedBlock_tag[count++];
        }
        return {addr_evict,evictedBlock.dirty};
    };
};


class CacheSystem
{
    Cache L1;
    Cache L2;
public:
    int L1AcceState, L2AcceState, MemAcceState;
    CacheSystem(config myconfig){
        L1 = Cache(myconfig.L1size, myconfig.L1setsize, myconfig.L1blocksize);
        L2 = Cache(myconfig.L2size, myconfig.L2setsize, myconfig.L2blocksize);
    };
    void read(bitset<32> addr, bool print=false){
        MemAcceState = NOWRITEMEM;
        L2AcceState = NA;
        L1AcceState = L1.read_access(addr);
        if(L1AcceState==RM){
            L2AcceState = L2.read_access(addr);
            if(print) cout<<"L2AcceState: "<<L2AcceState<<endl;
            bool insertPossibleL1;
            bool dirtybit = false;//if we're getting from memory
            if(L2AcceState==RH){//remove from L2 and push to L1
                dirtybit = L2.evict(addr);//move to L1
            }
            if(print) cout<<"dirtybit: "<<dirtybit<<endl;
            insertPossibleL1 =  L1.insert_first(addr, dirtybit);
            if(print) cout<<"insertPossibleL1: "<<insertPossibleL1<<endl;
            if(!insertPossibleL1){
                tuple<bitset<32>, bool> mytuple = L1.full_evict(addr);
                bitset<32> evictedBlockAddrL1 = get<0>(mytuple); 
                bool insertPossibleL2 =  L2.insert_first(evictedBlockAddrL1, get<1>(mytuple)); 
                if(print) cout<<"insertPossibleL2: "<<insertPossibleL2<<endl;
                if(!insertPossibleL2){
                    if(print) cout<<"    addr: "<<addr<<endl;
                    tuple<bitset<32>, bool> mytuple2 = L2.full_evict(evictedBlockAddrL1);
                    bitset<32> evictedBlockAddrL2 = get<0>(mytuple2);
                    if(print) cout<<"        get<1>(mytuple2): "<<get<1>(mytuple2)<<endl;
                    if( get<1>(mytuple2))//dirty bit
                        MemAcceState = WRITEMEM;
                    insertPossibleL2 =  L2.insert_first(evictedBlockAddrL1, get<1>(mytuple)); 
                }
                insertPossibleL1 =  L1.insert_first(addr, dirtybit); 
            }  
        }
    };
    void write(bitset<32> addr){
        L1AcceState = L1.write_access(addr);
        if(L1AcceState==WM)
            L2AcceState = L2.write_access(addr);
        else L2AcceState = NA;
        if(L2AcceState==WM) MemAcceState = WRITEMEM;
        else MemAcceState = NOWRITEMEM;
    };
};


/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/





int main(int argc, char *argv[])
{

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while (!cache_params.eof())                   // read config file
    {
        cache_params >> dummyLine;                // L1:
        cache_params >> cacheconfig.L1blocksize;  // L1 Block size
        cache_params >> cacheconfig.L1setsize;    // L1 Associativity
        cache_params >> cacheconfig.L1size;       // L1 Cache Size
        cache_params >> dummyLine;                // L2:
        cache_params >> cacheconfig.L2blocksize;  // L2 Block size
        cache_params >> cacheconfig.L2setsize;    // L2 Associativity
        cache_params >> cacheconfig.L2size;       // L2 Cache Size
    }
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    string line;
    string accesstype;     // the Read/Write access type from the memory trace;
    string xaddr;          // the address from the memory trace store in hex;
    unsigned int addr;     // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;




/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    if (cacheconfig.L1blocksize!=cacheconfig.L2blocksize){
        printf("please test with the same block size\n");
        return 1;
    }
    // cache c1(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size,
    //          cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
    CacheSystem myCacheSystem(cacheconfig);
    int L1AcceState = NA; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState = NA; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
    int MemAcceState = NOWRITEMEM; // Main Memory access state variable, can be either NA or WH;
    int count=1;
    if (traces.is_open() && tracesout.is_open())
    {
        while (getline(traces, line))
        { // read mem access file and access Cache
            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)){
                break;
            }
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32>(addr);

            // access the L1 and L2 Cache according to the trace;
            if (accesstype.compare("R") == 0)  // a Read request
            {
                // Implement by you:
                //   read access to the L1 Cache,
                //   and then L2 (if required),
                //   update the access state variable;
                //   return: L1AcceState L2AcceState MemAcceState

                myCacheSystem.read(accessaddr, false);
                L1AcceState = myCacheSystem.L1AcceState;
                L2AcceState = myCacheSystem.L2AcceState;
                MemAcceState = myCacheSystem.MemAcceState;
                // For example:
                // L1AcceState = cache.readL1(addr); // read L1
                // if(L1AcceState == RM){
                //     L2AcceState, MemAcceState = cache.readL2(addr); // if L1 read miss, read L2
                // }
                // else{ ... }
            }
            else{ // a Write request
                // Implement by you:
                //   write access to the L1 Cache, or L2 / main MEM,
                //   update the access state variable;
                //   return: L1AcceState L2AcceState
                myCacheSystem.write(accessaddr);
                L1AcceState = myCacheSystem.L1AcceState;
                L2AcceState = myCacheSystem.L2AcceState;
                MemAcceState = myCacheSystem.MemAcceState;
                // For example:
                // L1AcceState = cache.writeL1(addr);
                // if (L1AcceState == WM){
                //     L2AcceState, MemAcceState = cache.writeL2(addr);
                // }
                // else if(){...}
            }
/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/




            // Grading: don't change the code below.
            // We will print your access state of each cycle to see if your simulator gives the same result as ours.
            tracesout << L1AcceState << " " << L2AcceState << " " << MemAcceState << endl; // Output hit/miss results for L1 and L2 to the output file;
            count++;
        }
        traces.close();
        tracesout.close();
    }
    else
        cout << "Unable to open trace or traceout file ";

    return 0;
}
