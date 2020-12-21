#pragma once
#include "Easel/Core/Base.h"
#include "Easel/Core/ReferenceCounter.h"
#include "Easel/Core/OS/Memory.h"
#include "Easel/Core/Log.h"

namespace Easel {

	class EASEL_EXPORT RefCount
	{
	public:
		RefCount();
		~RefCount();

		_FORCE_INLINE_ bool IsReferenced() const {
			return m_RefcountInit.get() < 1;
		}

		bool InitRef();

		//Returns false if refcount is at zero and didn't get increased
		bool reference();
		bool unreference();

		bool weakReference();
		bool weakUnreference();

		int GetReferenceCount() const;
		int GetWeakReferenceCount() const;

	private:
		ReferenceCounter m_Refcount;
		ReferenceCounter m_RefcountInit;
		ReferenceCounter m_WeakRefcount;
	};

	template<class T>
	class Reference {
	public:
		Reference() noexcept : m_Ptr(nullptr) {}

		Reference(std::nullptr_t) noexcept : m_Ptr(nullptr) , m_Counter(nullptr) {}

		explicit Reference(T* ptr) noexcept
			: m_Ptr(nullptr)
			, m_Counter(nullptr) {

			if (ptr)
				refPointer(ptr);
		}

		Reference(const Reference<T>& other) noexcept {

			m_Ptr = nullptr;
			m_Counter = nullptr;

			ref(other);
		}

		Reference(Reference<T>&& rhs) noexcept {

			m_Ptr = nullptr;
			m_Counter = nullptr;

			ref(rhs);
		}

		template<typename U>
		_FORCE_INLINE_ Reference(const Reference<U>& moving) noexcept {
			U* movingPtr = moving.get();

			T* castPointer = static_cast<T*>(movingPtr);

			unref();

			if (castPointer != nullptr) {
				if (moving.get() == m_Ptr)
					return;

				if (moving.GetCounter() && moving.get()) {
					m_Ptr = castPointer;
					m_Counter = moving.GetCounter();
					m_Counter->reference();
				}
			}
			else {
				EASEL_CORE_ERROR("Failed to cast Reference");
			}
		}

		~Reference() noexcept {
			unref();
		}

		_FORCE_INLINE_ T* get() const {
			return m_Ptr;
		}

		_FORCE_INLINE_ RefCount* GetCounter() const {
			return m_Counter;
		}

		_FORCE_INLINE_ T* release() noexcept {
			T* tmp = nullptr;

			if (m_Counter->unreference()) {
				delete m_Counter;
				m_Counter = nullptr;
			}

			std::swap(tmp, m_Ptr);
			m_Ptr = nullptr;

			return tmp;
		}

		_FORCE_INLINE_ void reset(T* p_ptr = nullptr) {
			unref();

			m_Ptr = p_ptr;
			m_Counter = nullptr;

			if (m_Ptr != nullptr) {
				m_Counter = new RefCount();
				m_Counter->InitRef();
			}
		}

		_FORCE_INLINE_ void operator=(Reference const& rhs) {
			ref(rhs);
		}

		_FORCE_INLINE_ Reference& operator=(Reference&& rhs) noexcept {
			ref(rhs);
			return *this;
		}

		_FORCE_INLINE_ Reference& operator=(T* newData) {
			reset(newData);
			return *this;
		}

		template<typename U>
		_FORCE_INLINE_ Reference& operator=(const Reference<U>& moving) {
			U* movingPtr = moving.get();

			T* castPointer = dynamic_cast<T*>(movingPtr);

			unref();

			if (castPointer != nullptr) {

				if (moving.GetCounter() && moving.get()) {
					m_Ptr = moving.get();
					m_Counter = moving.GetCounter();
					m_Counter->reference();
				}
			}
			else {
				EASEL_CORE_ERROR("Failed to cast Reference");
			}

			return *this;
		}

		_FORCE_INLINE_ Reference& operator=(std::nullptr_t) {
			reset();
			return *this;
		}

		_FORCE_INLINE_ T* operator->() const {
			return &*m_Ptr;
		}

		_FORCE_INLINE_ T& operator*() const {
			return *m_Ptr;
		}

		_FORCE_INLINE_ T& operator[](int index) {
			assert(m_Ptr);
			return m_Ptr[index];
		}

