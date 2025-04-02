#pragma once

#include <vector>

namespace source_runtime::ui
{
	class widget;
	class widget_container
	{
	public:
		template <typename T, typename ... Args>
		T& create_widget(Args&&... args)
		{
			m_widgets_.emplace_back(new T(args...));
			T& instance = *reinterpret_cast<T*>(m_widgets_.back());
			instance.SetParent(this);
			return instance;
		}
	protected:
		std::vector<widget*> m_widgets_;
	};
}
