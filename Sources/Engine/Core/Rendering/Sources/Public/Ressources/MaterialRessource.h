#pragma once 

#include "GraphicRessource.h"
#include "Assets/ShaderModule.h"


namespace Rendering
{
	struct TextureRessource;
	struct MaterialRessource;
	class ViewportInstance;
	class Texture2D;

	typedef int EMaterialCreationFlags;
	enum EMaterialCreationFlags_
	{
		EMATERIAL_CREATION_FLAG_NONE = 0,
		EMATERIAL_CREATION_FLAG_TRANSLUCENT = 1 << 0,
		EMATERIAL_CREATION_FLAG_WIREFRAME = 1 << 1,
		EMATERIAL_CREATION_FLAG_DOUBLESIDED = 1 << 2,
		EMATERIAL_CREATION_FLAG_DISABLE_DEPTH_TEST = 1 << 3,
		EMATERIAL_CREATION_FLAG_ENABLE_STENCIL_TEST = 1 << 4,
	};

	struct SMaterialStaticProperties {
		bool bUseGlobalUbo = true;
		uint32_t VertexTexture2DCount = 0;
		uint32_t FragmentTexture2DCount = 0;
		EMaterialCreationFlags materialCreationFlag = EMATERIAL_CREATION_FLAG_NONE;
		class VertexShaderModule* vertexShaderModule = nullptr;
		class FragmentShaderModule* fragmentShaderModule = nullptr;


		inline bool operator==(const SMaterialStaticProperties& other) const {
			return
				bUseGlobalUbo == other.bUseGlobalUbo &&
				VertexTexture2DCount == other.VertexTexture2DCount &&
				materialCreationFlag == other.materialCreationFlag &&
				FragmentTexture2DCount == other.FragmentTexture2DCount &&
				*vertexShaderModule == *other.vertexShaderModule &&
				*fragmentShaderModule == *other.fragmentShaderModule;
		}
	};

	struct SMaterialDynamicProperties {
		std::vector<Texture2D*> vertexTextures2D = {};
		std::vector<Texture2D*> fragmentTextures2D = {};
	};

}

MAKE_HASHABLE(Rendering::SMaterialStaticProperties, t.bUseGlobalUbo, t.FragmentTexture2DCount, t.VertexTexture2DCount, t.materialCreationFlag, t.vertexShaderModule, t.fragmentShaderModule);

namespace Rendering {

	struct MaterialRessource : public Ressource
	{
		void PreDraw();
		void Draw(VkCommandBuffer commandBuffer, const size_t& imageIndex);
		inline VkDescriptorSet& GetDescriptorSet(const size_t& imageIndex) { return descriptorSets[imageIndex]; }
		inline VkPipelineLayout& GetPipelineLayout() { return pipelineLayout; }

		// @TODO handle material instanciation
		inline static MaterialRessource* FindMaterialRessource(const SMaterialStaticProperties materialProperties) {
			if (materialRessources.count(materialProperties) == 0) {
				MaterialRessource* newMat = new MaterialRessource(materialProperties);
				materialRessources[materialProperties] = newMat;
				return newMat;
			}
			return new MaterialRessource(materialProperties);// materialRessources[materialProperties];
		}


	protected:

		virtual void CreateOrUpdateRessource();
		virtual void DestroyRessources();

		virtual ~MaterialRessource();

	private:

		MaterialRessource(const SMaterialStaticProperties& materialProperties);

		static std::vector<VkDescriptorSetLayoutBinding> MakeLayoutBindings(const SMaterialStaticProperties& materialProperties);
		void CreatePipeline(const SMaterialStaticProperties& materialProperties);
		void CreateDescriptorSets(std::vector<VkDescriptorSetLayoutBinding> layoutBindings);

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> descriptorSets;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline shaderPipeline = VK_NULL_HANDLE;
		size_t lastDrawIamgeIndex = -1;

		inline static std::unordered_map<SMaterialStaticProperties, MaterialRessource*> materialRessources;

		SMaterialStaticProperties materialStaticProperties;
		bool bShouldRecreatePipeline = false;
		inline void RequestPipelineUpdate() { bShouldRecreatePipeline = true; }
	};





	class ViewportInstance;

	class MaterialRessourceItem : public Ressource {
	public:

		MaterialRessourceItem(const SMaterialStaticProperties& materialProperties, const SMaterialDynamicProperties& materialDynProps)
			: parent(MaterialRessource::FindMaterialRessource(materialProperties)), dynamicMaterialProperties(materialDynProps), staticMaterialProperties(materialProperties), Ressource() {}

		void PreDraw(ViewportInstance* inViewport, const size_t& imageIndex);
		void Draw(VkCommandBuffer commandBuffer, ViewportInstance* drawViewport, const size_t& imageIndex);

		inline VkDescriptorSet& GetDescriptorSet(const size_t& imageIndex) { return parent->GetDescriptorSet(imageIndex); }
		inline VkPipelineLayout& GetPipelineLayout() { return parent->GetPipelineLayout(); }


	protected:

		virtual void CreateOrUpdateRessource() {}
		virtual void DestroyRessources() {}

		virtual ~MaterialRessourceItem() {}

	private:

		void UpdateDescriptorSets(ViewportInstance* drawViewport, size_t imageIndex);

		const SMaterialDynamicProperties dynamicMaterialProperties;
		const SMaterialStaticProperties staticMaterialProperties;

		size_t lastDrawIamgeIndex = -1;
		MaterialRessource* parent;
	};
}