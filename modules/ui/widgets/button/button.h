#pragma once
#include "Modules/ui/widgets/widget.h"
#include "eventpp/callbacklist.h"
#include <assimp/vector2.h>

namespace source_runtime::ui
{
	class button : public widget
	{
	public:
		button(const std::string&& label = "", const aiVector2D&& size = aiVector2D(0.0f, 0.0f), bool disabled = false);
		std::string m_label;
		aiVector2D m_size;
		bool m_disable = false;
		eventpp::CallbackList<void()> m_on_click;
	protected:
		void draw_impl() override;
	};
}
