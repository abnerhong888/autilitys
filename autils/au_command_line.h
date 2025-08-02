#ifndef __AUTILITY_COMMAND_LINE_H__
#define __AUTILITY_COMMAND_LINE_H__

#include "au_common.h"

#ifdef USE_AU_COMMAND_LINE

#ifdef USE_AU_EXCEPTION
#include "au_exception.h"
#endif

#include <string>
#include <regex>
#include <unordered_map>

namespace au {
	namespace cmdline {
		namespace types_ {
			using au_cl_int = int;
			using au_cl_double = double;
			using au_cl_bool = bool;
			using au_cl_string = std::string;

			using au_cl_info = bool;
		}
	}
}
namespace au {
	namespace cmdline {
		namespace global {

			enum class etypes : int {
				_int,
				_double,
				_bool,
				_string,

				_info,
				_total,
			};

			static const std::array<std::string, (size_t)etypes::_total> type_string =
			{ "int", "double", "bool", "string", "info" };
			static int _cl_index = 0;

			class cla_types_base {
			public:
				etypes type = etypes::_int;
				int index = 0;
				cla_types_base() = default;
				cla_types_base(etypes _type) : type(_type){
					index = _cl_index;
					_cl_index++;
				}
			};

			template<typename T>
			class cla_types : public cla_types_base
			{
			public:
				std::string name;
				std::string message;
				T default_value = T();
				T* global_ptr = nullptr;
				cla_types() {};
				cla_types(etypes _type, std::string _name, std::string _message, T _default_value, T* _global_ptr) :
					cla_types_base(_type), name(_name), message(_message), default_value(_default_value), global_ptr(_global_ptr)
				{
				}
			};

			using _int_t = types_::au_cl_int;
			using _double_t = types_::au_cl_double;
			using _bool_t = types_::au_cl_bool;
			using _string_t = types_::au_cl_string;

			using _info_t = types_::au_cl_info;

			using cla_int_t = cla_types<_int_t>;
			using cla_double_t = cla_types<_double_t>;
			using cla_bool_t = cla_types<_bool_t>;
			using cla_string_t = cla_types<_string_t>;

			using cla_info_t = cla_types<_info_t>;

			std::unordered_map<std::string, cla_types_base*> type_map;

			template<typename T>
			class map_op{
			public:
				map_op(std::string _name, std::string _help, T _value, T* _global_ptr);
			};

			template<> map_op<_int_t>::map_op(std::string _name, std::string _help, _int_t _value, _int_t* _global_ptr) {
				type_map[_name] = new cla_int_t(etypes::_int, _name, _help, _value, _global_ptr);
			}
			template<> map_op<_double_t>::map_op(std::string _name, std::string _help, _double_t _value, _double_t* _global_ptr) {
				type_map[_name] = new cla_double_t(etypes::_double, _name, _help, _value, _global_ptr);
			}
			template<> map_op<_bool_t>::map_op(std::string _name, std::string _help, _bool_t _value, _bool_t* _global_ptr) {
				type_map[_name] = new cla_bool_t(etypes::_bool, _name, _help, _value, _global_ptr);
			}
			template<> map_op<_string_t>::map_op(std::string _name, std::string _help, _string_t _value, _string_t* _global_ptr) {
				type_map[_name] = new cla_string_t(etypes::_string, _name, _help, _value, _global_ptr);
			}

			class map_op_info {
			public:
				map_op_info(std::string _name, std::string _help, _info_t _value, _info_t* _global_ptr) {
					type_map[_name] = new cla_info_t(etypes::_info, _name, _help, _value, _global_ptr);
					type_map[_name]->index = -1;
				}
			};

