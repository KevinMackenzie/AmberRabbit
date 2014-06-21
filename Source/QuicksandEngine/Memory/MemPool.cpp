#include "../Stdafx.hpp"
#include "MemPool.hpp"


const static size_t CHUNK_HEADER_SIZE = (sizeof(unsigned char*));


CMemPool::CMemPool(void)
{
	Reset();
}

CMemPool::~CMemPool(void)
{
	Destroy();
}

bool CMemPool::GrowMemoryArray(void)
{
	//allocate new array
	size_t allocationSize = sizeof(unsigned char*) * (m_MemArraySize + 1);
	unsigned char** ppNewMemArray = (unsigned char**)malloc(allocationSize);

	//make sure the allocation succeeded
	if (!ppNewMemArray)
		return false;

	//copy any existing memory pointers over
	for (unsigned int i = 0; i < m_MemArraySize; ++i)
		ppNewMemArray[i] = m_ppRawMemoryArray[i];

	//allocate a new block of memory.  Indexing m_MemArraySize here is
	//safe becuase we haven't incremented it yet to reflect ht enew size
	ppNewMemArray[m_MemArraySize] = AllocateNewMemoryBlock();

	//attach the block to the end of the current memory list
	if (m_pHead)
	{
		unsigned char* pCurr = m_pHead;
		unsigned char* pNext = GetNext(m_pHead);
		while (pNext)
		{
			pCurr = pNext;
			pNext = GetNext(pNext);
		}
		SetNext(pCurr, ppNewMemArray[m_MemArraySize]);
	}
	else
	{
		m_pHead = ppNewMemArray[m_MemArraySize];
	}

	//destroy the old memory array
	if (m_ppRawMemoryArray)
		free(m_ppRawMemoryArray);

	//assign the new memory array and increment the size cound
	m_ppRawMemoryArray = ppNewMemArray;
	++m_MemArraySize;

	return true;
}

unsigned char* CMemPool::AllocateNewMemoryBlock(void)
{
	//calculate the size of each block and the size of the actual memory allocation
	size_t blockSize = m_ChunkSize + CHUNK_HEADER_SIZE;
	//overhead
	size_t trueSize = blockSize * m_NumChunks;

	//allocate the memory
	unsigned char* pNewMem = (unsigned char*)malloc(trueSize);
	if (!pNewMem)
		return nullptr;

	//turn the memory into a linked list of chunks
	unsigned char* pEnd = pNewMem + trueSize;
	unsigned char* pCurr = pNewMem;
	while (pCurr < pEnd)
	{
		//calculate the next pointer position
		unsigned char* pNext = pCurr + blockSize;

		//set the next pointer
		unsigned char** ppChunkHeader = (unsigned char**)pCurr;
		ppChunkHeader[0] = (pNext < pEnd ? pNext : nullptr);

		//move to the next block
		pCurr += blockSize;
	}

	return pNewMem;
}

void* CMemPool::Alloc(void)
{
	//if we are out of memory chunks, grow the pool (EXPENSIVE)
	if (!m_pHead)
	{
		if (!m_ToAllowResize)
			return nullptr;

		if (!GrowMemoryArray())
			return nullptr;
	}

	//grab the first chunk from the list and move to the next chunks
	unsigned char* pRet = m_pHead;
	m_pHead = GetNext(m_pHead);
	return (pRet + CHUNK_HEADER_SIZE);//make sure we return a pointer to the data section only
}

void CMemPool::Free(void* pMem)
{
	//calling Free() on a null pointer is perfectly valid in C++ so we must check for it
	if (pMem != nullptr)
	{
		//the pointer we get back is just to the data section of the chunk.  This gets us the full chunk
		unsigned char* pBlock = ((unsigned char*)pMem) - CHUNK_HEADER_SIZE;

		//push the chunk of the front of the list
		SetNext(pBlock, m_pHead);
		m_pHead = pBlock;
	}
}

void CMemPool::Reset(void)
{
	m_ppRawMemoryArray = nullptr;
	m_pHead = nullptr;
	m_ChunkSize = 0;
	m_NumChunks = 0;
	m_MemArraySize = 0;
	m_ToAllowResize = true;
}

unsigned char* CMemPool::GetNext(unsigned char* pBlock)
{
	unsigned char** ppChunkHeader = (unsigned char**)pBlock;
	return ppChunkHeader[0];
}

void CMemPool::SetNext(unsigned char* pBlockToChange, unsigned char* pNewNext)
{
	unsigned char** ppChunkHeader = (unsigned char**)pBlockToChange;
	ppChunkHeader[0] = pNewNext;
}

