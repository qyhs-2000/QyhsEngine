#include "editor_ui.h"



#include "editor/include/editor_global_context.h"
#include "editor/include/editor_global_context.h"

#include "function/framework/world/world_manager.h"
#include "function/global/global_context.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "resource/config_manager/config_manager.h"
#include "editor/include/editor_input_manager.h"
#include "function/framework/component/transform/transform_component.h"
#include "engine.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <stb_image.h>
namespace QYHS
{
	void  drawVecControl(const std::string& label,QYHS::Vector3& values,
		float resetValue = 0.0f,float columnWidth = 100.0f);
	std::vector<std::pair<std::string, bool>> tree_node_state;			//the state of node in each depth
	int tree_node_depth = -1;
	EditorUI::EditorUI()
	{
		m_editor_ui_creators["TreeNodePush"] = [this](const std::string& name, void* value_ptr)->void {
			static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
			bool node_state = false;
			++tree_node_depth;
			if (tree_node_depth > 0)
			{
				if (tree_node_state[tree_node_depth - 1].second)
				{
					node_state = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
				}
				else
				{
					tree_node_state.emplace_back(name.c_str(), node_state);
					return;
				}
			}
			else
			{
				node_state = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
			}
			tree_node_state.emplace_back(name.c_str(), node_state);
			};

		m_editor_ui_creators["TreeNodePop"] = [this](const std::string& name, void* value_ptr)->void
			{
				if (tree_node_state[tree_node_depth].second)
				{
					ImGui::TreePop();
				}
				tree_node_state.pop_back();
				--tree_node_depth;
			};

		m_editor_ui_creators["Transform"] = [this](const std::string& name, void* value_ptr)->void
			{
				if (!tree_node_state[tree_node_depth].second)return;
				Transform* transform = static_cast<Transform*>(value_ptr);
				Vector3 rotation;
				rotation.x = transform->m_rotation.getPitch(false).valueDegrees();
				rotation.y = transform->m_rotation.getRoll(false).valueDegrees();
				rotation.z = transform->m_rotation.getYaw(false).valueDegrees();
				drawVecControl("Position", transform->m_position);
				drawVecControl("Rotation", rotation);
				drawVecControl("Scale", transform->m_scale);

				// calculate half of each component
				float half_angle_x = Math::degreesToRadians(rotation.x / 2);
				float half_angle_y = Math::degreesToRadians(rotation.y / 2);
				float half_angle_z = Math::degreesToRadians(rotation.z / 2);

				// modify rotation quaternion
				transform->m_rotation.x = Math::sin(half_angle_x) * Math::cos(half_angle_y) * Math::cos(half_angle_z) -
					Math::cos(half_angle_x) * Math::sin(half_angle_y) * Math::sin(half_angle_z);

				transform->m_rotation.y = Math::cos(half_angle_x) * Math::sin(half_angle_y) * Math::cos(half_angle_z) +
					Math::sin(half_angle_x) * Math::cos(half_angle_y) * Math::sin(half_angle_z);

				transform->m_rotation.z = Math::cos(half_angle_x) * Math::cos(half_angle_y) * Math::sin(half_angle_z) -
					Math::sin(half_angle_x) * Math::sin(half_angle_y) * Math::cos(half_angle_z);

				transform->m_rotation.w = Math::cos(half_angle_x) * Math::cos(half_angle_y) * Math::cos(half_angle_z) +
					Math::sin(half_angle_x) * Math::sin(half_angle_y) * Math::sin(half_angle_z);
				transform->m_rotation.normalise();
				g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
			};

	}

	void EditorUI::preRender()
	{
		showEditorUI();
	}

	inline void windowContentScaleUpdate(float content_scale)
	{
#if defined(__GNUC__) && defined(__MACH__)
		float font_scale = fmaxf(1.0f, scale);
		ImGui::GetIO().FontGlobalScale = 1.0f / font_scale;
#endif
	}

