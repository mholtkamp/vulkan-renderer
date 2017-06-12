#include "Renderer.h"
#include "ApplicationInfo.h"
#include "Util.h"

#include <assert.h>
#include <stdlib.h>
#include <exception>
#include <stdio.h>
#include <vector>
#include <set>
#include <fstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <chrono>

#undef min
#undef max

using namespace std;
using namespace std::chrono;

static const char* sValidationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
static uint32_t sNumValidationLayers = 1;

static const char* sDeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
static uint32_t sNumDeviceExtensions = 1;

Renderer* Renderer::sInstance = nullptr;

Renderer::Renderer() :
	mInstance(0),
	mCallback(0),
	mPhysicalDevice(0),
	mDevice(0),
	mGraphicsQueue(0),
	mPresentQueue(0),
	mSurface(0),
	mSwapchain(0),
	mRenderPass(0),
	mImageAvailableSemaphore(0),
	mRenderFinishedSemaphore(0),
	mScene(nullptr),
	mInitialized(false)
{
	
}

void Renderer::Create()
{
	Destroy();

	sInstance = new Renderer();
}

void Renderer::Destroy()
{
	if (sInstance != nullptr)
	{
		delete sInstance;
		sInstance = nullptr;
	}
}

Renderer* Renderer::Get()
{
	return sInstance;
}

void Renderer::SetAppState(AppState* appState)
{
	mAppState = appState;
}

void Renderer::WaitOnExecutionFinished()
{
	vkDeviceWaitIdle(mDevice);
}

