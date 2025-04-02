#pragma once
#include "Modules/ui/panel/panel.h"
#include "Modules/ui/widgets/drawable.h"

namespace source_runtime::ui
{
	class canvas : public drawable
	{
	public:
		void draw() override;
	private:
		std::vector<std::reference_wrapper<panel>> m_panels_;
		bool m_is_dock_space_ = false;
	};
}
