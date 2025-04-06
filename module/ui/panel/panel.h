#pragma once

#include "module/ui/plugins/plugin.h"
#include "module/ui/widgets/widget_container.h"
#include "module/ui/widgets/drawable.h"

namespace source_runtime::ui
{
	
	class panel : public drawable, plugin, public widget_container
	{
	public:
		void draw() override;
	};
}
