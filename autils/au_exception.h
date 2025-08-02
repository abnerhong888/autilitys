#ifndef __AUTILITY_EXCEPTION_H__
#define __AUTILITY_EXCEPTION_H__

#include "au_common.h"

#ifdef USING_AU_EXCEPTION

#include <string>
#include <stdexcept>

namespace au {
	namespace exception {

		enum class etype: int
		{
			UNKNOWN = 0,
			DIVIDE_BY_ZERO,
			NULL_POINTER,
			KEY_EXIST,
			KEY_NO_EXIST,
			UNAVAILABLE_VALUE,
			ASSERT,
			total
		}excep_type;
	}
}
namespace au{
	namespace exception{

		static const std::array<const char*, (size_t)etype::total> error_type_stirngs =
		{ 
			"UNKNOWN", 
			"DIVIDE_BY_ZERO", 
			"NULL_POINTER",
			"KEY_EXIST",
			"KEY_NO_EXIST",
			"UNAVAILABLE_VALUE",
			"ASSERT"
		};

		typedef struct _my_error
		{
			std::string msg;
			std::string func;
			std::string line;
			std::string file;
			etype type;

			_my_error() : msg(""), func(""), line(""), file(""), type(etype::UNKNOWN) {}

			AU_INLINE explicit _my_error(
				std::string _what,
				const char* _func, 
				long		_line,
				const char* _file,
				const etype _type) 
				: msg(""), func(_func), line(std::to_string(_line)), file(_file), type(_type)
			{
				if(type != etype::ASSERT)
					msg = "\n!!!======== exception ========!!!";

				msg +=
					"\n message: " + _what +
					"\n func: " + func +
					"\n line: " + line +
					"\n file: " + file +
					"\n type: " + error_type_stirngs[static_cast<size_t>(type)] + "\n\n";
					
			}
			AU_INLINE explicit _my_error(std::string _what):
				msg(""), func(""), line(""), file(""), type(etype::UNKNOWN)
			{
				msg = "\n!!!======== exception ========!!!";

				msg +=
					"\n message : " + _what +
					"\n func: " + func +
					"\n line: " + line +
					"\n file: " + file +
					"\n type: " + error_type_stirngs[static_cast<size_t>(type)] + "\n\n";
			}
			AU_INLINE std::string get_error_msg() const { return msg; }
			AU_INLINE std::string get_error_func() const { return func; }
			AU_INLINE std::string get_error_line() const { return line; }
			AU_INLINE std::string get_error_file() const { return file; }
			AU_INLINE etype get_error_type() const { return type; }
		}error;


		class Exception : public std::runtime_error
		{
		private:
			error err;
		public:
			explicit Exception(error &_err) : runtime_error(_err.get_error_msg())
			{
				err = _err;
			};
			AU_INLINE std::string get_error_msg() const { return err.get_error_msg(); }
			AU_INLINE std::string get_error_func() const { return err.get_error_func(); }
			AU_INLINE std::string get_error_line() const { return err.get_error_line(); }
			AU_INLINE std::string get_error_file() const { return err.get_error_file(); }
			AU_INLINE etype get_error_type() const { return err.get_error_type(); }

			static void errorProc(error &err);
			static void assertThrow(const char* condition, error err);
			static void exceptThrow(error err);
		};

#define ERROR_CLASS(name, base) \
		class name : public base\
		{\
		public:\
			explicit name(error &e) noexcept : base(e) {}\
		}

		ERROR_CLASS(divide_by_zero, Exception);
		ERROR_CLASS(null_pointer, Exception);
		ERROR_CLASS(key_exist, Exception);
		ERROR_CLASS(key_no_exist, Exception);
		ERROR_CLASS(unavailable_value, Exception);
		ERROR_CLASS(assert, Exception);

#undef ERROR_CLASS

		AU_INLINE void Exception::errorProc(error &err)
		{
			switch (err.get_error_type())
			{
			case etype::DIVIDE_BY_ZERO:
				throw divide_by_zero(err);
			case etype::NULL_POINTER:
				throw null_pointer(err);
			case etype::KEY_EXIST:
				throw key_exist(err);
			case etype::KEY_NO_EXIST:
				throw key_no_exist(err);
			case etype::UNAVAILABLE_VALUE:
				throw unavailable_value(err);
			case etype::ASSERT:
				throw assert(err);

			case etype::UNKNOWN:
			default:
				throw Exception(err);
			}
		}

		AU_INLINE void Exception::assertThrow(const char* condition, error err)
		{
			std::cerr << "\n\n Assertion failed: (" << condition << "): " << err.get_error_msg() << "\n";
			Exception::errorProc(err);
		}
		AU_INLINE void Exception::exceptThrow(error err)
		{
			std::cerr << err.get_error_msg() << "\n";
			Exception::errorProc(err);
		}
	}
}

#endif //USING_AU_EXCEPTION

#endif //__AUTILITY_EXCEPTION_H__