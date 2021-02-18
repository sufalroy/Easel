#pragma once

#include "Easel/Core/Base.h"

namespace Easel {

	class EASEL_EXPORT Timestep {

	public:
		explicit inline Timestep(float initialTime)
			: m_Timestep(0.0f), m_LastTime(initialTime), m_Elapsed(0.0f) {

		}

		inline void Update(float currentTime) {
			m_Timestep = currentTime - m_LastTime;
			m_LastTime = currentTime;
			m_Elapsed += m_Timestep;
		}

		inline float GetMilliseconds() const { return m_Timestep; }
		inline float GetSeconds() const { return m_Timestep * 0.001f; }

		inline float GetElapsedMilliseconds() const { return m_Elapsed; }
		inline float GetElapsedSeconds() const { return m_Elapsed * 0.001f; }

	private:
		float m_Timestep;
		float m_LastTime;
		float m_Elapsed;
	};
}
