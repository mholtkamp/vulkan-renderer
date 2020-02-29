#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct Allocation
{
	VkDeviceMemory mDeviceMemory;
	uint32_t mType;
	uint32_t mID;
	VkDeviceSize mSize;
	VkDeviceSize mOffset;
};

struct MemoryBlock
{
	MemoryChunk* AllocateChunk(int32_t size);
	void FreeChunk(int32_t id);

	MemoryBlock() :
		mDeviceMemory(0),
		mSize(0),
		mAvailableMemory(0),
		mLargestChunk(0)
	{
		
	}

	std::vector<MemoryChunk> mChunks;
	VkDeviceMemory mDeviceMemory;
	int32_t mSize;
	int32_t mAvailableMemory;
	int32_t mLargestChunk;
};

struct MemoryChunk
{
	int32_t mOffset;
	int32_t mSize;
	bool mFree;
};

class Allocator
{
public:

	static void Alloc(int32_t size, int32_t alignment, Allocation& outAllocation);
	static void Free();

	static int32_t GetNumBlocksAllocated();
	static int32_t GetNumAllocations();
	static int32_t GetNumAllocatedBytes();

	static const int32_t sDefaultBlockSize;

private:

	static MemoryBlock* AllocateNewBlock(int32_t newBlockSize);

	static std::vector<MemoryBlock> sBlocks;
	static int32_t sNumAllocations;
	static int32_t sNumAllocatedBytes;
};