			AU_INLINE static void UpdateGlobalValue(std::string _name, std::string _value_s) {
				cla_types_base *_base = type_map[_name];
				switch (_base->type)
				{
					case etypes::_int: 
					{
						if (_value_s == "") return;
						cla_int_t* v = (cla_int_t*)_base;
						AU_TRY{
							*(v->global_ptr) = std::stoi(_value_s);
						}
						AU_CATCH{
							std::string str = "input command int value is unavailable. ";
							str += "name: " + _name + ", error value: " + _value_s;
							AU_THROW_ERR(str, au::exception::etype::UNAVAILABLE_VALUE);
						}
					}break;
					case etypes::_double:
					{
						if (_value_s == "") return;
						cla_double_t* v = (cla_double_t*)_base;
						AU_TRY{
							*(v->global_ptr) = std::stod(_value_s.c_str());
						}
						AU_CATCH{
							std::string str = "input command double value is unavailable. ";
							str += "name: " + _name + ", error value: " + _value_s;
							AU_THROW_ERR(str, au::exception::etype::UNAVAILABLE_VALUE);
						}
						
					}break;
					case etypes::_bool:
					{
						if (_value_s == "") return;
						cla_bool_t* v = (cla_bool_t*)_base;
						std::transform(_value_s.begin(), _value_s.end(), _value_s.begin(), [](char c) { return std::tolower(c); });
						if (_value_s == "true" || _value_s == "1" || _value_s == "on") {
							*(v->global_ptr) = true;
						}
						else if (_value_s == "false" || _value_s == "0" || _value_s == "off") {
							*(v->global_ptr) = false;
						}
						else {
							std::string str = "input command bool value is unavailable. format[true, 1, on, false, 0, off] ";
							str += "name: " + _name + ", error value: " + _value_s;
							AU_THROW_ERR(str, au::exception::etype::UNAVAILABLE_VALUE);
						}
						
					}break;
					case etypes::_string:
					{
						cla_string_t* v = (cla_string_t*)_base;
						*(v->global_ptr) = _value_s;
					}break;
					case etypes::_info:
					{
						cla_info_t* v = (cla_info_t*)_base;
						*(v->global_ptr) = true;
					}break;
				default:
					break;
				}
			}
			
			template<typename T>
			AU_INLINE static std::string generateCommandHelp(T v);

			template<> std::string generateCommandHelp(cla_int_t *v) {
				return " [ -" + v->name + " | --" + v->name + " ]:\n\t" +
					"type    -> " + type_string[(size_t)v->type] + "\n\t" +
					"default -> " + std::to_string(v->default_value) + "\n\t" +
					"usage   -> " + v->message + "\n\n";
			}
			template<> std::string generateCommandHelp(cla_double_t* v) {
				return " [ -" + v->name + " | --" + v->name + " ]:\n\t" +
					"type    -> " + type_string[(size_t)v->type] + "\n\t" +
					"default -> " + std::to_string(v->default_value) + "\n\t" +
					"usage   -> " + v->message + "\n\n";
			}
			template<> std::string generateCommandHelp(cla_bool_t* v) {
				return " [ -" + v->name + " | --" + v->name + " ]:\n\t" +
					"type    -> " + type_string[(size_t)v->type] + "\n\t" +
					"default -> " + std::to_string(v->default_value) + "\n\t" +
					"usage   -> " + v->message + "\n\n";
			}
			template<> std::string generateCommandHelp(cla_string_t* v) {
				return " [ -" + v->name + " | --" + v->name + " ]:\n\t" +
					"type    -> " + type_string[(size_t)v->type] + "\n\t" +
					"default -> " + (v->default_value) + "\n\t" +
					"usage   -> " + v->message + "\n\n";
			}
		}
	}
}
// only support 4 type, [ int | double | bool | string ]
#define AU_DEFINE_CMD(_type, _name, _default, _message)					\
namespace au {															\
	namespace cmdline {													\
		namespace cl_##_type {											\
			global::_##_type##_t CMD_##_name = _default;				\
			global::map_op<global::_##_type##_t>						\
			temp_##_name(#_name, _message, _default, &CMD_##_name);		\
		}}}																\
using au::cmdline::cl_##_type::CMD_##_name;								\