		_FORCE_INLINE_ explicit constexpr operator bool() const {
			return m_Ptr != nullptr;
		}

		_FORCE_INLINE_ constexpr bool operator==(const T* p_ptr) const {
			return m_Ptr == p_ptr;
		}

		_FORCE_INLINE_ constexpr bool operator!=(const T* p_ptr) const {
			return m_Ptr != p_ptr;
		}

		_FORCE_INLINE_ constexpr bool operator<(const Reference<T>& p_r) const {
			return m_Ptr < p_r.m_Ptr;
		}

		_FORCE_INLINE_ constexpr bool operator==(const Reference<T>& p_r) const {
			return m_Ptr == p_r.m_Ptr;
		}

		_FORCE_INLINE_ constexpr bool operator!=(const Reference<T>& p_r) const {
			return m_Ptr != p_r.m_Ptr;
		}

		_FORCE_INLINE_ void swap(Reference& other) noexcept {
			std::swap(m_Ptr, other.m_Ptr);
			std::swap(m_Counter, other.m_Counter);
		}


		template<typename U>
		_FORCE_INLINE_ Reference<U> As() const {
			return Reference<U>(*this);
		}

	private:
		_FORCE_INLINE_ void ref(const Reference& p_from) {
			if (p_from.m_Ptr == m_Ptr)
				return;

			unref();

			m_Counter = nullptr;
			m_Ptr = nullptr;

			if (p_from.GetCounter() && p_from.get()) {
				m_Ptr = p_from.get();
				m_Counter = p_from.GetCounter();
				m_Counter->reference();
			}
		}

		_FORCE_INLINE_ void refPointer(T* ptr)
		{
			EASEL_ASSERT(ptr, "Creating shared ptr with nullptr");

			m_Ptr = ptr;
			m_Counter = new RefCount();
			m_Counter->InitRef();
		}

		_FORCE_INLINE_ void unref() {
			if (m_Counter != nullptr) {
				if (m_Counter->unreference()) {
					delete m_Ptr;

					if (m_Counter->GetWeakReferenceCount() == 0) {
						delete m_Counter;
					}

					m_Ptr = nullptr;
					m_Counter = nullptr;
				}
			}
		}

		RefCount* m_Counter = nullptr;
		T* m_Ptr = nullptr;
	};

	template<class T>
	class EASEL_EXPORT WeakReference
	{
	public:
		WeakReference() noexcept : m_Ptr(nullptr) , m_Counter(nullptr) {}

		WeakReference(std::nullptr_t) noexcept : m_Ptr(nullptr) , m_Counter(nullptr) {}

		WeakReference(const WeakReference<T>& rhs) noexcept
			: m_Ptr(rhs.m_Ptr) , m_Counter(rhs.m_Counter) {
			AddRef();
		}

		explicit WeakReference(T* ptr) noexcept : m_Ptr(ptr) {
			EASEL_ASSERT(ptr, "Creating weak ptr with nullptr");

			m_Counter = new RefCount();
			m_Counter->weakReference();
		}

		template<class U>
		WeakReference(const WeakReference<U>& rhs) noexcept
			: m_Ptr(rhs.m_Ptr) , m_Counter(rhs.m_Counter) {
			AddRef();
		}

		WeakReference(const Reference<T>& rhs) noexcept
			: m_Ptr(rhs.get()) , m_Counter(rhs.m_Counter) {
			AddRef();
		}

		~WeakReference() noexcept {
			if (m_Counter->weakUnreference())
			{
				delete m_Ptr;
			}
		}

		void AddRef() {
			m_Counter->weakReference();
		}

		bool Expired() const {
			return m_Counter ? m_Counter->GetReferenceCount() <= 0 : true;
		}

		Reference<T> Lock() const {
			if (Expired())
				return Reference<T>();
			else
				return Reference<T>(m_Ptr);
		}

		_FORCE_INLINE_ T* operator->() const {
			return &*m_Ptr;
		}

		_FORCE_INLINE_ T& operator*() const {
			return *m_Ptr;
		}

		_FORCE_INLINE_ T& operator[](int index) {
			assert(m_Ptr);
			return m_Ptr[index];
		}

		_FORCE_INLINE_ explicit operator bool() const {
			return m_Ptr != nullptr;
		}

