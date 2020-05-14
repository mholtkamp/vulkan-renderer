#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include "glm/glm.hpp"
#include <array>

#include "ApplicationState.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Scene.h"

#include "Pipeline.h"
#include "PipelineConfigs.h"
#include "GBuffer.h"

#include "ShadowCaster.h"

struct GlobalUniformData
{
    glm::mat4 mSunVP;
	glm::vec4 mSunDirection;
	glm::vec4 mSunColor;
	glm::vec4 mViewPosition;
	glm::vec2 mScreenDimensions;
	float mShadowIntensity;
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

	Scene* GetScene();

	void SetAppState(AppState* appState);

	void WaitOnExecutionFinished();

	VkPhysicalDevice GetPhysicalDevice();

	void RecreateSwapchain();

	VkDescriptorPool GetDescriptorPool();

	EarlyDepthPipeline& GetEarlyDepthPipeline();
	GeometryPipeline& GetGeometryPipeline();
	LightPipeline& GetLightPipeline();
	Pipeline& GetDeferredPipeline();
	QuadPipeline& GetQuadPipeline();
	TextPipeline& GetTextPipeline();

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CreateBuffer(VkDeviceSize size,
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer& buffer,
					  Allocation& bufferMemory);

	void CreateVertexBuffer(void* vertexData, uint32_t vertexSize, uint32_t numVertices, VkBuffer& outVertexBuffer, Allocation& outVertexBufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkCommandBuffer BeginSingleSubmissionCommands();

	void EndSingleSubmissionCommands(VkCommandBuffer commandBuffer, bool waitForIdle = true);

	VkExtent2D& GetSwapchainExtent();

	VkFormat GetSwapchainFormat();

	VkRenderPass GetRenderPass();

	void SetVisualizationMode(int32_t mode);

	void SetDirectionalLightColor(glm::vec4 color);

	void SetDirectionalLightDirection(glm::vec3 direction);

	void SetDebugMode(DebugMode mode);

	void SetViewportAndScissor(VkCommandBuffer cb, int32_t x, int32_t y, int32_t width, int32_t height);

	GBuffer& GetGBuffer();

	VkDescriptorSet& GetGlobalDescriptorSet();

	VkDescriptorSet& GetDeferredDescriptorSet();

	VkDescriptorSet& GetPostProcessDescriptorSet();

	void CreateCommandBuffers();

    void SetEnvironmentDebugFace(uint32_t index);

    void UpdateGlobalDescriptorSet();

    void UpdateGlobalUniformData();

    GlobalUniformData& GetGlobalUniformData();

    void UpdateEnvironmentCaptures();

    void UpdateDeferredDescriptorSet();

    VkImageView GetShadowMapImageView();

    VkSampler GetShadowMapSampler();

	void ToggleIrradianceDebug();

	void ToggleEnvironmentCaptureDebug();

	void SetInterfaceResolution(glm::vec2 newResolution);
	glm::vec2 GetInterfaceResolution() const;

	Texture2D* GetBlackTexture();
	TextureCube* GetBlackCubemap();

	Material* GetDefaultMaterial();

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

	void CreateDefaultTextures();

	void CreatePipelines();

	void DestroyPipelines();

	void CreateFramebuffers();

	void CreateCommandPool();

	void CreateSemaphores();

	void UpdateInputEnabled();

	void CreateDescriptorPool();

	void CreateDepthImage();

	void CreatePostProcessDescriptorSet();

	void CreateLitColorImage();

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

	void DestroyDefaultTextures();

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
	Allocation mDepthImageMemory;
	VkImageView mDepthImageView;

	// Lit Color image
	VkImage mLitColorImage;
	Allocation mLitColorImageMemory;
	VkImageView mLitColorImageView;
	VkSampler mLitColorSampler;
	VkFormat mLitColorImageFormat;

	VkSemaphore mImageAvailableSemaphore;
	VkSemaphore mRenderFinishedSemaphore;

	EarlyDepthPipeline mEarlyDepthPipeline;
	GeometryPipeline mGeometryPipeline;
	LightPipeline mLightPipeline;
    DirectionalLightPipeline mDirectionalLightPipeline;
	DebugDeferredPipeline mDebugDeferredPipeline;
	EnvironmentCaptureDebugPipeline mEnvironmentCaptureDebugPipeline;
	ShadowMapDebugPipeline mShadowMapDebugPipeline;
	PostProcessPipeline mPostProcessPipeline;
	NullPostProcessPipeline mNullPostProcessPipeline;
	QuadPipeline mQuadPipeline;
	TextPipeline mTextPipeline;

	VkDescriptorSet mGlobalDescriptorSet;
	VkBuffer mGlobalUniformBuffer;
	Allocation mGlobalUniformBufferMemory;

	VkDescriptorSet mDeferredDescriptorSet;
	VkDescriptorSet mDebugDescriptorSet;
	VkDescriptorSet mPostProcessDescriptorSet;

	GlobalUniformData mGlobalUniformData;

	GBuffer mGBuffer;

	Scene* mScene;

	AppState* mAppState;

	DebugMode mDebugMode;

	bool mInitialized;

    uint32_t mEnvironmentDebugFace;

	ShadowCaster mShadowCaster;
	Material mDefaultMaterial;

	glm::vec2 mInterfaceResolution;

	public:

	// Default Resources
	Texture2D mWhiteTexture;
	Texture2D mBlackTexture;
	TextureCube mBlackCubemap;
	TextureCube mGreenCubemap;
};