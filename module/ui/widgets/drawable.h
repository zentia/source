#pragma once

namespace source_runtime::ui
{
	class drawable
	{
	public:
		virtual void draw() = 0;
	protected:
		virtual ~drawable() = default;
	};
}
