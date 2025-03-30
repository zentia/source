#pragma once
#include "Modules/ui/widgets/data_widget.h"

namespace source_runtime::ui
{
	class text : public data_widget<std::string>
	{
	public:
		explicit text(const std::string&& content = "");
		std::string m_content;
	protected:
		void draw_impl() override;
	};
}
