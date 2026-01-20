#include "ui_button.h"
#include "state/ui_normal_state.h"
#include <spdlog/spdlog.h>

namespace engine::ui {
UIButton::UIButton(engine::core::Context& context,
	const std::string& normalSpriteId,
	const std::string& hoverSpriteId,
	const std::string& pressedSpriteId,
	const glm::vec2& position,
	const glm::vec2& size,
	std::function<void()> callback)
	: UIInteractive(context, position, size), mCallback(std::move(callback))
{
	addSprite("normal", std::make_unique<engine::render::Sprite>(normalSpriteId));
	addSprite("hover", std::make_unique<engine::render::Sprite>(hoverSpriteId));
	addSprite("pressed", std::make_unique<engine::render::Sprite>(pressedSpriteId));

	// 设置默认状态为"normal"
	setState(std::make_unique<engine::ui::state::UINormalState>(this));

	// 设置默认音效
	addSound("hover", "assets/audio/button_hover.wav");
	addSound("pressed", "assets/audio/button_click.wav");
	spdlog::trace("UIButton 构造完成");
}

void UIButton::clicked()
{
	if (mCallback) mCallback();
}

} // namespace engine::ui