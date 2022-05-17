#include "pch.h"

#include "ApplicationState.h"
#include "Camera.h"
#include "ImguiUI.h"
#include "BasicComponents.h"

#include "tiny_gltf.h"
#include "Vertex.h"
namespace gltf = tinygltf;

LRESULT CALLBACK WindowProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
using namespace DirectX;
int main(int argc, char** argv)
{
	WNDCLASSEX wnd_class{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_VREDRAW | CS_HREDRAW,
		.lpfnWndProc = WindowProc,
		.hInstance = GetModuleHandle(nullptr),
		.lpszClassName = L"MainWindowClass",
	};
	RegisterClassEx(&wnd_class);

	HWND window = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wnd_class.lpszClassName, L"Main Window",
		WS_OVERLAPPEDWINDOW, 0, 0, 1280, 720, nullptr, nullptr, wnd_class.hInstance, nullptr);

	if (window == nullptr)
	{
		fmt::print("Failed to create a window\n");
		return EXIT_FAILURE;
	}
	ApplicationState::Get().window = window;
	ShowWindow(ApplicationState::Get().window, SHOW_OPENWINDOW);

	ApplicationState::Get().renderer = new D3D11Renderer;
	ApplicationState::Get().renderer->Init((void*)ApplicationState::Get().window);

	std::vector<float> tri_verts =
	{// x, y, z, u,v
			 -0.5f,  0.5f, 0.0f, 0.f, 0.f,
			0.5f, -0.5f, 0.0f, 1.f, 1.f,
			-0.5f, -0.5f, 0.0f, 0.f, 1.f,
			-0.5f,  0.5f, 0.0f, 0.f, 0.f,
			0.5f,  0.5f, 0.0f, 1.f, 0.f,
			0.5f, -0.5f, 0.0f, 1.f, 1.f,

			-0.5f,  0.5f, 0.5, 0.f, 0.f,
			0.5f, -0.5f, 0.5, 1.f, 1.f,
			-0.5f, -0.5f, 0.5, 0.f, 1.f,
			-0.5f,  0.5f, 0.5, 0.f, 0.f,
			0.5f,  0.5f, 0.5, 1.f, 0.f,
			0.5f, -0.5f, 0.5, 1.f, 1.f,
	};
	{
		gltf::Model model;
		gltf::TinyGLTF loader;
		bool load_res = loader.LoadBinaryFromFile(&model, nullptr, nullptr, "assets/chief.glb");

		for (size_t i = 0; i < model.meshes.size(); i++) {
			std::vector<Vec3> model_verts;
			std::vector<Vec3> model_norms;
			std::vector<Vec2> model_uvs;

			auto& mesh = model.meshes[i];
			auto& p1 = mesh.primitives.front();
			auto& position_buff = model.bufferViews[p1.attributes["POSITION"]];
			auto& normal_buff = model.bufferViews[p1.attributes["NORMAL"]];
			auto& uv_buff = model.bufferViews[p1.attributes["TEXCOORD_0"]];
			auto& inds = model.bufferViews[p1.indices];

			auto model_verts_size = position_buff.byteLength / sizeof(Vec3);
			model_verts.resize(model_verts_size);
			std::memcpy((void*)model_verts.data(), (void*)(model.buffers[position_buff.buffer].data.data() + position_buff.byteOffset), position_buff.byteLength);

			auto model_norms_size = normal_buff.byteLength / sizeof(Vec3);
			model_norms.resize(model_norms_size);
			std::memcpy((void*)model_norms.data(), (void*)(model.buffers[normal_buff.buffer].data.data() + normal_buff.byteOffset), normal_buff.byteLength);

			auto model_uvs_size = uv_buff.byteLength / sizeof(Vec2);
			model_uvs.resize(model_uvs_size);
			std::memcpy((void*)model_uvs.data(), (void*)(model.buffers[uv_buff.buffer].data.data() + uv_buff.byteOffset), uv_buff.byteLength);

			std::vector<Vertex> model_data;
			model_data.reserve(model_verts.size());
			for (auto it = 0; it < model_verts.size(); it++)
			{
				Vertex v{};
				v.position = model_verts[it];
				if (it < model_norms.size()) {
					v.normal = model_norms[it];
				}
				if (it < model_uvs.size()) {
					v.uv = model_uvs[it];
				}
				model_data.push_back(v);
			}


			size_t model_inds_size;
			auto& acc = model.accessors[p1.indices];
			std::vector<uint32_t> conv_indices;
			switch (acc.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
				conv_indices.resize(inds.byteLength / sizeof(uint32_t));
				std::memcpy((void*)conv_indices.data(), (void*)(model.buffers[inds.buffer].data.data() + inds.byteOffset), inds.byteLength);
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
				std::vector<unsigned short> model_inds;
				model_inds.resize(inds.byteLength / sizeof(unsigned short));
				std::memcpy((void*)model_inds.data(), (void*)(model.buffers[inds.buffer].data.data() + inds.byteOffset), inds.byteLength);
				conv_indices.reserve(model_inds.size());
				for (auto index : model_inds) {
					conv_indices.push_back((uint32_t)index);
				}
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_SHORT: {
				std::vector<short> model_inds;
				model_inds.resize(inds.byteLength / sizeof(short));
				std::memcpy((void*)model_inds.data(), (void*)(model.buffers[inds.buffer].data.data() + inds.byteOffset), inds.byteLength);
				conv_indices.reserve(model_inds.size());
				for (auto index : model_inds) {
					conv_indices.push_back((uint32_t)index);
				}
				break;
			}
			default:
				break;
			}

			const auto tri = ApplicationState::Get().entities.create();
			bool use_default_texture = true;
			if (model.textures.size() > 0) {
				auto& tex = model.textures[0];
				if (tex.source > -1) {
					use_default_texture = false;
					auto& image = model.images[tex.source];
					ApplicationState::Get().entities.emplace<Renderable>(tri, L"VertexShader.cso", L"PixelShader.cso", model_data, conv_indices, image);
				}
			}
			if (use_default_texture) {
				ApplicationState::Get().entities.emplace<Renderable>(tri, L"VertexShader.cso", L"PixelShader.cso", model_data, conv_indices, 0, "assets/chekerboard.png");
			}
			ApplicationState::Get().entities.emplace<Transform>(tri, Vector3(1.0f, 1.0f, 1.0f), Vector3(DirectX::XMConvertToRadians(180), 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
			ApplicationState::Get().entities.emplace<Name>(tri, model.meshes[i].name.c_str());
		}
	}
	{
		gltf::Model model;
		gltf::TinyGLTF loader;
		bool load_res = loader.LoadBinaryFromFile(&model, nullptr, nullptr, "assets/cube.glb");

		for (size_t i = 0; i < model.meshes.size(); i++) {
			std::vector<Vec3> model_verts;
			std::vector<Vec3> model_norms;
			std::vector<Vec2> model_uvs;

			auto& mesh = model.meshes[i];
			auto& p1 = mesh.primitives.front();
			auto& position_buff = model.bufferViews[p1.attributes["POSITION"]];
			auto& normal_buff = model.bufferViews[p1.attributes["NORMAL"]];
			auto& uv_buff = model.bufferViews[p1.attributes["TEXCOORD_0"]];
			auto& inds = model.bufferViews[p1.indices];

			auto model_verts_size = position_buff.byteLength / sizeof(Vec3);
			model_verts.resize(model_verts_size);
			std::memcpy((void*)model_verts.data(), (void*)(model.buffers[position_buff.buffer].data.data() + position_buff.byteOffset), position_buff.byteLength);

			auto model_norms_size = normal_buff.byteLength / sizeof(Vec3);
			model_norms.resize(model_norms_size);
			std::memcpy((void*)model_norms.data(), (void*)(model.buffers[normal_buff.buffer].data.data() + normal_buff.byteOffset), normal_buff.byteLength);

			auto model_uvs_size = uv_buff.byteLength / sizeof(Vec2);
			model_uvs.resize(model_uvs_size);
			std::memcpy((void*)model_uvs.data(), (void*)(model.buffers[uv_buff.buffer].data.data() + uv_buff.byteOffset), uv_buff.byteLength);

			std::vector<Vertex> model_data;
			model_data.reserve(model_verts.size());
			for (auto it = 0; it < model_verts.size(); it++)
			{
				Vertex v{};
				v.position = model_verts[it];
				if (it < model_norms.size()) {
					v.normal = model_norms[it];
				}
				if (it < model_uvs.size()) {
					v.uv = model_uvs[it];
				}
				model_data.push_back(v);
			}


			size_t model_inds_size;
			auto& acc = model.accessors[p1.indices];
			std::vector<uint32_t> conv_indices;
			switch (acc.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
				conv_indices.resize(inds.byteLength / sizeof(uint32_t));
				std::memcpy((void*)conv_indices.data(), (void*)(model.buffers[inds.buffer].data.data() + inds.byteOffset), inds.byteLength);
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
				std::vector<unsigned short> model_inds;
				model_inds.resize(inds.byteLength / sizeof(unsigned short));
				std::memcpy((void*)model_inds.data(), (void*)(model.buffers[inds.buffer].data.data() + inds.byteOffset), inds.byteLength);
				conv_indices.reserve(model_inds.size());
				for (auto index : model_inds) {
					conv_indices.push_back((uint32_t)index);
				}
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_SHORT: {
				std::vector<short> model_inds;
				model_inds.resize(inds.byteLength / sizeof(short));
				std::memcpy((void*)model_inds.data(), (void*)(model.buffers[inds.buffer].data.data() + inds.byteOffset), inds.byteLength);
				conv_indices.reserve(model_inds.size());
				for (auto index : model_inds) {
					conv_indices.push_back((uint32_t)index);
				}
				break;
			}
			default:
				break;
			}

			const auto tri = ApplicationState::Get().entities.create();
			bool use_default_texture = true;
			if (model.textures.size() > 0) {
				auto& tex = model.textures[0];
				if (tex.source > -1) {
					use_default_texture = false;
					auto& image = model.images[tex.source];
					ApplicationState::Get().entities.emplace<Renderable>(tri, L"VertexShader.cso", L"PixelShader.cso", model_data, conv_indices, image);
				}
			}
			if (use_default_texture) {
				ApplicationState::Get().entities.emplace<Renderable>(tri, L"VertexShader.cso", L"PixelShader.cso", model_data, conv_indices, 0, "assets/momoko.png");
			}
			ApplicationState::Get().entities.emplace<Transform>(tri, Vector3(0.0f, -2.0f, 0.0f), Vector3(DirectX::XMConvertToRadians(0), 0.0f, 0.0f), Vector3(10.0f, 0.1f, 10.0f));
			ApplicationState::Get().entities.emplace<Name>(tri, "MeuCubo");
		}
	}

	std::unique_ptr<DirectX::Keyboard> keyboard = std::make_unique<DirectX::Keyboard>();
	std::unique_ptr<DirectX::Mouse> mouse = std::make_unique<DirectX::Mouse>();
	std::unique_ptr<GamePad> gamepad = std::make_unique<GamePad>();

	auto camera = ApplicationState::Get().entities.create();
	ApplicationState::Get().entities.emplace<Camera>(camera);
	ApplicationState::Get().entities.emplace<Name>(camera, "Camera");
	{
		auto& cam = ApplicationState::Get().entities.get<Camera>(camera);
		cam.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), 1920.0f / 1080.0f, 0.1f, 1000.0f);
		//cam.projection = cam.projection.Transpose();
		cam.view = cam.getView();
	}

	//auto teapot = ApplicationState::Get().entities.create();
	//ApplicationState::Get().entities.emplace<std::unique_ptr<DirectX::GeometricPrimitive>>(teapot, DirectX::GeometricPrimitive::CreateTeapot(((D3D11Renderer*)ApplicationState::Get().renderer)->context, 1.0f, 8, false));
	//ApplicationState::Get().entities.emplace<Transform>(teapot, Vector3(2.0f, 2.0f, 0.0f), Vector3(0.0f, 0.0f, DirectX::XMConvertToRadians(180)), Vector3(1.0f, 1.0f, 1.0f));
	//ApplicationState::Get().entities.emplace<Name>(teapot, "Teapot");

	//auto cube = ApplicationState::Get().entities.create();
	//ApplicationState::Get().entities.emplace<std::unique_ptr<DirectX::GeometricPrimitive>>(cube, DirectX::GeometricPrimitive::CreateCube(((D3D11Renderer*)ApplicationState::Get().renderer)->context, 1.0f, false));
	//ApplicationState::Get().entities.emplace<Transform>(cube, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, DirectX::XMConvertToRadians(180)), Vector3(1.0f, 1.0f, 1.0f));
	//ApplicationState::Get().entities.emplace<Name>(cube, "Cube");

	auto imgui_entity = ApplicationState::Get().entities.create();
	ApplicationState::Get().entities.emplace<ImguiUI>(imgui_entity);

	auto lightEntity = ApplicationState::Get().entities.create();
	ApplicationState::Get().entities.emplace<Transform>(lightEntity, Vector3(2.0f, 2.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
	ApplicationState::Get().entities.emplace<Name>(lightEntity, "LightPoint");
	ApplicationState::Get().entities.emplace<Light>(lightEntity, Light{ .intensity = 50 });
	ApplicationState::Get().entities.emplace<std::unique_ptr<DirectX::GeometricPrimitive>>(lightEntity, DirectX::GeometricPrimitive::CreateSphere(((D3D11Renderer*)ApplicationState::Get().renderer)->context, 0.5));


	ApplicationState::Get().running = true;
	while (ApplicationState::Get().running)
	{
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};
		if (msg.message == WM_QUIT) {
			ApplicationState::Get().running = false;
			break;
		}
		if (DirectX::Keyboard::Get().GetState().Escape) {
			ApplicationState::Get().running = false;
			break;
		}
		static uint64_t start = GetTickCount64();
		uint64_t current = GetTickCount64();
		auto delta = (current - start) / 1000.f;
		start = current;
		{
			auto& cam = ApplicationState::Get().entities.get<Camera>(camera);
			auto move_speed = cam.move_speed * delta;
			if (DirectX::Keyboard::Get().GetState().W) {
				cam.Position += move_speed * -cam.Front;
			}
			if (DirectX::Keyboard::Get().GetState().S) {
				cam.Position += move_speed * cam.Front;
			}
			if (DirectX::Keyboard::Get().GetState().A) {
				auto v = cam.Up;
				v.Cross(cam.Front);
				v.Normalize();
				cam.Position += move_speed * -v;
			}
			if (DirectX::Keyboard::Get().GetState().D) {
				auto v = cam.Up;
				v.Cross(cam.Front);
				v.Normalize();
				cam.Position += move_speed * v;
			}

			if (GamePad::Get().GetState(0).IsRightShoulderPressed()) {
				cam.Position.y += move_speed;
			}
			if (GamePad::Get().GetState(0).IsLeftShoulderPressed()) {
				cam.Position.y -= move_speed;
			}

			float yaw = cam.getYaw();
			float pitch = cam.getPitch();
			yaw -= (GamePad::Get().GetState(0).thumbSticks.rightX * cam.mouse_sensitivity) * delta;
			pitch += (GamePad::Get().GetState(0).thumbSticks.rightY * cam.mouse_sensitivity) * delta;
			auto pitch_degree = DirectX::XMConvertToDegrees(pitch);
			if (pitch_degree > 85) {
				pitch = DirectX::XMConvertToRadians(85);
			}
			if (pitch_degree < -85) {
				pitch = DirectX::XMConvertToRadians(-85);
			}
			cam.updateRollPitchYaw(0.0f, pitch, yaw);

			cam.Position -= cam.Front * (GamePad::Get().GetState(0).thumbSticks.leftY * move_speed);
			cam.Position -= cam.Right * (GamePad::Get().GetState(0).thumbSticks.leftX * move_speed);

		}
		auto& ui = ApplicationState::Get().entities.get<ImguiUI>(imgui_entity);
		ApplicationState::Get().renderer->Render(ApplicationState::Get().entities);
		ui.Draw(ApplicationState::Get().entities, ApplicationState::Get().renderer->GetFramebuffer());
		ApplicationState::Get().renderer->Present();
	}
	ApplicationState::Get().renderer->Dispose();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	return EXIT_SUCCESS;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}

	LRESULT return_result = 0;
	switch (msg)
	{
	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
		ApplicationState::Get().running = false;
		break;
	}
	default:
		return_result = DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return return_result;
}