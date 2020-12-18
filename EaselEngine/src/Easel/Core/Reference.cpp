#include "Precompiled.h"
#include "Easel/Core/Reference.h"

namespace Easel {

	RefCount::RefCount() {

		m_Refcount.init();
		m_RefcountInit.init();
		m_WeakRefcount.init(0);
	}

	RefCount::~RefCount()
	{
	}

	bool RefCount::InitRef()
	{
		if (reference()) {

			if (m_RefcountInit.get() > 0) {
				m_RefcountInit.unref();
				unreference();
			}

			return true;
		}
		else {

			return false;
		}
	}

	bool RefCount::reference() {
		bool success = m_Refcount.ref();

		return success;
	}

	bool RefCount::unreference() {
		bool die = m_Refcount.unref();

		return die;
	}

	bool RefCount::weakReference() {
		bool success = m_WeakRefcount.ref();

		return success;
	}

	bool RefCount::weakUnreference() {
		bool die = m_WeakRefcount.unref() && m_Refcount.count == 0;

		return die;
	}

	int RefCount::GetReferenceCount() const {
		return m_Refcount.get();
	}

	int RefCount::GetWeakReferenceCount() const {
		return m_WeakRefcount.get();
	}
}
