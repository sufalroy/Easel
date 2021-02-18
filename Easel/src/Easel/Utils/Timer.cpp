#include "Precompiled.h"
#include "Easel/Utils/Timer.h"

namespace Easel {

	float Timer::GetTimedMS() {
		float time = Duration(m_LastTime, Now(), 1000.0f);
		m_LastTime = Now();
		return time;
	}
}
