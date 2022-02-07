#pragma once

#include <memory>
#include <vector>

#include "scene.h"
#include "window.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace sge
{
	class RenderState;

	class ImGuiRenderer
	{
	public:
		void initialize(std::weak_ptr<Window>, RenderState* state, std::weak_ptr<Scene> scene);
		void update(float delta_time);
		void shutdown();

	private:
		void drawSettings(float delta_time);

		void drawPointLights();
		void drawDirectionalLight();

		void drawModel(Object& obj, int index);
		void drawPosition(const std::string& title, const std::string& name, glm::vec3& position);
		void drawPosition(std::string& name, Vector3& position);
		void drawColor(std::string& name, ColorData& color);
		void drawFPS(float delta_time);
		void drawCountVertices();
		void drawPolygonModeSettings();
		void drawNormalMapsSettings();
		void drawScreenSettings();
		void drawObjects();
		void setPointLightDistance(PointLightData& point_light);

	private:
		RenderState* state{ nullptr };
		std::weak_ptr<Scene> scene;
		std::weak_ptr<Window> window;
		std::vector<std::string> objects;
		int selected_object;
	};
}