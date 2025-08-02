#ifndef __AUTILITY_COMMON_H__
#define __AUTILITY_COMMON_H__

#if defined(_MSC_VER)
#   if _MSC_VER < 1900
#       error This project needs at least Visual Studio 2015.
#   elif _MSC_VER >= 1900 && _MSC_VER < 1910 // Visual Studio 2015
#       define AU_VS_2015
#   elif _MSC_VER >= 1910 && _MSC_VER < 1920 // Visual Studio 2017
#       define AU_VS_2017
#   elif _MSC_VER > 1920  // Visual Studio 2019
#       define AU_VS_2019
#   endif
#endif

#if !defined(__cplusplus)
#   error This project needs c++
#endif

#if defined(_MSC_VER)
#	if defined(AU_DLL)
#		ifdef AU_DLL_EXPORT
#			define AU_API __declspec(dllexport)
#		else
#			define AU_API __declspec(dllimport)
#		endif
#		define AU_INLINE
#	else
#		define AU_API
#		define AU_INLINE inline
#	endif //AU_DLL

#pragma warning(disable:4996)

#endif //_MSC_VER


#if defined(__GNUC__)
#	if defined(AU_DLL)
#		ifdef AU_DLL_EXPORT
#			define AU_API __attribute__((visibility("default")))
#		else
#			define AU_API 
#		endif
#		define AU_INLINE
#	else
#		define AU_API
#		define AU_INLINE inline
#	endif //AU_DLL
#endif //__GNUC__


#if defined(_MSC_VER)
#   define AU_FUNC __FUNCTION__
#   define AU_LINE __LINE__
#   define AU_FILE __FILE__
#elif defined(__GNUC__)
#   define AU_FUNC __FUNCTION__
#   define AU_LINE __LINE__
#   define AU_FILE __FILE__
#endif

#define AU_ENABLE_IF(is_xxx) typename = typename std::enable_if<is_xxx::value>::type
#define AU_ENABLE_IF_NOT(is_xxx) typename = typename std::enable_if<!is_xxx::value>::type

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; } 
#endif

#if !defined(_MSC_VER)
#   ifndef ARRAYSIZE
    template <typename T, size_t N>
    char(*RtlpNumberOf(T(&)[N]))[N];
#   define ARRAYSIZE(A) (sizeof(*RtlpNumberOf(A)))
#endif

#   ifndef _ARRAYSIZE
#   define _ARRAYSIZE(A) (sizeof(A) / sizeof(A[0]))
#   endif
#endif

namespace au {
	template<typename T, AU_ENABLE_IF(std::is_class<T>)>
	AU_INLINE static std::string to_string(const T& v) { return ((T)v).to_string(); }
	template<typename T, AU_ENABLE_IF(std::is_class<T>)>
	AU_INLINE static std::string to_string(const T* v) { return v->to_string(); }
	template<typename T, AU_ENABLE_IF(std::is_arithmetic<T>)> // int, float, double ...
	AU_INLINE static std::string to_string(const T v) { return std::to_string(v); }
	AU_INLINE static std::string to_string(std::chrono::system_clock::time_point v, std::string fmt = "") {
		time_t t = std::chrono::system_clock::to_time_t(v);
		std::tm* timeinfo;
		timeinfo = std::localtime(&t);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), fmt != "" ? fmt.c_str() : "%d-%m-%Y %H:%M:%S", timeinfo);
		return std::string(buffer);
	}
}

#include <chrono>
#include <thread>
namespace au {
    AU_INLINE static void sleep_ms(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
	AU_INLINE static void sleep_us(int us) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(us));
	}
}


#include "au_module_def.h"


#ifdef USING_AU_EXCEPTION
//#       define AU_ASSERT(condition, message)\
//        (!(condition)) ?\
//            (std::cerr << "Assertion failed: (" << #condition << "): "\
//	        << au::exception::error(message, AU_FUNC, AU_LINE, AU_FILE, au::exception::etype::ASSERT).get_error_msg()\
//            << "\n",abort(), 0) : 1

#       define AU_ASSERT(condition, message)\
        (!(condition)) ?\
            ( au::exception::Exception::assertThrow(#condition,\
			au::exception::error(message, AU_FUNC, AU_LINE, AU_FILE, au::exception::etype::ASSERT))\
			,0 ): 1
#else
#       define AU_ASSERT(condition, message)\
        (!(condition)) ?\
          (std::cerr << "Assertion failed: (" << #condition << "), "\
          << "function " << __FUNCTION__ \
          << ", file " << __FILE__\
          << ", line " << __LINE__ << "."\
          << "\t Message: " << message << "\n", abort(), 0) : 1
#endif

#define AU_STATIC_ASSERT(condition, message) static_assert(condition, "!!!!! assert: " #condition ", " message " !!!!!")
#define AU_TRY try
#define AU_CATCH catch(std::exception e)

#ifdef USING_AU_EXCEPTION
// _eType example = exception::etype::NULL_POINTER
#	define AU_THROW_ERR(_what, _eType) \
			do {\
				auto err = au::exception::error(_what, AU_FUNC, AU_LINE, AU_FILE, _eType); \
				au::exception::Exception::exceptThrow(err);\
			}while(0)
#else
#	define AU_THROW_ERR(_what, _eType)\
			do {\
				std::string str = "\n!!!======== exception ========!!!" "\n message: " + std::string(_what) + "\n func: " AU_FUNC "\n line: " + \
				au::to_string(AU_LINE) + "\n file: " AU_FILE "\n type: " #_eType "\n\n";\
				throw std::exception(str.c_str());\
			} while (0)
#endif

#endif //__AUTILITY_COMMON_H__