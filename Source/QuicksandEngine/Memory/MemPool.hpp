#ifndef AR_MEMPOOL_HPP
#define AR_MEMPOOL_HPP



class CMemPool
{
	//an array of memory blocks, each split up into chunks
	unsigned char** m_ppRawMemoryArray;

	//the front of the memory chunk linked list
	unsigned char* m_pHead;

	//The size of each chunk and the number of chunks per array;
	unsigned int m_ChunkSize, m_NumChunks;

	//the number of elements in the memory array
	unsigned int m_MemArraySize;

	//true if we resize the memory pool when it fills
	bool m_ToAllowResize;

public:
		CMemPool(void);
		~CMemPool(void);

		bool Init(unsigned int chunkSize, unsigned int numChunks);
		void Destroy(void);

		void* Alloc(void);
		void Free(void *pMem);
		unsigned int GetChunkSize(void) const { return m_ChunkSize; }

		void SetAllowResize(bool toAllowResize){ m_ToAllowResize = toAllowResize; }


private:

	//reset initial variables
	void Reset(void);

	//internal memory allocation helpers
	bool GrowMemoryArray(void);
	unsigned char* AllocateNewMemoryBlock(void);

	// internal linked list managemnet
	unsigned char* GetNext(unsigned char* pBlock);
	void SetNext(unsigned char* pBlockToChange, unsigned char* pNewNext);

	//don't allot a copy constructor
	CMemPool(const CMemPool& memPool){}
};



#endif