void Renderer::DestroySwapchain()
{
	for (size_t i = 0; i < mSwapchainFramebuffers.size(); ++i)
	{
		vkDestroyFramebuffer(mDevice, mSwapchainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

	vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
	vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

	for (size_t i = 0; i < mSwapchainImageViews.size(); ++i)
	{
		vkDestroyImageView(mDevice, mSwapchainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
}

Renderer::~Renderer()
{
	DestroySwapchain();

	mTexture.Destroy();

	vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);
	vkDestroyBuffer(mDevice, mUniformBuffer, nullptr);
	vkFreeMemory(mDevice, mUniformBufferMemory, nullptr);

	mMesh.Destroy();

	vkDestroySemaphore(mDevice, mRenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(mDevice, mImageAvailableSemaphore, nullptr);

	vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

	DestroyDebugCallback();

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyDevice(mDevice, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

void Renderer::Initialize()
{
	CreateInstance();
	CreateDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandPool();

	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSet();
	CreateCommandBuffers();
	CreateSemaphores();

	mInitialized = true;
}

VkDevice Renderer::GetDevice()
{
	return mDevice;
}

void Renderer::CreateSwapchain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = 2;

	if (imageCount < swapChainSupport.capabilities.minImageCount)
		imageCount = swapChainSupport.capabilities.minImageCount;
	if (swapChainSupport.capabilities.maxImageCount != 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR ciSwapchain = {};
	ciSwapchain.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	ciSwapchain.surface = mSurface;

	ciSwapchain.minImageCount = imageCount;
	ciSwapchain.imageFormat = surfaceFormat.format;
	ciSwapchain.imageColorSpace = surfaceFormat.colorSpace;
	ciSwapchain.imageExtent = extent;
	ciSwapchain.imageArrayLayers = 1;
	ciSwapchain.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// TODO: Replace VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT with
	//   VK_IMAGE_USAGE_TRANSFER_DST_BIT for deferred renderer output

	QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.mGraphicsFamily, (uint32_t)indices.mPresentFamily };

	if (indices.mGraphicsFamily != indices.mPresentFamily)
	{
		ciSwapchain.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		ciSwapchain.queueFamilyIndexCount = 2;
		ciSwapchain.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		ciSwapchain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		ciSwapchain.queueFamilyIndexCount = 0;
		ciSwapchain.pQueueFamilyIndices = nullptr;
	}

	ciSwapchain.preTransform = swapChainSupport.capabilities.currentTransform;
	ciSwapchain.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	ciSwapchain.presentMode = presentMode;
	ciSwapchain.clipped = VK_TRUE;
	ciSwapchain.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(mDevice, &ciSwapchain, nullptr, &mSwapchain) != VK_SUCCESS)
	{
		throw exception("Failed to create swapchain");
	}

	vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, nullptr);
	mSwapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, mSwapchainImages.data());

	mSwapchainImageFormat = surfaceFormat.format;
	mSwapchainExtent = extent;
}

void Renderer::PreparePresentation()
{

}

void Renderer::Render()
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(mDevice, mSwapchain, std::numeric_limits<uint64_t>::max(), mImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS &&
		result != VK_SUBOPTIMAL_KHR)
	{
		throw exception("Failed to acquire swapchain image");
	}

	UpdateUniformBuffer();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		throw exception("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapchains[] = { mSwapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(mPresentQueue, &presentInfo);

	// TODO: Perhaps only wait if validation layers are enabled.
	vkQueueWaitIdle(mPresentQueue);
}

void Renderer::SetScene(const Scene* scene)
{
	if (!mInitialized)
	{
		Renderer::Initialize();
	}

	if (mScene != scene)
	{
		CreateCommandBuffers()
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData)
{
	printf("Validation Layer ");
	printf(msg);

	OutputDebugString("Validation Layer: ");
	OutputDebugString(msg);
	OutputDebugString("\n");

	return VK_FALSE;
}

std::vector<char> Renderer::ReadFile(const std::string& filename)
{
	ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw exception("Failed to open file.");
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);

	file.read(buffer.data(), fileSize);

	return buffer;
}

void Renderer::CreateInstance()
{
	VkResult result;
	VkBool32 surfaceExtFound = false;
	VkBool32 platformSurfaceExtFound = false;
	uint32_t extensionCount = 0;
	uint32_t enabledExtensions = 0;

	if (mAppState->mValidate &&
		CheckValidationLayerSupport(sValidationLayers, sNumValidationLayers) == false)
	{
		throw std::exception("Validation layers enabled but the configured layers are not supported.");
	}

	result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	assert(result == VK_SUCCESS);

	if (extensionCount > 0)
	{
		VkExtensionProperties* extensions = reinterpret_cast<VkExtensionProperties*>(malloc(sizeof(VkExtensionProperties) * extensionCount));
		
		result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);
		assert(result == VK_SUCCESS);

		for (uint32_t i = 0; i < extensionCount; i++)
		{
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, extensions[i].extensionName))
			{
				surfaceExtFound = true;
				mAppState->mEnabledExtensions[mAppState->mEnabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}

			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, extensions[i].extensionName))
			{
				platformSurfaceExtFound = 1;
				mAppState->mEnabledExtensions[mAppState->mEnabledExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}

			if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, extensions[i].extensionName))
			{
				if (mAppState->mValidate)
				{
					mAppState->mEnabledExtensions[mAppState->mEnabledExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
				}
			}
			assert(mAppState->mEnabledExtensionCount < MAX_ENABLED_EXTENSIONS);
		}

		free(extensions);
		extensions = nullptr;
	}

	if (!surfaceExtFound)
	{
		ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
	}
	if (!platformSurfaceExtFound)
	{
		ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = APP_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "None";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo ciInstance = {};
	ciInstance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	ciInstance.pApplicationInfo = &appInfo;
	ciInstance.enabledExtensionCount = mAppState->mEnabledExtensionCount;
	ciInstance.ppEnabledExtensionNames = mAppState->mEnabledExtensions;
	ciInstance.enabledLayerCount = mAppState->mEnabledLayersCount;
	ciInstance.ppEnabledLayerNames = mAppState->mEnabledLayers;
	ciInstance.pNext = nullptr;

	result = vkCreateInstance(&ciInstance, nullptr, &mInstance);

	if (result != VK_SUCCESS)
	{
		throw std::exception("Failed to create instance.");
	}
}