	inline void windowContentScaleCallback(GLFWwindow* window, float xscale, float yscale)
	{
		windowContentScaleUpdate(fmaxf(xscale, yscale));
	}

	void EditorUI::initialize(WindowUIInitInfo* info)
	{
		WindowUI::initialize(info);

		std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		//update imgui content scale
		float x_scale, y_scale;
		glfwGetWindowContentScale(info->window->getWindow(), &x_scale, &y_scale);
		float content_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));
		windowContentScaleUpdate(content_scale);
		glfwSetWindowContentScaleCallback(info->window->getWindow(), windowContentScaleCallback);

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigDockingAlwaysTabBar = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.Fonts->AddFontFromFileTTF(
			config_manager->getEditorFontPath().generic_string().data(), content_scale * 16, nullptr, nullptr);
		io.Fonts->Build();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(1.0, 0);
		style.FramePadding = ImVec2(14.0, 2.0f);
		style.ChildBorderSize = 0.0f;
		style.FrameRounding = 5.0f;
		style.FrameBorderSize = 1.5f;

		// set imgui color style
		setUIColorStyle();

		// setup window icon
		GLFWimage   window_icon[2];
		std::string big_icon_path_string = config_manager->getEditorBigIconPath().generic_string();
		std::string small_icon_path_string = config_manager->getEditorSmallIconPath().generic_string();
		window_icon[0].pixels =
			stbi_load(big_icon_path_string.data(), &window_icon[0].width, &window_icon[0].height, 0, 4);
		window_icon[1].pixels =
			stbi_load(small_icon_path_string.data(), &window_icon[1].width, &window_icon[1].height, 0, 4);
		glfwSetWindowIcon(info->window->getWindow(), 2, window_icon);
		stbi_image_free(window_icon[0].pixels);
		stbi_image_free(window_icon[1].pixels);

		// initialize imgui vulkan render backend
		info->render_system->initializeUIRenderBackend(this);
	}

	void EditorUI::setUIColorStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(0.4745f, 0.4745f, 0.4745f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.0078f, 0.0078f, 0.0078f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.047f, 0.047f, 0.047f, 0.5411f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.196f, 0.196f, 0.196f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.294f, 0.294f, 0.294f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.0039f, 0.0039f, 0.0039f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.0039f, 0.0039f, 0.0039f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
		colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_CheckMark];
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.3647f, 0.0392f, 0.2588f, 0.50f);
		colors[ImGuiCol_Button] = ImVec4(0.0117f, 0.0117f, 0.0117f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.0235f, 0.0235f, 0.0235f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.0353f, 0.0196f, 0.0235f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.1137f, 0.0235f, 0.0745f, 0.588f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(6.0f / 255.0f, 6.0f / 255.0f, 8.0f / 255.0f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 150.0f / 255.0f);
		colors[ImGuiCol_TabActive] = ImVec4(47.0f / 255.0f, 6.0f / 255.0f, 29.0f / 255.0f, 1.0f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 25.0f / 255.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(6.0f / 255.0f, 6.0f / 255.0f, 8.0f / 255.0f, 200.0f / 255.0f);
		colors[ImGuiCol_DockingPreview] = ImVec4(47.0f / 255.0f, 6.0f / 255.0f, 29.0f / 255.0f, 0.7f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(2.0f / 255.0f, 2.0f / 255.0f, 2.0f / 255.0f, 1.0f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	void EditorUI::showGameInstancesUI(bool* p_bool_show)
	{
		if (*p_bool_show == false) return;
		ImGuiWindowFlags window_flag = ImGuiWindowFlags_None;

		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		if (!ImGui::Begin("World Objects", p_bool_show, window_flag))
		{
			ImGui::End();
			return;
		}

		std::shared_ptr<Level> current_level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>> game_objects = current_level->getAllGameObjects();

		for (auto& gobject_pair : game_objects)
		{
			GameObjectID id = gobject_pair.first;
			std::shared_ptr<GameObject> gobject = gobject_pair.second;
			std::string object_name = gobject->m_name;
			if (object_name.size() > 0)
			{
				if (ImGui::Selectable(object_name.c_str(),
					g_editor_global_context.m_scene_manager->m_selected_game_object_id == id))
				{
					if (g_editor_global_context.m_scene_manager->m_selected_game_object_id != id)
					{
						g_editor_global_context.m_scene_manager->GObjectSelected(id);
					}
					else
					{
						g_editor_global_context.m_scene_manager->GObjectSelected(k_invalid_gobject_id);
					}
					break;
				}
			}
		}
		ImGui::End();
	}

	void EditorUI::showComponentDetails(bool* p_bool_show)
	{
		if (!p_bool_show) return;
		GameObjectID gobject_id = g_editor_global_context.m_scene_manager->m_selected_game_object_id;
		if (!ImGui::Begin("Component Details", p_bool_show))
		{
			ImGui::End();
			return;
		}
		if (gobject_id == k_invalid_gobject_id)
		{
			ImGui::End();
			return;
		}

		ImGui::Text("Name");
		ImGui::SameLine();
		std::shared_ptr<GameObject> selected_object = g_editor_global_context.m_scene_manager->getSelectedGameObject().lock();
		const std::string name = selected_object->m_name;
		char cname[128];
		memset(cname, 0, 128);
		memcpy(cname, name.c_str(), name.size());
		ImGui::InputText("##Name", cname, IM_ARRAYSIZE(cname), ImGuiInputTextFlags_ReadOnly);
		static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
		auto selected_object_components = selected_object->getComponents();
		for (auto& component : selected_object_components)
		{
			m_editor_ui_creators["TreeNodePush"](component.getTypeName(), nullptr);
			Reflection::ReflectionInstance object_instance = Reflection::ReflectionInstance(Reflection::TypeMeta::newMetaFromName(component.getTypeName().c_str()), component.operator->());
			createClassUI(object_instance);
			m_editor_ui_creators["TreeNodePop"](component.getTypeName(), nullptr);
		}

		ImGui::End();
	}

	void EditorUI::showEditorGameUI(bool *p_show_game_engine_window)
	{
		if (!*p_show_game_engine_window)
		{
			return;
		}
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;
		if (!ImGui::Begin("Game Engine", p_show_game_engine_window, window_flags))
		{
			ImGui::End();
			return;
		}

		static bool trans_button_ckecked  = false;
		static bool rotate_button_ckecked = false;
		static bool scale_button_ckecked  = false;

		switch (g_editor_global_context.m_scene_manager->getEditorAxisMode())
		{
		case EditorSceneManager::EditorAxisType::Translate:
			trans_button_ckecked  = true;
			rotate_button_ckecked = false;
			scale_button_ckecked  = false;
			break;
		case EditorSceneManager::EditorAxisType::Rotate:
			trans_button_ckecked  = false;
			rotate_button_ckecked = true;
			scale_button_ckecked  = false;
			break;
		case EditorSceneManager::EditorAxisType::Scale:
			trans_button_ckecked  = false;
			rotate_button_ckecked = false;
			scale_button_ckecked  = true;
			break;
		default:
			break;
		}
		
		if (ImGui::BeginMenuBar())
		{
			ImGui::Indent(10.f);
			drawAxisToggleButton("Trans", trans_button_ckecked, EditorSceneManager::EditorAxisType::Translate);
			ImGui::Unindent();

			ImGui::SameLine();
			drawAxisToggleButton("Rotate", rotate_button_ckecked, EditorSceneManager::EditorAxisType::Rotate);

			ImGui::SameLine();
			drawAxisToggleButton("Scale", scale_button_ckecked, EditorSceneManager::EditorAxisType::Scale);
			ImGui::SameLine();

			float indent_val = 0.0f;

#if defined(__GNUC__) && defined(__MACH__)
			float indent_scale = 1.0f;
#else // Not tested on Linux
			float x_scale, y_scale;
			glfwGetWindowContentScale(g_editor_global_context.m_window_system->getWindow(), &x_scale, &y_scale);
			float indent_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));
#endif
			indent_val = g_editor_global_context.m_input_manager->getEngineWindowSize().x - 100.0f * indent_scale;

			ImGui::Indent(indent_val);
			if (g_is_editor_mode)
			{
				ImGui::PushID("Editor Mode");
				if (ImGui::Button("Editor Mode"))
				{
					g_is_editor_mode = false;
					g_runtime_global_context.m_window_system->disableCursor(true);
					g_runtime_global_context.m_window_system->setFocusMode(true);
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::PushID("Editor Mode");
				if (ImGui::Button("Game Mode"))
				{
					g_is_editor_mode = true;
					g_runtime_global_context.m_render_system->changeRenderCameraType(RenderCameraType::Editor);
				}
				ImGui::PopID();
			}
			
			ImGui::Unindent();
			ImGui::EndMenuBar();
			Vector2 render_target_window_position{ 0,0 };
			Vector2 render_target_window_size{ 0,0 };
			ImRect menu_bar_rect = ImGui::GetCurrentWindow()->MenuBarRect();
			render_target_window_position.x = ImGui::GetWindowPos().x;
			render_target_window_position.y = menu_bar_rect.Max.y;
			auto pos_y = ImGui::GetWindowPos().y;
			auto size_y = g_runtime_global_context.m_window_system->getHeight();
			auto max_y = menu_bar_rect.Max.y;
			render_target_window_size.x = ImGui::GetWindowSize().x;
			render_target_window_size.y = ImGui::GetWindowPos().y + size_y - menu_bar_rect.Max.y;

			g_runtime_global_context.m_render_system->updateEngineContentViewport(render_target_window_position, render_target_window_size);
			g_editor_global_context.m_input_manager->setEngineWindowPos(render_target_window_position);
			g_editor_global_context.m_input_manager->setEngineWindowSize(render_target_window_size);

		}
		ImGui::End();
	}

	void EditorUI::drawAxisToggleButton(std::string name, bool button_checked, EditorSceneManager::EditorAxisType axis_type)
	{
		if (button_checked)
		{
			ImGui::PushID(name.c_str());
			ImVec4 check_button_color = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
			ImGui::PushStyleColor(ImGuiCol_Button,
				ImVec4(check_button_color.x, check_button_color.y, check_button_color.z, 0.40f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, check_button_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, check_button_color);
			ImGui::Button(name.c_str());
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}
		else
		{
			if (ImGui::Button(name.c_str()))
			{
				g_editor_global_context.m_scene_manager->setEditorAxisMode(axis_type);
				g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
			}
		}
	}

	void EditorUI::createClassUI(Reflection::ReflectionInstance& object_instance)
	{
		Reflection::ReflectionInstance* reflection_instance_list = nullptr;
		int count = object_instance.m_meta.getBaseClassReflectionInstanceList(reflection_instance_list, object_instance.m_instance);
		for (int i = 0; i < count; ++i)
		{
			createClassUI(reflection_instance_list[i]);
		}
		createLeafNodeUI(object_instance);
		if (count > 0)
			delete[] reflection_instance_list;
	}

	void EditorUI::createLeafNodeUI(Reflection::ReflectionInstance& object_instance)
	{
		Reflection::FieldAccessor* fields;
		int field_count = object_instance.m_meta.getFields(fields);

		for (int i = 0; i < field_count; ++i)
		{
			Reflection::FieldAccessor field = fields[i];
			if (field.isArray())
			{
				Reflection::ArrayAccessor array_accessor;
				if (Reflection::TypeMeta::newArrayAccessorFromName(field.getFieldTypeName(), array_accessor))
				{
					void* fields_instance = field.get(object_instance.m_instance);
					int array_count = array_accessor.getSize(fields_instance);
					m_editor_ui_creators["TreeNodePush"]("[" + std::to_string(array_count) + "]", nullptr);
					auto item_type_meta = Reflection::TypeMeta::newMetaFromName(array_accessor.getElementType());
					auto item_ui_creator_iter = m_editor_ui_creators.find(item_type_meta.getTypeName());
					for (int array_index = 0; array_index < array_count; ++array_index)
					{
						if (item_ui_creator_iter == m_editor_ui_creators.end())
						{

							m_editor_ui_creators["TreeNodePush"]("[" + std::to_string(array_index) + "]", nullptr);
							Reflection::ReflectionInstance object_instance(Reflection::TypeMeta::newMetaFromName(item_type_meta.getTypeName()), array_accessor.get(array_index, fields_instance));
							createClassUI(object_instance);
							m_editor_ui_creators["TreeNodePop"]("[" + std::to_string(array_index) + "]", nullptr);
						}
						else
						{
							m_editor_ui_creators[item_type_meta.getTypeName()]("[" + std::to_string(array_index) + "]", array_accessor.get(array_index, fields_instance));
						}
					}
					m_editor_ui_creators["TreeNodePop"]("[" + std::to_string(array_count) + "]", nullptr);
				}
			}
			auto ui_creater_iterator = m_editor_ui_creators.find(field.getFieldTypeName());
			if (ui_creater_iterator == m_editor_ui_creators.end())
			{
				Reflection::TypeMeta field_meta = Reflection::TypeMeta::newMetaFromName(field.getFieldTypeName());
				if (field.getTypeMeta(field_meta))
				{
					auto child_instance = Reflection::ReflectionInstance(field_meta, field.get(object_instance.m_instance));
					m_editor_ui_creators["TreeNodePush"](field_meta.getTypeName(), nullptr);
					createClassUI(child_instance);
					m_editor_ui_creators["TreeNodePop"](field_meta.getTypeName(), nullptr);
				}

			}
			else
			{
				m_editor_ui_creators[field.getFieldTypeName()](field.getTypeName(), field.get(object_instance.m_instance));
			}
		}
		delete[]fields;
	}

	void drawVecControl(const std::string& name, QYHS::Vector3& vec, float reset_value, float column_width)
	{
		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text("%s", name.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
			vec.x = reset_value;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.55f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			vec.y = reset_value;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
			vec.z = reset_value;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	void EditorUI::showEditorMenu(bool* p_open)
	{
		ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_DockSpace;
		ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
			ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;


		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
		int window_width = g_editor_global_context.m_window_system->getWidth();
		int window_height = g_editor_global_context.m_window_system->getHeight();
		ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height), ImGuiCond_Always);
		ImGui::SetNextWindowViewport(main_viewport->ID);

		ImGui::Begin("Editor Menu", p_open, window_flags);
		ImGuiID main_docking_id = ImGui::GetID("Main Docking");
		ImGui::DockSpace(main_docking_id);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Reload Current level"))
				{
					std::cout << " reload current level" << std::endl;
				}
				if (ImGui::MenuItem("Save Current Level"))
				{
					std::cout << " Save Current Level" << std::endl;
				}
				if (ImGui::BeginMenu("Debug"))
				{
					if (ImGui::MenuItem("Debug_1"))
					{
						std::cout << " Debug_1" << std::endl;
					}
					if (ImGui::MenuItem("Debug_2"))
					{
						std::cout << " Debug_2" << std::endl;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Game Objects"))
				{
					std::cout << "Game Objects" << std::endl;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	void EditorUI::showEditorUI()
	{
		showEditorMenu(&m_editor_show_menu_ui);
		showGameInstancesUI(&m_editor_show_game_instances);
		showEditorGameUI(&m_game_engine_window_open);
		showComponentDetails(&m_editor_show_component_details);
	}


}
