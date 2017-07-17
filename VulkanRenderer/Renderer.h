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
#include "PipelineConfigs.h"
#include "GBuffer.h"

#include "ShadowCaster.h"

struct GlobalUniformData
{
	glm::vec4 mSunDirection;
	glm::vec4 mSunColor;
	glm::vec4 mViewPosition;
	glm::vec2 mScreenDimensions;
	int32_t mVisualizationMode;
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

	EarlyDepthPipeline& GetEarlyDepthPipeline();
	GeometryPipeline& GetGeometryPipeline();
	LightPipeline& GetLightPipeline();
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

	VkFormat GetSwapchainFormat();

	VkRenderPass GetRenderPass();

	void SetVisualizationMode(int32_t mode);

	void SetDirectionalLightColor(glm::vec4 color);

	void SetDirectionalLightDirection(glm::vec3 direction);

	void SetDebugMode(DebugMode mode);

	GBuffer& GetGBuffer();

	VkDescriptorSet& GetGlobalDescriptorSet();

	VkDescriptorSet& GetDeferredDescriptorSet();

	void CreateCommandBuffers();

    void SetEnvironmentDebugFace(uint32_t index);

    void UpdateGlobalUniformBuffer();

    void UpdateGlobalUniformData();

    GlobalUniformData& GetGlobalUniformData();

    void UpdateEnvironmentCaptures();

    void UpdateDeferredDescriptorSet();

	void RenderShadowMaps();

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

	void CreateGlobalUniformBuffer();

	void CreateGlobalDescriptorSet();

	void CreateDebugDescriptorSet();

	void UpdateDebugDescriptorSet();

	void CreateRenderPass();

	void CreatePipelines();

	void CreateFramebuffers();

	void CreateCommandPool();

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
	VkSemaphore mImageAvailableSemaphore;
	VkSemaphore mRenderFinishedSemaphore;

	EarlyDepthPipeline mEarlyDepthPipeline;
	GeometryPipeline mGeometryPipeline;
	LightPipeline mLightPipeline;
	DebugDeferredPipeline mDebugDeferredPipeline;
	EnvironmentCaptureDebugPipeline mEnvironmentCaptureDebugPipeline;
	ShadowMapDebugPipeline mShadowMapDebugPipeline;

	VkDescriptorSet mGlobalDescriptorSet;
	VkBuffer mGlobalUniformBuffer;
	VkDeviceMemory mGlobalUniformBufferMemory;

	VkDescriptorSet mDeferredDescriptorSet;
	VkDescriptorSet mDebugDescriptorSet;

	GlobalUniformData mGlobalUniformData;

	GBuffer mGBuffer;

	Scene* mScene;

	AppState* mAppState;

	DebugMode mDebugMode;

	bool mInitialized;

    uint32_t mEnvironmentDebugFace;

	ShadowCaster mShadowCaster;
};