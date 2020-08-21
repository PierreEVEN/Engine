#include "UI/SubWindows/DebugUI.h"
#include "Scene/Scene.h"
#include "Scene/SceneComponents/Camera.h"
#include "Scene/SceneComponents/StaticMeshComponent.h"
#include "Rendering.h"
#include "Assets/Texture2D.h"

SRotatorf objRot(90,0,0);


Rendering::DebugUI* currentDebugUI = nullptr;

double LastPosX = 0, LastPosY = 0;

bool bCapture = false;

void glfwCursorPosCallback(GLFWwindow* window, double posX, double posY)
{

	if (!bCapture) return;

	if (!currentDebugUI) return;


	currentDebugUI->cameraRotation.z += (float)((posX - LastPosX) / 5);
	currentDebugUI->cameraRotation.y += (float)((posY - LastPosY) / -5);


	LastPosX = posX;
	LastPosY = posY;
}

bool bFor = false;
bool bBakc = false;
bool bRight = false;
bool bLeft = false;
bool bUp = false;
bool bDown = false;


void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!currentDebugUI) return;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		bCapture = !bCapture;
		glfwSetInputMode(window, GLFW_CURSOR, bCapture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	if (!bCapture) return;

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			bFor = true;
		}
		if (key == GLFW_KEY_S) {
			bBakc = true;
		}
		if (key == GLFW_KEY_A) {
			bLeft = true;
		}
		if (key == GLFW_KEY_D) {
			bRight = true;
		}
		if (key == GLFW_KEY_SPACE) {
			bUp = true;
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			bDown = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) {
			bFor = false;
		}
		if (key == GLFW_KEY_S) {
			bBakc = false;
		}
		if (key == GLFW_KEY_A) {
			bLeft = false;
		}
		if (key == GLFW_KEY_D) {
			bRight = false;
		}
		if (key == GLFW_KEY_SPACE) {
			bUp = false;
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			bDown = false;
		}
	}
}

void processMoves() {

	if (!bCapture) return;

	float speed = 20 * (float)currentDebugUI->parent->GetDeltaTime();
	if (bFor) {
		currentDebugUI->parent->GetCamera()->AddLocalOffset(SVector(speed, 0, 0));
	}
	if (bBakc) {
		currentDebugUI->parent->GetCamera()->AddLocalOffset(SVector(-speed, 0, 0));
	}
	if (bLeft) {
		currentDebugUI->parent->GetCamera()->AddLocalOffset(SVector(0, speed, 0));
	}
	if (bRight) {
		currentDebugUI->parent->GetCamera()->AddLocalOffset(SVector(0, -speed, 0));
	}
	if (bUp) {
		currentDebugUI->parent->GetCamera()->AddLocalOffset(SVector(0, 0, -speed));
	}
	if (bDown) {
		currentDebugUI->parent->GetCamera()->AddLocalOffset(SVector(0, 0, speed));
	}
}
bool bSetScale = false;

Rendering::DebugUI::DebugUI(ViewportInstance* parentScene)
	: SubWindow("Debug UI") {
	parent = parentScene;

	currentDebugUI = this;

	glfwSetCursorPosCallback(GetPrimaryWindow(), glfwCursorPosCallback);
	glfwSetKeyCallback(GetPrimaryWindow(), glfwKeyCallback);

}
void Rendering::DebugUI::DrawContent(const size_t& imageIndex)
{
	processMoves();
	float avgFps = 0;

	fpsHistory[fpsFrameIndex] = 1 / (float)parent->GetDeltaTime();

	for (int i = 199; i >= 0; --i)
	{
		avgFps += fpsHistory[(fpsFrameIndex - i + 1000) % 1000];
	}
	avgFps /= 200;
	ImGui::Text(String(String("Framerate : ") + ToString(1 / parent->GetDeltaTime()) + " fps").GetData());
	if (G_ENABLE_MULTISAMPLING.GetValue())
	{
		ImGui::Text("MSAA samples : %d", G_MSAA_SAMPLE_COUNT);
	}
	else
	{
		ImGui::Text("MSAA samples : disabled");
	}
	if ((1 / parent->GetDeltaTime() > maxFpsHistory)) maxFpsHistory = 1 / (float(parent->GetDeltaTime()));
	TConVarUI<int32_t>::DrawUI(G_MAX_FRAMERATE);
	fpsFrameIndex = (fpsFrameIndex + 1) % 1000;
	ImGui::PlotLines("framerate", fpsHistory, IM_ARRAYSIZE(fpsHistory), fpsFrameIndex, String(String("average : ") + ToString(avgFps) + String(" / max : ") + ToString(maxFpsHistory)).GetData(), 0, maxFpsHistory, ImVec2(0, 80.0f));

	TConVarUI<bool>::DrawUI(G_ENABLE_MULTISAMPLING);
	TConVarUI<bool>::DrawUI(G_SLEEP_HIDLE_THREADS);
	TConVarUI<bool>::DrawUI(G_FULSCREEN_MODE);
	TConVarUI<bool>::DrawUI(G_ENABLE_VALIDATION_LAYERS);
	ImGui::Separator();

	if (G_TEST_COMP) {

		if (!bSetScale) {
			bSetScale = true;
			G_TEST_COMP->SetScale(0.1f);
		}
		ImGui::Separator();
		SVector testPos = G_TEST_COMP->GetLocation();
		SVector testScale = G_TEST_COMP->GetScale();

		ImGui::SliderFloat3("obj pos", testPos.coords, -20, 20, "%.3f", 4);
		ImGui::SliderFloat3("obj rot", objRot.coords, -180, 180);
		ImGui::SliderFloat("objScale", &testScale.x, 0.01f, 100, "%.3f", 4);
		testScale.y = testScale.x;
		testScale.z = testScale.x;

		G_TEST_COMP->SetTransform(STransform(testPos, objRot, testScale));
		ImGui::Separator();

	}
	ImGui::SliderFloat("fov", &fov, 1, 179);
	ImGui::SliderFloat("clip min", &mi, 0, 1);
	ImGui::SliderFloat("clip max", &mx, 0, 1000);
	ImGui::Separator();
	ImGui::Text("Default texture");
	ImGui::Image(G_DEFAULT_TEXTURE->GetTextureID(imageIndex), ImVec2(512, 512));


	if (ImGui::Button("log")) LOG("log");
	if (ImGui::Button("warning")) LOG_WARNING("warning");
	if (ImGui::Button("error")) LOG_ERROR("error");

// 	parent->GetCamera()->SetLocation(cameraPosition);
 	parent->GetCamera()->SetRotation(cameraRotation);
	parent->GetCamera()->fieldOfView = fov;
	parent->GetCamera()->nearClipPlane = mi;
	parent->GetCamera()->farClipPlane = mx;
}