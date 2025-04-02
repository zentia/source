#pragma once

#include "Modules/ui/plugins/plugin.h"
#include "Modules/ui/widgets/widget_container.h"
#include "Modules/ui/widgets/drawable.h"

namespace source_runtime::ui
{
	
	class panel : public drawable, plugin, public widget_container
	{
	public:
		void draw() override;
	};
}