void Renderer::CreateDebugCallback()
{
	if (!mAppState->mValidate)
	{
		return;
	}

	VkDebugReportCallbackCreateInfoEXT ciDebugCallback;
	ciDebugCallback.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	ciDebugCallback.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	ciDebugCallback.pfnCallback = DebugCallback;

	PFN_vkCreateDebugReportCallbackEXT  pfnVkCreateDebugReportCallbackEXT = 
		(PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT");

	if (pfnVkCreateDebugReportCallbackEXT != nullptr)
	{
		pfnVkCreateDebugReportCallbackEXT(mInstance, &ciDebugCallback, nullptr, &mCallback);
	}
	else
	{
		throw exception("Failed to setup debug callback!");
	}
}

void Renderer::CreateSurface()
{
	PFN_vkCreateWin32SurfaceKHR pfnCreateWin32Surface = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(mInstance, "vkCreateWin32SurfaceKHR");

	assert(pfnCreateWin32Surface != nullptr);

	VkWin32SurfaceCreateInfoKHR ciSurface = {};
	ciSurface.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	ciSurface.hwnd = mAppState->mWindow;
	ciSurface.hinstance = GetModuleHandle(nullptr);

	if (pfnCreateWin32Surface(mInstance, &ciSurface, nullptr, &mSurface) != VK_SUCCESS)
	{
		throw exception("Failed to create window surface.");
	}
}

void Renderer::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw exception("No physical device found.");
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			mPhysicalDevice = device;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE)
	{
		throw exception("Failed to find a suitable GPU.");
	}
}

void Renderer::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);

	float priorities = 1.0f;

	const int QUEUE_GRAPHICS = 0;
	const int QUEUE_PRESENT = 1;
	int queueCount = (indices.mGraphicsFamily != indices.mPresentFamily) ? 2 : 1;

	VkDeviceQueueCreateInfo ciDeviceQueues[2];
	memset(ciDeviceQueues, 0, sizeof(VkDeviceQueueCreateInfo)*2);
	ciDeviceQueues[QUEUE_GRAPHICS].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	ciDeviceQueues[QUEUE_GRAPHICS].queueFamilyIndex = indices.mGraphicsFamily;
	ciDeviceQueues[QUEUE_GRAPHICS].queueCount = 1;
	ciDeviceQueues[QUEUE_GRAPHICS].pQueuePriorities = &priorities;

	ciDeviceQueues[QUEUE_PRESENT].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	ciDeviceQueues[QUEUE_PRESENT].queueFamilyIndex = indices.mPresentFamily;
	ciDeviceQueues[QUEUE_PRESENT].queueCount = 1;
	ciDeviceQueues[QUEUE_PRESENT].pQueuePriorities = &priorities;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo ciDevice = {};
	ciDevice.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	ciDevice.pQueueCreateInfos = ciDeviceQueues;
	ciDevice.queueCreateInfoCount = queueCount;
	ciDevice.pEnabledFeatures = &deviceFeatures;
	ciDevice.enabledExtensionCount = sNumDeviceExtensions;
	ciDevice.ppEnabledExtensionNames = sDeviceExtensions;

	if (mAppState->mValidate)
	{
		ciDevice.enabledLayerCount = sNumValidationLayers;
		ciDevice.ppEnabledLayerNames = sValidationLayers;
	}
	else
	{
		ciDevice.enabledLayerCount = 0;
	}

	VkResult result = vkCreateDevice(mPhysicalDevice, &ciDevice, nullptr, &mDevice);

	if (result != VK_SUCCESS)
	{
		throw exception("Failed to create logical device.");
	}

	vkGetDeviceQueue(mDevice, indices.mGraphicsFamily, 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, indices.mPresentFamily, 0, &mPresentQueue);
}

void Renderer::CreateImageViews()
{
	mSwapchainImageViews.resize(mSwapchainImages.size());

	for (size_t i = 0; i < mSwapchainImages.size(); ++i)
	{
		mSwapchainImageViews[i] = Texture::CreateImageView(mSwapchainImages[i], mSwapchainImageFormat);
	}
}

