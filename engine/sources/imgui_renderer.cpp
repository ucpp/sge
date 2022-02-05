#include "imgui_renderer.h"

#include <string>
#include <sstream>

#include "resource_manager.h"
#include "editor.h"

namespace sge
{
	void ImGuiRenderer::initialize(std::weak_ptr<Window> window, RenderState* state, std::weak_ptr<Scene> scene)
	{
		this->state = state;
		this->scene = scene;
		this->window = window;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto s_ptr_window = window.lock();
		ImGui_ImplGlfw_InitForOpenGL(s_ptr_window->get(), true);

#ifdef __APPLE__
		ImGui_ImplOpenGL3_Init("#version 150");
#else
		ImGui_ImplOpenGL3_Init("#version 130");
#endif

		ImGui::StyleColorsDark();
	}

	void ImGuiRenderer::update(float delta_time)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), 1);
		ImGui::SetNextWindowSize(ImVec2(360, 400));
		ImGui::Begin("SGE v0.2.0", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);

		drawSettings(delta_time);

		ImGui::Separator();
		ImGui::BeginGroup();
		drawObjects();
		ImGui::EndGroup();

		ImGui::End();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiRenderer::drawSettings(float delta_time)
	{
		if (!ImGui::CollapsingHeader("Settings"))
		{
			return;
		}
		drawFPS(delta_time);
		ImGui::SameLine();
		drawCountVertices();
		ImGui::Separator();
		drawScreenSettings();
		drawPolygonModeSettings();
		drawNormalMapsSettings();
	}

	void ImGuiRenderer::drawObjects()
	{
		if (!ImGui::CollapsingHeader("Scene"))
		{
			return;
		}
		auto sptr_scene = scene.lock();
		if (sptr_scene != nullptr)
		{
			drawLight();
			ImGui::Separator();
			ImGui::Checkbox("skybox", &sptr_scene->skybox_enabled);
			ImGui::Separator();
			if (ImGui::TreeNode("models"))
			{
				int index = 0;
				for (auto& obj : sptr_scene->objects)
				{
					index++;
					drawModel(obj, index);
				}
				ImGui::TreePop();
			}
		}
	}

	void ImGuiRenderer::drawLight()
	{
		if (ImGui::TreeNode("lights"))
		{
			drawDirectionalLight();
			ImGui::Separator();
			drawPointLights();
			ImGui::TreePop();
		}
	}

	void ImGuiRenderer::drawPointLights()
	{
		auto sptr_scene = scene.lock();
		auto& lights = sptr_scene->getPointLights();
		if (ImGui::TreeNode("points"))
		{
			for (int i = 0; i < lights.size(); ++i)
			{
				std::string name = "point[" + std::to_string(i) + "]";
				ImGui::Checkbox(name.c_str(), &lights[i].data.enabled);
				ImGui::SameLine();
				drawPosition(name, lights[i].data.position);
				ImGui::SameLine();
				drawColor(name, lights[i].data.color);
				ImGui::Text("Distance:");
				ImGui::SameLine();
				if (ImGui::DragInt(("##dist" + std::to_string(i)).c_str(), &lights[i].data.distance, 1.0f, 0, 3250))
				{
					setPointLightDistance(lights[i].data);
				}
			}
			ImGui::TreePop();
		}
	}

	void ImGuiRenderer::drawDirectionalLight()
	{
		ImGui::Indent();
		auto sptr_scene = scene.lock();
		std::string name = "DirLight";
		ImGui::Checkbox(name.c_str(), &sptr_scene->getDirectionalLight()->data.enabled);
		ImGui::SameLine();
		drawPosition(name, sptr_scene->getDirectionalLight()->data.position);
		ImGui::Unindent();
	}

	void ImGuiRenderer::drawColor(std::string& name, ColorData& color)
	{
		name = "##" + name;
		ImVec4 col = { (float)color.r, (float)color.g, (float)color.b, 1.0f };
		bool open_popup = ImGui::ColorButton(name.c_str(), col);
		if (open_popup)
		{
			ImGui::OpenPopup((name + "popup_picker").c_str());
		}
		if (ImGui::BeginPopup((name + "popup_picker").c_str()))
		{
			float c[3] = { col.x, col.y, col.z };
			if (ImGui::ColorPicker3((name + "picker").c_str(), c))
			{
				color.r = c[0];
				color.g = c[1];
				color.b = c[2];
			}
			ImGui::EndPopup();
		}
	}

	void ImGuiRenderer::drawPosition(const std::string& title, const std::string& name, glm::vec3& position)
	{
		std::string new_name = "##" + name + title;
		ImGui::Text(title.c_str());
		ImGui::SameLine();
		float v3[3]{ position.x, position.y, position.z };
		ImGui::SetNextItemWidth(160.0f);
		if (ImGui::DragFloat3(new_name.c_str(), v3, 0.05f, 0.0f, 0.0f, "%.2f"))
		{
			position.x = v3[0];
			position.y = v3[1];
			position.z = v3[2];
		}
	}

	void ImGuiRenderer::drawPosition(std::string& name, Vector3& position)
	{
		name = "##" + name;
		float v3[3]{ position.x, position.y, position.z };
		ImGui::SetNextItemWidth(160.0f);
		if (ImGui::DragFloat3(name.c_str(), v3, 0.05f, 0.0f, 0.0f, "%.2f"))
		{
			position.x = v3[0];
			position.y = v3[1];
			position.z = v3[2];
		}
	}

	void ImGuiRenderer::drawModel(Object& obj, int index)
	{
		ImGui::AlignTextToFramePadding();
		std::string label_text = std::to_string(index) + ". " + obj.name;
		ImGui::Checkbox(label_text.c_str(), &obj.enabled);
		float w = ImGui::GetItemRectSize().x;
		ImGui::SameLine(0, 150 - w);
		if (ImGui::BeginPopup(label_text.c_str()))
		{
			for (const std::string& shader_name : ResourceManager::getAllShaderNames())
			{
				if (ImGui::Button(shader_name.c_str(), ImVec2(110, 20)))
				{
					obj.material.shader = ResourceManager::getShader(shader_name);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		std::string button_name = obj.material.shader.name + "##" + label_text;
		if (ImGui::Button(button_name.c_str(), ImVec2(110, 20)))
		{
			ImGui::OpenPopup(label_text.c_str());
		}
		ImGui::SameLine();
		drawColor(label_text, obj.material.color);
		drawPosition("Position: ", label_text, obj.position);
		drawPosition("Rotation: ", label_text, obj.rotation);
	}

	void ImGuiRenderer::drawFPS(float delta_time)
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (std::to_string((int)(1.0f / delta_time)) + " fps").c_str());
	}

	void ImGuiRenderer::drawCountVertices()
	{
		auto s_ptr_scene = scene.lock();
		std::string vertices = "vertices: " + std::to_string(s_ptr_scene.get()->getCountVertices());
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), vertices.c_str());
	}

	void ImGuiRenderer::drawPolygonModeSettings()
	{
		if (state != nullptr)
		{
			std::string label_text = "Wireframe";
			if (ImGui::Checkbox(label_text.c_str(), &state->polygon_mode_enabled))
			{
				glPolygonMode(GL_FRONT_AND_BACK, state->polygon_mode_enabled ? GL_LINE : GL_FILL);
			}
		}
	}

	void ImGuiRenderer::drawNormalMapsSettings()
	{
		if (state != nullptr)
		{
			std::string label_text = "Normal maps";
			if (ImGui::Checkbox(label_text.c_str(), &state->normal_maps_enabled))
			{
				auto sptr_scene = scene.lock();
				if (sptr_scene != nullptr)
				{
					sptr_scene->enableNormalMaps(state->normal_maps_enabled);
				}
			}
		}
	}

	void ImGuiRenderer::drawScreenSettings()
	{
		std::string label_text = "Fullscreen";
		auto s_ptr_window = window.lock();
		bool is_fullscreen = s_ptr_window->isFullscreen();
		if (ImGui::Checkbox(label_text.c_str(), &is_fullscreen))
		{
			s_ptr_window->setFullscreen(is_fullscreen);
		}
	}

	void ImGuiRenderer::shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	//TODO: move to pointlight helper and create algorithm
	// diat min 0, max 3250
	void ImGuiRenderer::setPointLightDistance(PointLightData& point_light)
	{
		int distance[] = { 7, 13, 20, 32, 50, 65, 100, 160, 200, 325, 600, 3250 };
		double linear[] = { 0.7, 0.35, 0.22, 0.14, 0.09, 0.07, 0.045, 0.027, 0.022, 0.014, 0.007, 0.0014 };
		double quadratic[] = { 1.8, 0.44, 0.2, 0.07, 0.032, 0.017, 0.0075, 0.0028, 0.0019, 0.0007, 0.0002, 0,000007 };

		int index = 0;
		for (int value : distance)
		{
			if (value >= point_light.distance)
			{
				break;
			}
			index++;
		}

		point_light.linear = linear[index];
		point_light.quadratic = quadratic[index];
	}
}