#include "Allocator.h"
#include "Renderer.h"
#include <assert.h>
#include <exception>

const int32_t Allocator::sDefaultBlockSize = 16777216; // 16 MB Blocks

int32_t Allocator::sNumAllocations = 0;
int32_t Allocator::sNumAllocatedBytes = 0;

static int32_t sNumChunksAllocated = 0;

MemoryChunk* MemoryBlock::AllocateChunk(int32_t size)
{
	MemoryChunk* chunk = nullptr;

	for (int32_t j = 0; j < mChunks.size(); ++j)
	{
		if (mChunks[j].mFree &&
			mChunks[j].mSize >= size)
		{
			// We found a chunk that is big enough, now let's split the chunk into two.
			int32_t extraSize = mChunks[j].mSize - size;

			if (extraSize > 0)
			{
				mChunks[j].mSize = size;
				
				MemoryChunk extraChunk;
				extraChunk.mFree = true;
				extraChunk.mOffset = mChunks[j].mOffset + mChunks[j].mSize;
				extraChunk.mSize = extraSize;
				extraChunk.mID = -1;

				mChunks.insert(mChunks.begin() + j + 1, extraChunk);
			}

			mChunks[j].mFree = false;
			mChunks[j].mID = sNumChunksAllocated++;

			assert(mChunks[j].mID > 0); // Did we overflow int32_t?

			// make sure we grab the pointer after inserting the new chunk (as it may reallocate the data).
			chunk = &mChunks[j];
			break;
		}
	}

	return chunk;
}

bool MemoryBlock::FreeChunk(int32_t id)
{
	bool bFreed = false;

	for (int32_t j = 0; j < mChunks.size(); ++j)
	{
		if (mChunks[j].mID == id)
		{
			// Mark this chunk as freed
			mChunks[j].mFree = true;
			
			// See if we can merge this chunk
			// First try merging it with the next chunk in the list
			if (j < mChunks.size() - 1 &&
				mChunks[j + 1].mFree)
			{
				mChunks[j].mSize += mChunks[j + 1].mSize;
				mChunks.erase(mChunks.begin() + j + 1);
			}

			// Now try merging with the previous chunk
			if (j > 0 &&
				mChunks[j - 1].mFree)
			{
				mChunks[j - 1].mSize += mChunks[j].mSize;
				mChunks.erase(mChunks.begin() + j);
			}
		}
	}
}

void Allocator::Alloc(int32_t size, int32_t alignment, uint32_t memoryType, Allocation& outAllocation)
{
	MemoryBlock* block = nullptr;
	MemoryChunk* chunk = nullptr;

	int32_t maxAlignSize = size + alignment;
	for (int32_t i = 0; i < sBlocks.size(); ++i)
	{
		if (sBlocks[i].mMemoryType == memoryType)
		{
			chunk = sBlocks[i].AllocateChunk(maxAlignSize);

			if (chunk != nullptr)
			{
				block = &sBlocks[i];
				break;
			}
		}
	}

	if (chunk == nullptr)
	{
		int32_t newBlockSize = maxAlignSize > sDefaultBlockSize ? maxAlignSize : sDefaultBlockSize;
		block = AllocateBlock(newBlockSize, memoryType);
		assert(block);

		chunk = block->AllocateChunk(maxAlignSize);
	}

	assert(chunk);

	outAllocation.mDeviceMemory = block->mDeviceMemory;
	outAllocation.mID = chunk->mID;
	outAllocation.mOffset = ((chunk->mOffset + alignment - 1) / alignment) * alignment;
	outAllocation.mSize = size;
	outAllocation.mType = block->mMemoryType;

	sNumAllocations++;
	sNumAllocatedBytes += maxAlignSize;
}

void Allocator::Free(Allocation& allocation)
{
	sNumAllocations--;
	sNumAllocatedBytes -= allocation.mSize;

	bool bFreed = false;
	for (int32_t i = 0; i < sBlocks.size(); ++i)
	{
		if (sBlocks[i].mMemoryType == allocation.mType)
		{
			bFreed = sBlocks[i].FreeChunk(allocation.mID);

			if (bFreed)
			{
				// If the block is entirely free, deallocate the memory.
				if (sBlocks[i].mChunks.size() == 1)
				{
					assert(sBlocks[i].mChunks[0].mFree);
					FreeBlock(sBlocks[i]);
				}
				break;
			}
		}
	}

	assert(bFreed);

	allocation.mDeviceMemory = VK_NULL_HANDLE;
	allocation.mID = -1;
	allocation.mOffset = 0;
	allocation.mSize = 0;
	allocation.mType = 0;
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

MemoryBlock* Allocator::AllocateBlock(int32_t newBlockSize, uint32_t memoryType)
{
	sBlocks.push_back(MemoryBlock());
	MemoryBlock& newBlock = sBlocks.back();

	newBlock.mSize = newBlockSize;
	newBlock.mAvailableMemory = newBlockSize;
	newBlock.mLargestChunk = newBlockSize;
	newBlock.mMemoryType = memoryType;

	// Allocate video memory.
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = newBlockSize;
	allocInfo.memoryTypeIndex = Renderer::Get()->FindMemoryType(memoryType, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(Renderer::Get()->GetDevice(), &allocInfo, nullptr, &newBlock.mDeviceMemory) != VK_SUCCESS)
	{
		throw std::exception("Failed to allocate image memory");
	}

	// Initialize the starting chunk.
	MemoryChunk firstChunk;
	firstChunk.mFree = true;
	firstChunk.mID = -1;
	firstChunk.mOffset = 0;
	firstChunk.mSize = newBlockSize;
	newBlock.mChunks.push_back(firstChunk);
}

bool Allocator::FreeBlock(MemoryBlock& block)
{
	int32_t index = 0;

	for (index = 0; index < sBlocks.size(); ++index)
	{
		if (block.mDeviceMemory == sBlocks[index].mDeviceMemory)
		{
			break;
		}
	}

	assert(index < sBlocks.size());

	vkFreeMemory(Renderer::Get()->GetDevice(), sBlocks[index].mDeviceMemory, nullptr);
	sBlocks.erase(sBlocks.begin() + index);
}