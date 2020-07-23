#ifndef MAIN
#define MAIN

// Belinda Brown
// July, 2020
// Based on the material provided - Digital Computer Structures I
// And theoretical support in documentation from MJ Arce :)

// This algorithm represents a cache that receives the cache size
// in bytes, block size in bytes, associativity and a file of addresses,
// and determined that the last address is 0, Return the failure rate according
// to the addresses given for a policy of replacement LFU.


#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <assert.h>

// Execute by
// ./<program> <Mem bytes(#)> <Block size(#)> <associativity(#)> <file .h>


struct cacheBlock{
	int tag;
	bool valid;
	bool dirtyBit;
	int replacement;
};

//Generic Def
bool isPowerOf2(int x);
bool isHit(struct cacheBlock tags[], int index, int tag, int assoc, int *way);

//Def cache LRU
void updateLRU(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimLRU(struct cacheBlock tags[], int index, int assoc);

//Def cache LFU
void updateLFU(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimLFU(struct cacheBlock tags[], int index, int assoc);

//Def la cache LIFO
void updateLIFO(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimLIFO(struct cacheBlock tags[], int index, int assoc);

//Def cache FIFO
void updateFIFO(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimFIFO(struct cacheBlock tags[], int index, int assoc);

int main(int argc, char** argv)
{
	int cacheSize;
	int assoc;
	int blockSize;
	std::string addressesFile;
	int numTags; // number of blocks in cache
	int numSets;
	int offsetBits;
	int indexBits;
	int indexMask; // isolete bits index

    if(argc==5) //Obtain values from first input
    { // Begin argv[0]
			cacheSize=std::stoi(argv[1]);
			assert(isPowerOf2(cacheSize)&&"\n\nCache size must be power of 2!");

			assoc=std::stoi(argv[2]);
			assert(isPowerOf2(assoc)&&"\nAssoc must be power of 2!");
			assert(cacheSize>assoc&&"\nCache size must be greater than assoc!");

			blockSize=std::stoi(argv[3]);
			assert(isPowerOf2(blockSize)&&"\nBlock size must be power of 2!");
			assert(cacheSize>blockSize&&"\nCache size must be greater than blockSize!");
			// variable.<action>
			addressesFile.append(argv[4]);

        //std::cout<<cacheSize<<assoc<<blockSize<<addressesFile<<std::endl;
    }
    else{
			// Usage: ./ cache_size Assoc Block_size Accesses_file
			std::cout<<"\nUsage: ./<program> <Mem bytes(#)> <Block size(#)> <associativity(#)> <file .h>"<<std::endl;
    	return -1;
    }
		// Calculating some values
    numTags=cacheSize/blockSize;
    numSets=numTags/assoc;
    offsetBits=log2(blockSize);
    indexBits=log2(numSets);
    indexMask=numSets-1;
	// Output results
	std::cout<<"\nSimulating cache of "<<cacheSize<<" bytes, "<<numTags<<" blocks, "<<numSets<<" sets, "<<assoc<<" ways or associativity, "<<offsetBits<<" offset bits and "<<indexBits<<" index bits"<<std::endl;
	// Creating arrays
	// tags is the array size of numTags
	// It will be distribuid in groups
	// according with the asso number
	// if associativity x means n groups
 	// of x registers, and numSets is n
	struct cacheBlock *tagsLRU=new cacheBlock[numTags];
	struct cacheBlock *tagsLFU=new cacheBlock[numTags];
	struct cacheBlock *tagsLIFO=new cacheBlock[numTags];
	struct cacheBlock *tagsFIFO=new cacheBlock[numTags];
// ways is the rows and columns is the from i to n of the
// associativity number
	for(int i=0; i<numSets; i++){
		for(int j=0;j<assoc;j++){
			// j it's for moving in rows but in the same column
			tagsLRU[(i*assoc)+j].valid=0;
			tagsLRU[(i*assoc)+j].tag=0;
			tagsLRU[(i*assoc)+j].dirtyBit=0;
			tagsLRU[(i*assoc)+j].replacement=j;

			tagsLFU[(i*assoc)+j].valid=0;
			tagsLFU[(i*assoc)+j].tag=0;
			tagsLFU[(i*assoc)+j].dirtyBit=0;
			tagsLFU[(i*assoc)+j].replacement=0;

			tagsLIFO[(i*assoc)+j].valid=0;
			tagsLIFO[(i*assoc)+j].tag=0;
			tagsLIFO[(i*assoc)+j].dirtyBit=0;
			tagsLIFO[(i*assoc)+j].replacement=0;
			// couting time [old]
			tagsFIFO[(i*assoc)+j].valid=0;
			tagsFIFO[(i*assoc)+j].tag=0;
			tagsFIFO[(i*assoc)+j].dirtyBit=0;
			tagsFIFO[(i*assoc)+j].replacement=0;
		}
	}
	std::cout<<"\nArray's initial"<<std::endl;

	int accesses=0;
	int missesLRU=0;
	int missesLFU=0;
	int missesLIFO=0;
	int missesFIFO=0;


    //Create an input file stream
		// in is the file where will be the data
    std::ifstream in(addressesFile,std::ios::in);

    /*
     As long as we haven't reached the end of the file, keep reading entries.
    */
    int address;  //Variable to hold each number as it is read
            //Read number using the extraction (>>) operator
    while (in >> address) {
		//int myAccess = accesses[acc];
		int way;
		accesses++;
		// Have l direction [long]
		// we need of the direction the index
		// that give set number, need the offset
		// and the tag is the internal label to know
		// which group is.
		int index = (address>>offsetBits)&indexMask;
		int tag = (address>>(indexBits+offsetBits));

		// ifHit -> update if not get victim

		//Update cache LRU
		if(isHit(tagsLRU, index, tag, assoc, &way)==false)
		{
			way = getVictimLRU(tagsLRU, index, assoc);
			tagsLRU[(index*assoc)+way].tag=tag;
			tagsLRU[(index*assoc)+way].valid=true;
			missesLRU+=1;
		}
		updateLRU(tagsLRU, index, way, assoc);

		//Update LFU
		if(isHit(tagsLFU, index, tag, assoc, &way)==false)
		{
			way = getVictimLFU(tagsLFU, index, assoc);
			tagsLFU[(index*assoc)+way].tag=tag;
			tagsLFU[(index*assoc)+way].valid=true;
			missesLFU+=1;
		}
		updateLFU(tagsLFU, index, way, assoc);

		//Update  LIFO
		if(isHit(tagsLIFO, index, tag, assoc, &way)==false)
		{
			way = getVictimLIFO(tagsLIFO, index, assoc);
			tagsLIFO[(index*assoc)+way].tag=tag;
			tagsLIFO[(index*assoc)+way].valid=true;
			missesLIFO+=1;
		}
		updateLIFO(tagsLIFO, index, way, assoc);

		//Update  FIFO
		if(isHit(tagsFIFO, index, tag, assoc, &way)==false)
		{
			way = getVictimFIFO(tagsFIFO, index, assoc);
			tagsFIFO[(index*assoc)+way].tag=tag;
			tagsFIFO[(index*assoc)+way].valid=true;
			missesFIFO+=1;
			updateFIFO(tagsFIFO, index, way, assoc);
		}
    }
    //Close the file stream
    in.close();

	std::cout<<"\nResults LRU ideal\n"<<"Accesses= "<<accesses<<" Misses= "<<missesLRU<<" Miss rate= "<<(float)missesLRU/accesses<<std::endl;
	std::cout<<"\nResults LFU\n"<<"Accesses= "<<accesses<<" Misses= "<<missesLFU<<" Miss rate= "<<(float)missesLFU/accesses<<std::endl;
	std::cout<<"\nResults LIFO\n"<<"Accesses= "<<accesses<<" Misses= "<<missesLIFO<<" Miss rate= "<<(float)missesLIFO/accesses<<std::endl;
	std::cout<<"\nResults FIFO\n"<<"Accesses= "<<accesses<<" Misses= "<<missesFIFO<<" Miss rate= "<<(float)missesFIFO/accesses<<std::endl;

	delete [] tagsLRU;
	delete [] tagsLFU;
	delete [] tagsLIFO;
	delete [] tagsFIFO;

}

bool isHit(struct cacheBlock tags[], int index, int tag, int assoc, int *way){
	for(int i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].tag==tag&&tags[(index*assoc)+i].valid==true)
		{
			*way=i;
			return(true);
		}
	}
	return(false);
}

// Condition from the begin
bool isPowerOf2(int x)
{
	return !(x == 0) && !(x & (x - 1));
}

//LRU
void updateLRU(struct cacheBlock tags[], int index, int way, int assoc){
	for(int i=0; i<assoc;i++) //
	{
		if(tags[(index*assoc)+i].replacement<(assoc-1))
			tags[(index*assoc)+i].replacement+=1;
	}
	tags[(index*assoc)+way].replacement=0;
}

int getVictimLRU(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++) // For associativity values
	{
		if(tags[(index*assoc)+i].valid==false) // if not valid return i
		{
			return i;
		}
	}
	//look for the victim way
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].replacement==(assoc-1)) // if place is  before end associativity item
			return i;
	}
	return i;
}