#define AU_DEFINE_CMD_INFO(_name, _message)								\
namespace au {															\
	namespace cmdline {													\
		namespace cl_info {												\
			global::_info_t CMD_INFO_##_name = false;					\
			global::map_op_info											\
			temp_##_name(#_name, _message, false, &CMD_INFO_##_name);	\
		}}}																\
using au::cmdline::cl_info::CMD_INFO_##_name;							\

namespace au {
	namespace cmdline {

		AU_INLINE static void Parser(int argc, char* argv[]) {
			for (int i = 0;i < argc;i++) {
				// check format
				if (std::regex_match(argv[i], std::regex("-.*|--.*"))) {
					// replace
					std::string str = std::regex_replace(argv[i], std::regex("-"), "");
					// split
					std::regex regexz(" ");
					std::sregex_token_iterator start(str.begin(), str.end(), regexz, -1);
					std::sregex_token_iterator end;
					std::vector<std::string> v = { start, end };

					if (global::type_map[v[0]]->type == global::etypes::_string) {
						global::UpdateGlobalValue(v[0], str.substr(v[0].size() + 1, str.size()));
					}
					else if (global::type_map[v[0]]->type == global::etypes::_info) {
						global::UpdateGlobalValue(v[0], "true");
					}
					else {
						if (v.size() == 1) {
							v.push_back("");
						}
						else if (v.size() > 2) {
							std::string str = "input command is unavailable. ";
							str += "error command: " + std::string(argv[i]);
							AU_THROW_ERR(str, au::exception::etype::UNAVAILABLE_VALUE);
						}
						global::UpdateGlobalValue(v[0], v[1]);
					}
				}
					
			}
		}

		// Generate help string in AU_DEFINE_CMD order
		AU_INLINE static std::string GenerateHelpString() {
			std::string result;
			std::vector<global::cla_types_base*> bases;

			for (auto pair : global::type_map) {
				bases.emplace_back(pair.second);
			}
			std::sort(bases.begin(), bases.end(), [](global::cla_types_base* a, global::cla_types_base* b)
				{
					return a->index < b->index;
				});

				
			for (auto base : bases) {
				switch (base->type) {
					case global::etypes::_int: 
						result += global::generateCommandHelp((global::cla_int_t*)base);
					break;
					case global::etypes::_double: 
						result += global::generateCommandHelp((global::cla_double_t*)base);
					break;
					case global::etypes::_bool: 
						result += global::generateCommandHelp((global::cla_bool_t*)base);
					break;
					case global::etypes::_string: 
						result += global::generateCommandHelp((global::cla_string_t*)base);
					break;
					case global::etypes::_info:
					{
						global::cla_info_t* v = (global::cla_info_t*)base;
						result += " [ -" + v->name + " | --" + v->name + " ]:\n\t" + "usage   -> " + v->message + "\n\n";
					}break;
					default:
						break;
				}
			}

			return result;
		}

		AU_INLINE static int getDefault_int(std::string _name) {
			global::cla_types_base* _base = global::type_map[_name];
			global::cla_int_t* v = (global::cla_int_t*)_base;
			return v->default_value;
		}

		AU_INLINE static double getDefault_double(std::string _name) {
			global::cla_types_base* _base = global::type_map[_name];
			global::cla_double_t* v = (global::cla_double_t*)_base;
			return v->default_value;
		}

		AU_INLINE static bool getDefault_bool(std::string _name) {
			global::cla_types_base* _base = global::type_map[_name];
			global::cla_bool_t* v = (global::cla_bool_t*)_base;
			return v->default_value;
		}

		AU_INLINE static std::string getDefault_string(std::string _name) {
			global::cla_types_base* _base = global::type_map[_name];
			global::cla_string_t* v = (global::cla_string_t*)_base;
			return v->default_value;
		}
	}
}

#endif //USE_AU_COMMAND_LINE

#endif //__AUTILITY_COMMAND_LINE_H__