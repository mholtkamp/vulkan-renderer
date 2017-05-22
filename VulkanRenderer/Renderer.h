#pragma once

#include <vulkan/vulkan.h>
#include "ApplicationState.h"

class Renderer
{
public:
	static void Create();
	static void Destroy();
	static Renderer* Get();

	~Renderer();

	void Initialize();

	void CreateSwapchain();

	void PreparePresentation();

	void Render();

	void SetAppState(AppState* appState);

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

	bool CheckValidationLayerSupport(const char** layers,
									 uint32_t count);

	void DestroyDebugCallback();

	static Renderer* sInstance;

	Renderer();

	VkInstance mInstance;
	VkDebugReportCallbackEXT mCallback;

	AppState* mAppState;
};