#include "WaterSurface.h"
#include "Renderer.h"
#include "Mesh.h"
#include "VkrMath.h"

#include <exception>

#define NUM_WAVE_PARTICLES 512
#define DISPLACEMENT_IMAGE_SIZE 1024

Mesh* WaterSurface::sWaterPlaneMesh = nullptr;

WaterSurface::WaterSurface()
{

}

WaterSurface::~WaterSurface()
{

}

void WaterSurface::CreateWaterSurface()
{
	mWaterSimulationPipeline.Create();
	mWaterRenderingPipeline.Create();

	// Actor setup
	mName = "WaterSurface";
	mMesh = sWaterPlaneMesh;
	//mWorldMatrix = glm::mat4(); // Identity matrix
	CreateUniformBuffer();
	CreateDescriptorSet();

	// Water setup
	CreateParticleBuffer();
	CreateWaterSurfaceBuffer();
	CreateDisplacementImage();
	CreateWaterDescriptorSets();
}

void WaterSurface::Destroy()
{
	Actor::Destroy();

	VkDevice device = Renderer::Get()->GetDevice();

	mWaterSimulationPipeline.Destroy();
	mWaterRenderingPipeline.Destroy();

	if (mParticleBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, mParticleBuffer, nullptr);
		Allocator::Free(mParticleBufferMemory);
		vkDestroyImageView(device, mDisplacementImageView, nullptr);
		vkDestroyImage(device, mDisplacementImage, nullptr);
		Allocator::Free(mDisplacementImageMemory);

		mParticleBuffer = VK_NULL_HANDLE;
		mDisplacementImage = VK_NULL_HANDLE;
		mDisplacementImageView = VK_NULL_HANDLE;
	}
}


void WaterSurface::Update(class Scene* scene, float deltaTime)
{
	Actor::Update(scene, deltaTime);
	
	VkDevice device = Renderer::Get()->GetDevice();

	// Move particles
	void* data = nullptr;
	vkMapMemory(device,
		mParticleBufferMemory.mDeviceMemory,
		mParticleBufferMemory.mOffset,
		mParticleBufferMemory.mSize,
		0,
		&data);

	for (int32_t i = 0; i < NUM_WAVE_PARTICLES; ++i)
	{
		WaveParticleData* particle = reinterpret_cast<WaveParticleData*>(data) + i;

		particle->mPosition += particle->mVelocity * deltaTime;
		
		particle->mPosition.x = glm::mod(particle->mPosition.x, 1.0f);
		particle->mPosition.y = glm::mod(particle->mPosition.y, 1.0f);

		//if (particle->mPosition.x < 0.0f)
		//	particle->mPosition.x += 1.0f;
		//if (particle->mPosition.x > 1.0f)
		//	particle->mPosition.x -= 1.0f;
		//if (particle->mPosition.y < 0.0f)
		//	particle->mPosition.y += 1.0f;
		//if (particle->mPosition.y > 1.0f)
		//	particle->mPosition.y -= 1.0f;

		//particle->mRadius = 0.05f;
		//particle->mFalloff = 1.0f;
	}

	vkUnmapMemory(device, mParticleBufferMemory.mDeviceMemory);
}

void WaterSurface::Draw(VkCommandBuffer cb)
{
	Renderer* renderer = Renderer::Get();
	//Pipeline& geometryPipeline = renderer->GetGeometryPipeline();

	if (mMesh != nullptr)
	{
		mWaterRenderingPipeline.BindPipeline(cb);

		mMesh->BindBuffers(cb);

		vkCmdBindDescriptorSets(cb,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mWaterRenderingPipeline.GetPipelineLayout(),
			0,
			1,
			&renderer->GetGlobalDescriptorSet(),
			0,
			0);


		vkCmdBindDescriptorSets(cb,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mWaterRenderingPipeline.GetPipelineLayout(),
			1,
			1,
			&mDescriptorSet,
			0,
			nullptr);

		// Prepare water surface graphics pipeline
		vkCmdBindDescriptorSets(cb,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mWaterRenderingPipeline.GetPipelineLayout(),
			2, // Set #1 (0 is global, 1 is geometry)
			1, // Only 1 descriptor set (with 2 bindings)
			&mRenderDescriptorSet,
			0,
			0);

		vkCmdDrawIndexed(cb,
			mMesh->GetNumIndices(),
			1,
			0,
			0,
			0);
	}
}

void WaterSurface::Simulate(VkCommandBuffer cb)
{
	Texture::TransitionImageLayout(mDisplacementImage,
		VK_FORMAT_R16_SFLOAT,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		1,
		1,
		cb);

	mWaterSimulationPipeline.BindPipeline(cb);

	vkCmdBindDescriptorSets(cb,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		mWaterSimulationPipeline.GetPipelineLayout(),
		1, // Set #1 (0 is global)
		1, // Only 1 descriptor set (with 2 bindings)
		&mSimulationDescriptorSet,
		0,
		0);

	vkCmdDispatch(cb, DISPLACEMENT_IMAGE_SIZE / 16, DISPLACEMENT_IMAGE_SIZE / 16, 1);

	// Insert a barrier before the water rendering reads from the texture.
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier.image = mDisplacementImage;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(cb,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,  // srcStageMask
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // dstStageMask
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,                                     // imageMemoryBarrierCount
		&imageMemoryBarrier);                   // pImageMemoryBarriers
}

void WaterSurface::LoadWaterPlaneMesh()
{
	DestroyWaterPlaneMesh();

	sWaterPlaneMesh = new Mesh();
	sWaterPlaneMesh->LoadMesh("Engine/Meshes/WaterPlaneMesh.dae");
	sWaterPlaneMesh->SetMaterial(Renderer::Get()->GetDefaultMaterial());
}

