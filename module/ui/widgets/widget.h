#pragma once
#include "drawable.h"
#include "module/ui/plugins/plugin.h"

namespace source_runtime::ui
{
	class widget_container;

	class widget : public drawable, public plugin
	{
	public:
		~widget() {}
		void set_parent(widget_container* parent);
	protected:
		virtual void draw_impl() = 0;
		widget_container* m_container_{ nullptr };
	};
}