void Renderer::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = mSwapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo ciRenderPass = {};
	ciRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	ciRenderPass.attachmentCount = 1;
	ciRenderPass.pAttachments = &colorAttachment;
	ciRenderPass.subpassCount = 1;
	ciRenderPass.pSubpasses = &subpass;
	ciRenderPass.dependencyCount = 1;
	ciRenderPass.pDependencies = &dependency;

	if (vkCreateRenderPass(mDevice, &ciRenderPass, nullptr, &mRenderPass) != VK_SUCCESS)
	{
		throw exception("Failed to create render pass");
	}
}

void Renderer::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo ciDescriptorSetLayout = {};
	ciDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	ciDescriptorSetLayout.bindingCount = 2;
	ciDescriptorSetLayout.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(mDevice, &ciDescriptorSetLayout, nullptr, &mDescriptorSetLayout) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor set layout");
	}
}

void Renderer::CreateGraphicsPipeline()
{
	vector<char> vertShaderCode = ReadFile("Shaders/bin/shader.vert");
	vector<char> fragShaderCode = ReadFile("Shaders/bin/shader.frag");

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	vertShaderModule = CreateShaderModule(vertShaderCode);
	fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescription = Vertex::GetAttributeDescriptions();
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mSwapchainExtent.width;
	viewport.height = (float)mSwapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = mSwapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;

	if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo ciPipeline = {};
	ciPipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	ciPipeline.stageCount = 2;
	ciPipeline.pStages = shaderStages;
	ciPipeline.pVertexInputState = &vertexInputInfo;
	ciPipeline.pInputAssemblyState = &inputAssembly;
	ciPipeline.pViewportState = &viewportState;
	ciPipeline.pRasterizationState = &rasterizer;
	ciPipeline.pMultisampleState = &multisampling;
	ciPipeline.pDepthStencilState = nullptr;
	ciPipeline.pColorBlendState = &colorBlending;
	ciPipeline.pDynamicState = nullptr;
	ciPipeline.layout = mPipelineLayout;
	ciPipeline.renderPass = mRenderPass;
	ciPipeline.subpass = 0;
	ciPipeline.basePipelineHandle = VK_NULL_HANDLE;
	ciPipeline.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &ciPipeline, nullptr, &mGraphicsPipeline) != VK_SUCCESS)
	{
throw exception("Failed to create graphics pipeline");
	}

	vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);
}

void Renderer::CreateFramebuffers()
{
	mSwapchainFramebuffers.resize(mSwapchainImageViews.size());

	for (size_t i = 0; i < mSwapchainImageViews.size(); ++i)
	{
		VkImageView attachments[] = { mSwapchainImageViews[i] };

		VkFramebufferCreateInfo ciFramebuffer = {};
		ciFramebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		ciFramebuffer.renderPass = mRenderPass;
		ciFramebuffer.attachmentCount = 1;
		ciFramebuffer.pAttachments = attachments;
		ciFramebuffer.width = mSwapchainExtent.width;
		ciFramebuffer.height = mSwapchainExtent.height;
		ciFramebuffer.layers = 1;

		if (vkCreateFramebuffer(mDevice, &ciFramebuffer, nullptr, &mSwapchainFramebuffers[i]) != VK_SUCCESS)
		{
			throw exception("Failed to create framebuffer.");
		}
	}
}

void Renderer::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(mPhysicalDevice);

	VkCommandPoolCreateInfo ciCommandPool = {};
	ciCommandPool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	ciCommandPool.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily;
	ciCommandPool.flags = 0;

	if (vkCreateCommandPool(mDevice, &ciCommandPool, nullptr, &mCommandPool))
	{
		throw exception("Failed to create command pool");
	}
}