//LFU
void updateLFU(struct cacheBlock tags[], int index, int way, int assoc){
	tags[(index*assoc)+way].replacement+=1;
}

///////////////////////// *********** ///////////////////////////
///////////////////////// *********** ///////////////////////////
///////////////////////// *********** ///////////////////////////
///////////////////////// *********** ///////////////////////////
// Counts the number of times each address has been accessed
// and removes those that have been accessed less frequently.
int getVictimLFU(struct cacheBlock tags[], int index, int assoc){
	//check empty way -> search if not valid
	int i;
	for(i=0;i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//----------look for the victim way ------------
	// ***. If there is a fault and if there is no counter at zero
	// then it lowers them all the same amount until it reaches
	// zero to some and eliminates the first zero it finds by
	// running it from right LS to left MS <-.
	int k =0;
	int z;
	int vic_counter = 0;
	// Each victim has the replacement bit in 1
	while(i<assoc){
		if(tags[(index*assoc)].replacement == 0){ // if not a victim returns the associativity
			return i;
		} // end if
		i += 1;
	} // end while

	while(k<assoc){
		if(tags[(index*assoc)+vic_counter].replacement > tags[(index*assoc)+k].replacement){
			vic_counter = k;
		}
		k+=1;
	}

	while(z<assoc){ // If none index == 0 --> sub 1 to index
		tags[(index*assoc)+z].replacement -= 1;
		z +=1;
		}

	return vic_counter;
}
///////////////////////// *********** ///////////////////////////
///////////////////////// *********** ///////////////////////////
///////////////////////// *********** ///////////////////////////
///////////////////////// *********** ///////////////////////////


//LIFO
void updateLIFO(struct cacheBlock tags[], int index, int way, int assoc){
	int i;
	for(i=0; i<assoc;i++)
	{
		tags[(index*assoc)+i].replacement=0;
	}
	tags[(index*assoc)+way].replacement=1;
}

int getVictimLIFO(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//look for the victim way
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].replacement==1)
			return i;
	}
	return i;
}

//FIFO
void updateFIFO(struct cacheBlock tags[], int index, int way, int assoc){
	int i;
	int greater=tags[(index*assoc)].replacement;
	for(i=1; i<assoc;i++)
	{
		if (greater<tags[(index*assoc)+i].replacement)
			greater=tags[(index*assoc)+i].replacement;
	}
	tags[(index*assoc)+way].replacement=greater+1;
}

int getVictimFIFO(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//look for the victim way
	int lower=0;
	for(i=1; i<assoc;i++)
	{
		if(tags[(index*assoc)+lower].replacement>tags[(index*assoc)+i].replacement)
			lower=i;
	}
	return lower;
}

#endif
