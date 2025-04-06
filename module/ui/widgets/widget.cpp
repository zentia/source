#include "widget.h"

namespace source_runtime::ui
{
	void widget::set_parent(widget_container* parent)
	{
		m_container_ = parent;
	}

}