void Renderer::CreateCommandBuffers()
{
	if (mCommandBuffers.size() == 0)
	{
		mCommandBuffers.resize(mSwapchainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();

		if (vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS)
		{
			throw exception("Failed to create command buffers");
		}
	}
	else
	{
		// Command buffers cannot be in pending state when reset
		vkQueueWaitIdle(mGraphicsQueue);

		for (size_t i = 0; i < mCommandBuffers.size(); ++i)
		{
			vkResetCommandBuffer(mCommandBuffers[i], 0);
		}
	}

	for (size_t i = 0; i < mCommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = mRenderPass;
		renderPassInfo.framebuffer = mSwapchainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = mSwapchainExtent;

		VkClearValue clearColor = { 0.2f, 0.2f, 0.2f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		mGeometryPipeline.BindPipeline(mCommandBuffers[i]);

		mScene->RenderGeometry();
		mMesh.BindBuffers(mCommandBuffers[i]);

		vkCmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 0, nullptr);

		vkCmdDrawIndexed(mCommandBuffers[i], 6, 1, 0, 0, 0);

		vkCmdEndRenderPass(mCommandBuffers[i]);

		if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS)
		{
			throw exception("Failed to record command buffer");
		}
	}
}

void Renderer::CreateSemaphores()
{
	VkSemaphoreCreateInfo ciSemaphore = {};
	ciSemaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(mDevice, &ciSemaphore, nullptr, &mImageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(mDevice, &ciSemaphore, nullptr, &mRenderFinishedSemaphore) != VK_SUCCESS)
	{
		throw exception("Failed to create semaphores");
	}
}

void Renderer::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(VSUniformBuffer);
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffer, mUniformBufferMemory);
}

void Renderer::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSizes[2] = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo ciPool = {};
	ciPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	ciPool.poolSizeCount = 2;
	ciPool.pPoolSizes = poolSizes;
	ciPool.maxSets = 1;

	if (vkCreateDescriptorPool(mDevice, &ciPool, nullptr, &mDescriptorPool) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor pool");
	}
}

void Renderer::CreateTextureImage()
{
	mTexture.Load("Textures/logo.jpg");
}

VkCommandBuffer Renderer::BeginSingleSubmissionCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Renderer::EndSingleSubmissionCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mGraphicsQueue);

	vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
}

void Renderer::UpdateUniformBuffer()
{
	static auto startTime = high_resolution_clock::now();

	auto currentTime = high_resolution_clock::now();
	float time = duration_cast<milliseconds>(currentTime - startTime).count() / 1000.0f;

	VSUniformBuffer ubo = {};
	ubo.mModel = glm::rotate(glm::mat4(), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mView = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mProjection = glm::perspective(glm::radians(45.0f), ((float)mSwapchainExtent.width) / mSwapchainExtent.height, 0.1f, 100.0f);
	ubo.mProjection[1][1] *= -1.0f;

	void* data;
	vkMapMemory(mDevice, mUniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mDevice, mUniformBufferMemory);
}

void Renderer::CreateBuffer(VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo ciBuffer = {};
	ciBuffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	ciBuffer.size = size;
	ciBuffer.usage = usage;
	ciBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ciBuffer.flags = 0;

	if (vkCreateBuffer(mDevice, &ciBuffer, nullptr, &buffer) != VK_SUCCESS)
	{
		throw exception("Failed to create vertex buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw exception("Failed to allocate memory for vertex buffer");
	}

	vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
}

void Renderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleSubmissionCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleSubmissionCommands(commandBuffer);
}

void Renderer::RecreateSwapchain()
{
	if (!mInitialized)
	{
		return;
	}

	vkDeviceWaitIdle(mDevice);

	DestroySwapchain();

	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandBuffers();
}

VkDescriptorPool Renderer::GetDescriptorPool()
{
	return GetDescriptorPool();
}

Pipeline& Renderer::GetGeometryPipeline()
{
	return mGeometryPipeline;
}

Pipeline& Renderer::GetLightPipeline()
{
	return mLightPipeline;
}

Pipeline& Renderer::GetDeferredPipeline()
{
	return mDeferredPipeline;
}

uint32_t Renderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw exception("Failed to find suitable memory type");
}