		_FORCE_INLINE_ bool operator==(const T* p_ptr) const {
			return m_Ptr == p_ptr;
		}

		_FORCE_INLINE_ bool operator!=(const T* p_ptr) const {
			return m_Ptr != p_ptr;
		}

		_FORCE_INLINE_ bool operator<(const WeakReference<T>& p_r) const {
			return m_Ptr < p_r.m_Ptr;
		}

		_FORCE_INLINE_ bool operator==(const WeakReference<T>& p_r) const {
			return m_Ptr == p_r.m_Ptr;
		}

		_FORCE_INLINE_ bool operator!=(const WeakReference<T>& p_r) const {
			return m_Ptr != p_r.m_Ptr;
		}

	private:
		T* m_Ptr;
		RefCount* m_Counter = nullptr;
	};

	template<class T>
	class EASEL_EXPORT Owned
	{
	public:
		explicit Owned(std::nullptr_t) {
			m_Ptr = nullptr;
		}

		Owned(T* ptr = nullptr) {
			m_Ptr = ptr;
		}

		template<typename U>
		explicit Owned(U* ptr) {
			m_Ptr = dynamic_cast<T*>(ptr);
		}

		~Owned() {
			delete m_Ptr;
		}

		Owned(Owned const&) = delete;
		Owned& operator=(Owned const&) = delete;

		_FORCE_INLINE_ Owned(Owned&& moving) noexcept {
			moving.swap(*this);
		}

		_FORCE_INLINE_ Owned& operator=(Owned&& moving) noexcept {
			moving.swap(*this);
			return *this;
		}

		template<typename U>
		_FORCE_INLINE_ Owned(Owned<U>&& moving) {
			Owned<T> tmp(moving.release());
			tmp.swap(*this);
		}

		template<typename U>
		_FORCE_INLINE_ Owned& operator=(Owned<U>&& moving) {
			Owned<T> tmp(moving.release());
			tmp.swap(*this);
			return *this;
		}

		_FORCE_INLINE_ Owned& operator=(std::nullptr_t) {
			reset();
			return *this;
		}

		// Const correct access owned object
		T* operator->() const {
			return m_Ptr;
		}

		T& operator*() const {
			return *m_Ptr;
		}

		// Access to smart pointer state
		T* get() const {
			return m_Ptr;
		}

		explicit operator bool() const {
			return m_Ptr;
		}

		// Modify object state
		_FORCE_INLINE_ T* release() {
			T* result = nullptr;
			std::swap(result, m_Ptr);
			return result;
		}

		_FORCE_INLINE_ void reset() {
			T* tmp = release();
			delete tmp;
		}

		_FORCE_INLINE_ void swap(Owned& src) noexcept {
			std::swap(m_Ptr, src.m_Ptr);
		}

	private:
		T* m_Ptr = nullptr;
	};

	template<typename T>
	void swap(Owned<T>& lhs, Owned<T>& rhs) noexcept {
		lhs.swap(rhs);
	}

#define CUSTOM_SMART_PTR
#ifdef CUSTOM_SMART_PTR

	template<class T>
	using Ref = Reference<T>;

	template<typename T, typename... Args>
	Ref<T> CreateRef(Args&&... args) {
		auto ptr = new T(std::forward<Args>(args)...);

		return Reference<T>(ptr);
	}

	template<class T>
	using UniqueRef = Owned<T>;

	template<typename T, typename... Args>
	UniqueRef<T> CreateUniqueRef(Args&&... args) {
		auto ptr = new T(std::forward<Args>(args)...);
		return Owned<T>(ptr);
	}

	template<class T>
	using WeakRef = WeakReference<T>;
#else
	template<class T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	Ref<T> CreateRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<class T>
	using WeakRef = std::weak_ptr<T>;

	template<class T>
	using UniqueRef = std::unique_ptr<T>;

	template<typename T, typename... Args>
	UniqueRef<T> CreateUniqueRef(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

#endif //CUSTOM_SMART_PTR
}

#ifdef CUSTOM_SMART_PTR
namespace std
{
	template<typename T>
	struct hash<Easel::Reference<T>> {
		size_t operator()(const Easel::Reference<T>& x) const {
			return hash<T*>()(x.get());
		}
	};
}
#endif //CUSTOM_SMART_PTR
