#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct Allocation
{
	VkDeviceMemory mDeviceMemory;
	uint32_t mType;
	int32_t mID;
	VkDeviceSize mSize;
	VkDeviceSize mOffset;

	Allocation() :
		mDeviceMemory(VK_NULL_HANDLE),
		mType(0),
		mID(-1),
		mSize(0),
		mOffset(0)
	{

	}
};

struct MemoryChunk
{
	int32_t mID;
	int32_t mOffset;
	int32_t mSize;
	bool mFree;

	MemoryChunk() :
		mID(-1),
		mOffset(0),
		mSize(0),
		mFree(true)
	{

	}
};

struct MemoryBlock
{
	MemoryChunk* AllocateChunk(int32_t size);
	bool FreeChunk(int32_t id);

	MemoryBlock() :
		mDeviceMemory(0),
		mSize(0),
		mAvailableMemory(0),
		mLargestChunk(0),
		mMemoryType(0)
	{
		
	}

	std::vector<MemoryChunk> mChunks;
	VkDeviceMemory mDeviceMemory;
	int32_t mSize;
	int32_t mAvailableMemory;
	int32_t mLargestChunk;
	uint32_t mMemoryType;
};

class Allocator
{
public:

	static void Alloc(int32_t size, int32_t alignment, uint32_t memoryType, Allocation& outAllocation);
	static void Free(Allocation& allocation);

	static int32_t GetNumBlocksAllocated();
	static int32_t GetNumAllocations();
	static int32_t GetNumAllocatedBytes();

	static const int32_t sDefaultBlockSize;

private:

	static MemoryBlock* AllocateBlock(int32_t newBlockSize, uint32_t memoryType);
	static bool FreeBlock(MemoryBlock& block);


	static std::vector<MemoryBlock> sBlocks;
	static int32_t sNumAllocations;
	static int32_t sNumAllocatedBytes;
};