#include "Allocator.h"
#include <assert.h>

const int32_t Allocator::sDefaultBlockSize = 16777216; // 16 MB Blocks

MemoryChunk* MemoryBlock::AllocateChunk(int32_t size)
{
	MemoryChunk* chunk = nullptr;

	for (int32_t j = 0; j < mChunks.size(); ++j)
	{
		if (mChunks[j].mFree &&
			mChunks[j].mSize >= size)
		{
			chunk = &mChunks[j];
			break;
		}
	}

	return chunk;
}

void MemoryBlock::FreeChunk(int32_t id)
{

}

void Allocator::Alloc(int32_t size, int32_t alignment, Allocation& outAllocation)
{
	MemoryBlock* block = nullptr;
	MemoryChunk* chunk = nullptr;

	int32_t maxAlignSize = size + alignment;
	for (int32_t i = 0; i < sBlocks.size(); ++i)
	{
		chunk = sBlocks[i].AllocateChunk(maxAlignSize);

		if (chunk != nullptr)
		{
			block = &sBlocks[i];
			break;
		}
	}

	if (chunk == nullptr)
	{
		int32_t newBlockSize = maxAlignSize > sDefaultBlockSize ? maxAlignSize : sDefaultBlockSize;
		block = AllocateNewBlock(newBlockSize);
		assert(block);

		chunk = block->AllocateChunk(maxAlignSize);
	}

	assert(chunk);

	outAllocation.mDeviceMemory = block->mDeviceMemory;
	outAllocation.mID = sNumTotalAllocs++;
	outAllocation.mOffset = chunk->mOffset;
	outAllocation.mSize = size;
	outAllocation.mType = 0; //??? like which block type I guess. memory type
}

void Allocator::Free()
{

}

int32_t Allocator::GetNumBlocksAllocated()
{
	return sBlocks.size();
}

int32_t Allocator::GetNumAllocations()
{
	return sNumAllocations;
}

int32_t Allocator::GetNumAllocatedBytes()
{
	return sNumAllocatedBytes;
}

MemoryBlock* Allocator::AllocateNewBlock(int32_t newBlockSize)
{

}