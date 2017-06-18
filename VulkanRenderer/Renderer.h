#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include "glm/glm.hpp"
#include <array>

#include "ApplicationState.h"
#include "Texture.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Scene.h"

#include "Pipeline.h"
#include "EarlyDepthPipeline.h"
#include "GeometryPipeline.h"
#include "LightPipeline.h"
#include "DeferredPipeline.h"

struct DeferredUniformBuffer
{
	glm::vec4 mLightDirection;
	glm::vec4 mLightColor;
};

struct QueueFamilyIndices
{
	int32_t mGraphicsFamily = -1;
	int32_t mPresentFamily = -1;

	bool IsComplete()
	{
		return mGraphicsFamily >= 0 &&
			mPresentFamily >= 0;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Renderer
{
public:

	static void Create();
	static void Destroy();
	static Renderer* Get();

	~Renderer();

	void Initialize();

	VkDevice GetDevice();

	void CreateSwapchain();

	void PreparePresentation();

	void Render();

	void SetScene(Scene* scene);

	void SetAppState(AppState* appState);

	void WaitOnExecutionFinished();

	void RecreateSwapchain();

	VkDescriptorPool GetDescriptorPool();

	Pipeline& GetGeometryPipeline();
	Pipeline& GetLightPipeline();
	Pipeline& GetDeferredPipeline();

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CreateBuffer(VkDeviceSize size,
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer& buffer,
					  VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkCommandBuffer BeginSingleSubmissionCommands();

	void EndSingleSubmissionCommands(VkCommandBuffer commandBuffer);

	VkExtent2D& GetSwapchainExtent();

	VkRenderPass GetRenderPass();

private:

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location, 
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);

	void CreateInstance();

	void CreateDebugCallback();

	void CreateSurface();

	void PickPhysicalDevice();

	void CreateLogicalDevice();

	void CreateImageViews();

	void CreateGBuffer();

	void CreateGBufferImages();

	void CreateGBufferAttachment(GBufferIndex index, VkFormat format);

	void CreateGBufferSampler();

	void CreateDeferredUniformBuffer();

	void UpdateDeferredUniformBuffer();

	void CreateDeferredDescriptorSet();

	void CreateRenderPass();

	void CreatePipelines();

	void CreateFramebuffers();

	void CreateCommandPool();

	void CreateCommandBuffers();

	void CreateSemaphores();

	void CreateDescriptorPool();

	void CreateDepthImage();

	void DestroySwapchain();

	bool IsDeviceSuitable(VkPhysicalDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	bool CheckValidationLayerSupport(const char** layers,
									 uint32_t count);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice device, 
									 const char** extensions,
									 uint32_t count);

	void DestroyDebugCallback();

	static Renderer* sInstance;

	Renderer();

	VkInstance mInstance;
	VkDebugReportCallbackEXT mCallback;
	VkPhysicalDevice mPhysicalDevice;
	VkDevice mDevice;
	VkQueue mGraphicsQueue;
	VkQueue mPresentQueue;
	VkSurfaceKHR mSurface;

	VkDescriptorPool mDescriptorPool;

	VkRenderPass mRenderPass;
	VkCommandPool mCommandPool;

	// Swapchain images
	VkSwapchainKHR mSwapchain;
	std::vector<VkImage> mSwapchainImages;
	std::vector<VkImageView> mSwapchainImageViews;
	VkFormat mSwapchainImageFormat;
	VkExtent2D mSwapchainExtent;

	std::vector<VkFramebuffer> mSwapchainFramebuffers;
	std::vector<VkCommandBuffer> mCommandBuffers;

	// Depth image
	VkImage mDepthImage;
	VkDeviceMemory mDepthImageMemory;
	VkImageView mDepthImageView;

	// GBuffer images
	std::vector<VkImage> mGBufferImages;
	std::vector<VkDeviceMemory> mGBufferImageMemory;
	std::vector<VkImageView> mGBufferImageViews;
	std::vector<VkFormat> mGBufferFormats;
	VkFramebuffer mGBuffer;
	VkSampler mGBufferSampler;

	VkSemaphore mImageAvailableSemaphore;
	VkSemaphore mRenderFinishedSemaphore;

	EarlyDepthPipeline mEarlyDepthPipeline;
	GeometryPipeline mGeometryPipeline;
	LightPipeline mLightPipeline;
	DeferredPipeline mDeferredPipeline;

	VkDescriptorSet mDeferredDescriptorSet;
	VkBuffer mDeferredUniformBuffer;
	VkDeviceMemory mDeferredUniformBufferMemory;

	Scene* mScene;

	AppState* mAppState;

	bool mInitialized;
};