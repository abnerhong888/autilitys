#ifndef __AUTILITY_PTR_H__
#define __AUTILITY_PTR_H__

#include "au_common.h"

#ifdef USE_AU_PTR

#include <memory>


namespace au {
	namespace aptr {
		
#if 0
		template<typename T>
		using ptr = std::shared_ptr<T>;

		template<typename T>
		AU_INLINE static ptr<T> make_ptr(T* v)
		{
			return std::shared_ptr<T>(v);
		}
#endif

		/*
		* make_unique<Vec3>():      { x=0, y=0, z=0 }
		* make_unique<Vec3>(0,1,2): { x=0, y=1, z=2 }
		* make_unique<Vec3[]>(5):   5x { x=0, y=0, z=0 }
		*/
		template<typename T>
		using uptr = std::unique_ptr<T>;


		template<typename T>
		struct Deleter {
			virtual void operator() (T* p);
		};

		template<typename T>
		struct ptr : public std::shared_ptr<T>
		{
			AU_INLINE ptr() noexcept : std::shared_ptr<T>() {}
			AU_INLINE ptr(std::nullptr_t) noexcept : std::shared_ptr<T>(nullptr) {}

			AU_INLINE ptr(const ptr<T>& p) noexcept : std::shared_ptr<T>(p) {}

			AU_INLINE ptr(std::shared_ptr<T>& p) noexcept : std::shared_ptr<T>(p) {}

			AU_INLINE ptr(std::shared_ptr<T>&& p) noexcept : std::shared_ptr<T>(std::move(p)) {}

			template<typename Y>
			AU_INLINE ptr(Y& p) noexcept : std::shared_ptr<T>(p) {}

			template<typename Y, typename Delete>
			AU_INLINE ptr(Y& p, Delete d) noexcept : std::shared_ptr<T>(p, d) {}

			template<typename Y, typename Delete>
			AU_INLINE ptr(Y&& p, Delete d) noexcept : std::shared_ptr<T>(std::move(p), d) {}


			template<typename Y>
			AU_INLINE ptr(Y&& p) noexcept : std::shared_ptr<T>(std::move(p)) {}

			template<typename Y>
			AU_INLINE ptr(ptr<Y>& p) noexcept : std::shared_ptr<T>(p) {}

			template<typename Y> 
			AU_INLINE ptr(ptr<Y>&& p) noexcept : std::shared_ptr<T>(std::move(p)) {}

			template<typename Y>
			AU_INLINE ptr(std::shared_ptr<Y> &p) noexcept : std::shared_ptr<Y>(p) {}

			template<typename Y>
			AU_INLINE ptr(std::shared_ptr<Y>&& p) noexcept : std::shared_ptr<Y>(std::move(p)) {}
			
			template<typename Y>
			AU_INLINE ptr<Y> staticCast() const noexcept { return std::static_pointer_cast<Y>(*this); }

			template<typename Y>
			AU_INLINE ptr<Y> constCast() const noexcept { return std::const_pointer_cast<Y>(*this); }

			template<typename Y> 
			AU_INLINE ptr<Y> dynamicCast() const noexcept { return std::dynamic_pointer_cast<Y>(*this); }

		};

		template<typename T>
		AU_INLINE static ptr<T> make_ptr(T&& v)
		{ 
			return std::make_shared<T>(std::move(v));
		}

		template<typename T>
		AU_INLINE static ptr<T> make_ptr(T& v)
		{
			return std::make_shared<T>(v);
		}

		template<typename T>
		AU_INLINE static ptr<T> make_ptr(T* v)
		{
			return std::shared_ptr<T>(v);
		}

		template<typename T>
		AU_INLINE static void safe_release(T* v)
		{
			SAFE_RELEASE(v);
		}

		template<typename T>
		AU_INLINE static ptr<T> make_ptr_release(T* v)
		{
			return std::shared_ptr<T>(v, safe_release<T>);
		}
	}
}

#endif //USE_AU_PTR

#endif //__AUTILITY_PTR_H__



