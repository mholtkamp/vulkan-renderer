#pragma once

#include <vulkan/vulkan.h>
//#include <glm/glm.hpp>

#include "Actor.h"
#include "Allocator.h"
#include "Pipeline.h"
#include "PipelineConfigs.h"

struct WaveParticleData
{
	glm::vec4 mPosition;
	glm::vec4 mVelocity;
	glm::vec4 mRadiusFalloff;
	//float mRadius;
	//float mFalloff;

	WaveParticleData() :
		mPosition(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)),
		mVelocity(glm::vec4(0.05f, 0.05f, 0.05f, 0.0f)),
		mRadiusFalloff(0.05f, 1.0f, 0.f, 0.0f)
		//mRadius(0.05f),
		//mFalloff(1.0f)
	{

	}
};

struct WaterSurfaceData
{
	glm::vec4 mPosition;
	glm::vec4 mDeepColor;
	glm::vec4 mShallowColor;
	glm::vec4 mWaveSpeedHeight;
	//float mWaveSpeed;
	//float mWaveHeight;

	WaterSurfaceData() :
		mPosition(glm::vec4(0.0f, 0.0f, 0.0f, 0.0)),
		mDeepColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)),
		mShallowColor(glm::vec4(0.0f, 0.3f, 0.9f, 0.5f)),
		mWaveSpeedHeight(glm::vec4(0.05f, 1.0f, 0.0f, 0.0f))
		//mWaveSpeed(0.05f),
		//mWaveHeight(1.0f)
	{

	}
};

class WaterSurface : public Actor
{
public:

	WaterSurface();
	~WaterSurface();

	void CreateWaterSurface();
	virtual void Destroy() override;
	void Simulate(VkCommandBuffer cb);
	//void Render(VkCommandBuffer cb);
	virtual void Draw(VkCommandBuffer cb) override;

	virtual void Update(class Scene* scene, float deltaTime) override;
	//void UpdateDescriptorSets();

	static void LoadWaterPlaneMesh();
	static void DestroyWaterPlaneMesh();

protected:

	static class Mesh* sWaterPlaneMesh;
	
	void CreateParticleBuffer();
	void CreateWaterSurfaceBuffer();
	void CreateDisplacementImage();
	void CreateWaterDescriptorSets();

	WaterSurfaceData mWaterSurfaceData;

	WaterSimulationPipeline mWaterSimulationPipeline;
	WaterRenderingPipeline mWaterRenderingPipeline;

	VkDescriptorSet mSimulationDescriptorSet;
	VkDescriptorSet mRenderDescriptorSet;

	VkBuffer mParticleBuffer;
	Allocation mParticleBufferMemory;

	VkBuffer mWaterSurfaceBuffer;
	Allocation mWaterSurfaceBufferMemory;

	VkImage mDisplacementImage;
	Allocation mDisplacementImageMemory;
	VkImageView mDisplacementImageView;
	VkSampler mDisplacementSampler;
};