void WaterSurface::DestroyWaterPlaneMesh()
{
	if (sWaterPlaneMesh != nullptr)
	{
		sWaterPlaneMesh->Destroy();
		delete sWaterPlaneMesh;
		sWaterPlaneMesh = nullptr;
	}
}

void WaterSurface::CreateParticleBuffer()
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkDeviceSize bufferSize = NUM_WAVE_PARTICLES * sizeof(WaveParticleData);
	Renderer::Get()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mParticleBuffer, mParticleBufferMemory);

	// Initialize the particle locations
	void* data = nullptr;
	vkMapMemory(device,
		mParticleBufferMemory.mDeviceMemory,
		mParticleBufferMemory.mOffset,
		mParticleBufferMemory.mSize,
		0,
		&data);

	for (int32_t i = 0; i < NUM_WAVE_PARTICLES; ++i)
	{
		WaveParticleData* particle = reinterpret_cast<WaveParticleData*>(data) + i;

		particle->mPosition.x = VkrMath::RandRange(0.0f, 1.0f);
		particle->mPosition.y = VkrMath::RandRange(0.0f, 1.0f);
		particle->mPosition.z = 0.0f;
		particle->mPosition.w = 0.0f;

		const float MaxSpeed = 0.05f;
		particle->mVelocity.x = MaxSpeed * VkrMath::RandRange(-1.0f, 1.0f);
		particle->mVelocity.y = MaxSpeed * VkrMath::RandRange(-1.0f, 1.0f);
		particle->mVelocity.z = 0.0f;
		particle->mVelocity.w = 0.0f;

		particle->mRadiusFalloff = glm::vec4(0.05f, 1.0f, 0.0f, 0.0f);
		particle->mRadiusFalloff.x = VkrMath::RandRange(0.03f, 0.07f);
		//particle->mRadius = 0.05f;
		//particle->mFalloff = 1.0f;
	}

	vkUnmapMemory(device, mParticleBufferMemory.mDeviceMemory);
}

void WaterSurface::CreateWaterSurfaceBuffer()
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkDeviceSize bufferSize = sizeof(WaterSurfaceData);
	Renderer::Get()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mWaterSurfaceBuffer, mWaterSurfaceBufferMemory);

	// Initialize the particle locations
	void* data = nullptr;
	vkMapMemory(device,
		mWaterSurfaceBufferMemory.mDeviceMemory,
		mWaterSurfaceBufferMemory.mOffset,
		mWaterSurfaceBufferMemory.mSize,
		0,
		&data);

	memcpy(data, &mWaterSurfaceData, sizeof(WaterSurfaceData));

	vkUnmapMemory(device, mWaterSurfaceBufferMemory.mDeviceMemory);
}

void WaterSurface::CreateDisplacementImage()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkFormat format = VK_FORMAT_R16_SFLOAT;
	VkImageUsageFlags usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	Texture::CreateImage(DISPLACEMENT_IMAGE_SIZE,
		DISPLACEMENT_IMAGE_SIZE,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mDisplacementImage,
		mDisplacementImageMemory);

	mDisplacementImageView = Texture::CreateImageView(mDisplacementImage,
		format,
		aspect);

	Texture::TransitionImageLayout(mDisplacementImage,
		format,
		VK_IMAGE_LAYOUT_UNDEFINED,
		layout);

	VkSamplerCreateInfo ciSampler = {};
	ciSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	ciSampler.magFilter = VK_FILTER_LINEAR;
	ciSampler.minFilter = VK_FILTER_LINEAR;
	ciSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	ciSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	ciSampler.addressModeV = ciSampler.addressModeU;
	ciSampler.addressModeW = ciSampler.addressModeU;
	ciSampler.mipLodBias = 0.0f;
	ciSampler.maxAnisotropy = 1.0f;
	ciSampler.minLod = 0.0f;
	ciSampler.maxLod = 1.0f;
	ciSampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	if (vkCreateSampler(device, &ciSampler, nullptr, &mDisplacementSampler) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture sampler");
	}
}

void WaterSurface::CreateWaterDescriptorSets()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	// Create Simulation descriptors
	{
		VkDescriptorSetLayout layouts[] = { mWaterSimulationPipeline.GetDescriptorSetLayout(1) };
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = renderer->GetDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;

		if (vkAllocateDescriptorSets(device, &allocInfo, &mSimulationDescriptorSet) != VK_SUCCESS)
		{
			throw std::exception("Failed to create descriptor set");
		}

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mParticleBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = NUM_WAVE_PARTICLES * sizeof(WaveParticleData);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = mDisplacementImageView;
		imageInfo.sampler = mDisplacementSampler;

		VkWriteDescriptorSet descriptorWrites[2] = {};

		// Update Simulation descriptor binding 0 : Wave Particle Buffer
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = mSimulationDescriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		// Update Simulation descriptor binding 1 : Displacement Map Image
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = mSimulationDescriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
	}

	// Create and Update the Rendering Descriptors
	{
		VkDescriptorSetLayout layouts[] = { mWaterRenderingPipeline.GetDescriptorSetLayout(2) };
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = renderer->GetDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;

		if (vkAllocateDescriptorSets(device, &allocInfo, &mRenderDescriptorSet) != VK_SUCCESS)
		{
			throw std::exception("Failed to create descriptor set");
		}

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mWaterSurfaceBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(WaterSurfaceData);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = mDisplacementImageView;
		imageInfo.sampler = mDisplacementSampler;

		VkWriteDescriptorSet descriptorWrites[2] = {};

		// Update render descriptor binding 0 : wave data 
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = mRenderDescriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		// Update render descriptor binding 1 : Displacement Map Image
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = mRenderDescriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
	}
}