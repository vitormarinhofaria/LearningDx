#include "pch.h"
#include "ImguiUI.h"
#include "ApplicationState.h"
#include "BasicComponents.h"
#include "Transform.h"
#include "Camera.h"

ImguiUI::ImguiUI()
{
	ImGui::CreateContext();
	this->io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplWin32_Init(ApplicationState::Get().window);
	D3D11Renderer* renderer = (D3D11Renderer*)ApplicationState::Get().renderer;
	ImGui_ImplDX11_Init(renderer->device, renderer->context);
}

ImguiUI::~ImguiUI()
{
}

void ImguiUI::Draw(entt::registry& entities, ImTextureID framebuffer)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport();

	ImGui::Begin("Framebuffer");
	ImGui::Image(framebuffer, {1920/2, 1080/2});
	ImGui::End();

	ImGui::Begin("Edit entity");
	ImGui::Separator();
	auto camera_ent = entities.view<Camera>().front();
	auto& camera = entities.get<Camera>(camera_ent);
	float pitch = camera.getPitch();
	float yaw = camera.getYaw();
	ImGui::Text("Camera");
	ImGui::DragFloat3("Position", (float*)&camera.Position, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat("Pitch", &pitch, 0.1f, 0.0, 90.0f);
	ImGui::DragFloat("Yaw", &yaw, 0.1f, 0.0, 360.0f);
	ImGui::DragFloat("Sensitivity", &camera.mouse_sensitivity, 0.1f, 0.1f, 1000.0f);
	ImGui::DragFloat("MoveSpeec", &camera.move_speed, 0.1f, 0.1f, 1000.0f);
	camera.updateRollPitchYaw(0.0f, pitch, yaw);
	ImGui::Separator();

	auto view = entities.view<const Name, Transform>();
	for (auto [ent, name, transform] : view.each()) {
		ImGui::Separator();
		ImGui::PushID((int)ent);
		bool mark_for_delete = false;
		float transform_position[3] = { transform.position.x, transform.position.y, transform.position.z };
		float transform_rotation[3] = { DirectX::XMConvertToDegrees(transform.rotation.x), DirectX::XMConvertToDegrees(transform.rotation.y), DirectX::XMConvertToDegrees(transform.rotation.z) };
		float transform_scale[3] = { transform.scale.x, transform.scale.y, transform.scale.z };
		ImGui::Text("Name: %s", name);
		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			mark_for_delete = true;
		}
		ImGui::DragFloat3("Position", transform_position, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Rotation", transform_rotation, 1.0f, 0.0f, 360.0f);
		ImGui::DragFloat3("Scale", transform_scale, 0.1f, 0.0f, 100.0f);

		entities.replace<Transform>(ent, Vector3{ transform_position }, 
			Vector3{ DirectX::XMConvertToRadians(transform_rotation[0]), DirectX::XMConvertToRadians(transform_rotation[1]), DirectX::XMConvertToRadians(transform_rotation[2]) },
			Vector3{transform_scale});

		if (mark_for_delete) {
			entities.destroy(ent);
		}
		ImGui::PopID();
		ImGui::Separator();
	}

	ImGui::End();

	ImGui::ShowDemoWindow();
}
