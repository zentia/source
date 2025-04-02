#pragma once
#include "Modules/ui/widgets/widget.h"
#include "eventpp/callbacklist.h"
#include "glm/vec2.hpp"
//#include <assimp/vector2.h>

namespace source_runtime::ui
{
	class button : public widget
	{
	public:
		button(const std::string&& label = "", const glm::vec2&& size = glm::vec2(0.0f, 0.0f), bool disabled = false);
		std::string m_label;
		glm::vec2 m_size;
		bool m_disable = false;
		eventpp::CallbackList<void()> m_on_click;
	protected:
		void draw_impl() override;
	};
}
