#pragma once

#include "SubWindow.h"
#include "Ressources/MaterialRessource.h"

namespace Rendering {

	class ShaderImportWindow : public SubWindow {
	public:


		ShaderImportWindow(const EShaderType& desiredShaderType);

	protected:

		virtual void DrawContent(const size_t& imageIndex) {}

	private:

		const EShaderType shaderType;

		void OnPickedShader(const String& path);
	};

	class MaterialConstructorWindow : public SubWindow {
	public:


		MaterialConstructorWindow();

	protected:

		virtual void DrawContent(const size_t& imageIndex);

	private:

		bool bIsTranslucent = false;
		bool bWireframe = false;
		bool bDoubleSided = false;
		bool bDisableDepthTest = false;
		bool bEnableStencilTest = false;

		SMaterialStaticProperties materialProperties;

		void CreateMaterial();


		void OnPickedVShader(VertexShaderModule* module);
		void OnPickedFShader(FragmentShaderModule* module);
	};
}