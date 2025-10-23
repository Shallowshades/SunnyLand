#include "context.h"
#include "../input/input_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../resource/resource_manager.h"
#include <spdlog/spdlog.h>

namespace engine::core {
engine::core::Context::Context(engine::input::InputManager& inputManager, engine::render::Renderer& renderer, engine::render::Camera& camera, engine::resource::ResourceManager& resourceManager) 
	: mInputManager(inputManager)
	, mRenderer(renderer)
	, mCamera(camera)
	, mResourceManager(resourceManager)
{
	spdlog::trace("{} 上下文创建并初始化, 包含输入管理器,渲染器,相机和资源管理器.");
}

engine::input::InputManager& Context::getInputManager() const {
	return mInputManager;
}

engine::render::Renderer& Context::getRenderer() const {
	return mRenderer;
}

engine::render::Camera& Context::getCamera() const {
	return mCamera;
}

engine::resource::ResourceManager& Context::getResourceManager() const {
	return mResourceManager;
}
}
