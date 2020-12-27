#pragma once

#include <stdint.h>

namespace Easel
{
	namespace InputCode
	{
		enum class MouseKey : uint32_t
		{
			ButtonLeft   =  0,
			ButtonMiddle =  1,
			ButtonRight  =  2,
			NoCursor     =  3,
		};
	}
}
