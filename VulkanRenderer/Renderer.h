#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include "glm/glm.hpp"
#include <array>

#include "ApplicationState.h"
#include "Texture.h"
#include "Mesh.h"

struct VSUniformBuffer
{
	glm::mat4 mModel;
	glm::mat4 mView;
	glm::mat4 mProjection;
};

struct Vertex
{
	glm::vec2 mPosition;
	glm::vec3 mColor;
	glm::vec2 mTexcoord;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		// Position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, mPosition);

		// Color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, mColor);

		// Texcoord
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, mTexcoord);

		return attributeDescriptions;
	}
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

	void SetAppState(AppState* appState);

	void WaitOnExecutionFinished();

	void RecreateSwapchain();

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CreateBuffer(VkDeviceSize size,
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer& buffer,
					  VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkCommandBuffer BeginSingleSubmissionCommands();

	void EndSingleSubmissionCommands(VkCommandBuffer commandBuffer);

private:

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location, 
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);

	static std::vector<char> ReadFile(const std::string& filename);

	void CreateInstance();

	void CreateDebugCallback();

	void CreateSurface();

	void PickPhysicalDevice();

	void CreateLogicalDevice();

	void CreateImageViews();

	void CreateRenderPass();

	void CreateDescriptorSetLayout();

	void CreateGraphicsPipeline();

	void CreateFramebuffers();

	void CreateCommandPool();

	void CreateCommandBuffers();

	void CreateSemaphores();

	void CreateUniformBuffer();

	void UpdateUniformBuffer();

	void CreateDescriptorPool();

	void CreateDescriptorSet();

	void CreateTextureImage();

	void DestroySwapchain();

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

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
	VkDescriptorSetLayout mDescriptorSetLayout;
	VkDescriptorPool mDescriptorPool;
	VkDescriptorSet mDescriptorSet;
	VkPipelineLayout mPipelineLayout;
	VkRenderPass mRenderPass;
	VkPipeline mGraphicsPipeline;
	VkCommandPool mCommandPool;

	VkSwapchainKHR mSwapchain;
	std::vector<VkImage> mSwapchainImages;
	std::vector<VkImageView> mSwapchainImageViews;
	VkFormat mSwapchainImageFormat;
	VkExtent2D mSwapchainExtent;

	std::vector<VkFramebuffer> mSwapchainFramebuffers;
	std::vector<VkCommandBuffer> mCommandBuffers;

	VkSemaphore mImageAvailableSemaphore;
	VkSemaphore mRenderFinishedSemaphore;

	VkBuffer mUniformBuffer;
	VkDeviceMemory mUniformBufferMemory;

	Texture mTexture;
	Mesh mMesh;

	AppState* mAppState;

	bool mInitialized;
};