VkShaderModule Renderer::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo ciModule = {};
	ciModule.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ciModule.codeSize = code.size();

	vector<uint32_t> codeLong(code.size() / sizeof(uint32_t) + 1);
	memcpy(codeLong.data(), code.data(), code.size());
	ciModule.pCode = codeLong.data();

	VkShaderModule module;

	if (vkCreateShaderModule(mDevice, &ciModule, nullptr, &module) != VK_SUCCESS)
	{
		throw exception("Failed to create shader module");
	}

	return module;
}

bool Renderer::IsDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	bool extensionsSupported = CheckDeviceExtensionSupport(device, sDeviceExtensions, sNumDeviceExtensions);

	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 0)
	{
		throw exception("No available formats for swap surface.");
	}

	if (availableFormats.size() == 1 &&
		availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
	vector<VkPresentModeKHR> preferredModes = { VK_PRESENT_MODE_FIFO_KHR,
											   VK_PRESENT_MODE_IMMEDIATE_KHR,
											   VK_PRESENT_MODE_FIFO_RELAXED_KHR,
											   VK_PRESENT_MODE_MAILBOX_KHR };

	for (VkPresentModeKHR mode : preferredModes)
	{
		if (find(availableModes.begin(), availableModes.end(), mode) != availableModes.end())
		{
			return mode;
		}
	}

	throw exception("Could not find a valid present mode for swapchain.");
}

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// Not entirely sure what this if statement is doing yet.
	if (capabilities.currentExtent.width != numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	VkExtent2D actualExtent = { mAppState->mWindowWidth, mAppState->mWindowHeight };

	if (actualExtent.width < capabilities.minImageExtent.width)
		actualExtent.width = capabilities.minImageExtent.width;
	if (actualExtent.width > capabilities.maxImageExtent.width)
		actualExtent.width = capabilities.maxImageExtent.width;

	if (actualExtent.height < capabilities.minImageExtent.height)
		actualExtent.height = capabilities.minImageExtent.height;
	if (actualExtent.height > capabilities.maxImageExtent.height)
		actualExtent.height = capabilities.maxImageExtent.height;

	return actualExtent;
}

QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int32_t i = 0;
	
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 &&
			queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.mGraphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

		if (queueFamily.queueCount > 0 &&
			presentSupport)
		{
			indices.mPresentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool Renderer::CheckValidationLayerSupport(const char** layers, uint32_t count)
{
	uint32_t numLayers = 0;
	vkEnumerateInstanceLayerProperties(&numLayers, nullptr);

	VkLayerProperties* layerProperties = (VkLayerProperties*) malloc(numLayers * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&numLayers, layerProperties);

	bool supported = true;
	
	for (uint32_t j = 0; j < count; j++)
	{
		bool layerFound = false;

		for (uint32_t i = 0; i < numLayers; i++)
		{
			if (strcmp(layers[j], layerProperties[i].layerName) == 0)
			{
				layerFound = true;
				mAppState->mEnabledLayers[mAppState->mEnabledLayersCount++] = layers[j];
				break;
			}
		}

		if (!layerFound)
		{
			supported = false;
			break;
		}
	}

	free(layerProperties);
	return supported;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device,
										   const char** extensions,
										   uint32_t count)
{
	uint32_t availableExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, nullptr);

	vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, availableExtensions.data());

	set<string> requiredExtensions;

	for (uint32_t i = 0; i < sNumDeviceExtensions; ++i)
	{
		requiredExtensions.insert(sDeviceExtensions[i]);
	}

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void Renderer::DestroyDebugCallback()
{
	PFN_vkDestroyDebugReportCallbackEXT pfnVkDestroyDebugReportCallbackEXT =
		(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT");

	if (pfnVkDestroyDebugReportCallbackEXT != nullptr)
	{
		pfnVkDestroyDebugReportCallbackEXT(mInstance, mCallback, nullptr);
	}
}
