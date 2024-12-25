#ifndef __AUTILITY_INI_H__
#define __AUTILITY_INI_H__

#include "common.h"

#ifdef USING_AU_INI

#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <regex>

namespace au {
	namespace ini {
		class value_{
		public:
			std::string v;
			AU_INLINE value_() {}
			AU_INLINE value_(const std::string&_v):v(_v) {}

			AU_INLINE void operator= (const int& _v) {
				v = std::to_string(_v);
			}
			AU_INLINE void operator= (const long& _v) {
				v = std::to_string(_v);
			}
			AU_INLINE void operator= (const float& _v) {
				v = std::to_string(_v);
			}
			AU_INLINE void operator= (const double& _v) {
				v = std::to_string(_v);
			}
			AU_INLINE void operator= (const bool& _v) {
				v = _v == true ? "true" : "false";
			}
			AU_INLINE void operator= (const std::string& _v) {
				v = _v;
			}
			AU_INLINE void operator= (const char* _v) {
				v = std::string(_v);
			}

			template<typename T>
			T as(T _default = T()) {
				AU_STATIC_ASSERT(false, "Not support other types.");
			}

			template<typename T>
			std::vector<T> as_split(std::string delimiter) {
				AU_STATIC_ASSERT(false, "Not support other types.");
			}
		private:
			AU_INLINE std::vector<std::string> string_split(std::string delimiter) {
				std::regex regexz(delimiter);
				std::sregex_token_iterator start(v.begin(), v.end(), regexz, -1);
				std::sregex_token_iterator end;
				return { start, end };
			}
		};

		template<> AU_INLINE int value_::as(int _default) { 
			AU_TRY{ return std::stoi(v); } 
			AU_CATCH{ return _default; } 
		}
		template<> AU_INLINE long value_::as(long _default) {
			AU_TRY{ return static_cast<long>(std::stoi(v)); }
			AU_CATCH{ return _default; }
		}
		template<> AU_INLINE float value_::as(float _default) { 
			AU_TRY{ return std::stof(v); }
			AU_CATCH{ return _default; }
		}
		template<> AU_INLINE double value_::as(double _default) {
			AU_TRY{ return std::stod(v); }
			AU_CATCH{ return _default; }
		}
		template<> AU_INLINE bool value_::as(bool _default) {
			if (v == "true" || v == "TRUE" || v == "on" || v == "ON") {
				return true;
			}
			else if (v == "false" || v == "FALSE" || v == "off" || v == "OFF") {
				return false;
			}
			else {
				return _default;
			}
		}
		template<> AU_INLINE std::string value_::as(std::string _default) { 
			if (!v.empty())
				return v;
			else
				return _default;
		}

		template<> AU_INLINE std::vector<int> value_::as_split(std::string delimiter) {
			std::vector<std::string> s = string_split(delimiter);
			std::vector<int> r(s.size());
			std::transform(s.begin(), s.end(), r.begin(), [](std::string s) {
				AU_TRY{ return std::stoi(s); }
				AU_CATCH{ return int(); }
				});
			return r;
		}
		template<> AU_INLINE std::vector<long> value_::as_split(std::string delimiter) {
			std::vector<std::string> s = string_split(delimiter);
			std::vector<long> r(s.size());
			std::transform(s.begin(), s.end(), r.begin(), [](std::string s) {
				AU_TRY{ return static_cast<long>(std::stoi(s)); }
				AU_CATCH{ return long(); }
				});
			return r;
		}
		template<> AU_INLINE std::vector<float> value_::as_split(std::string delimiter) {
			std::vector<std::string> s = string_split(delimiter);
			std::vector<float> r(s.size());
			std::transform(s.begin(), s.end(), r.begin(), [](std::string s) {
				AU_TRY{ return std::stof(s); }
				AU_CATCH{ return float(); }
				});
			return r;
		}
		template<> AU_INLINE std::vector<double> value_::as_split(std::string delimiter) {
			std::vector<std::string> s = string_split(delimiter);
			std::vector<double> r(s.size());
			std::transform(s.begin(), s.end(), r.begin(), [](std::string s) {
				AU_TRY{ return std::stod(s); }
				AU_CATCH{ return double(); }
				});
			return r;
		}
		template<> AU_INLINE std::vector<std::string> value_::as_split(std::string delimiter) {
			return string_split(delimiter);
		}

		class IniKeyValue {
		public:
			std::unordered_map<std::string, size_t> _map;
			std::unordered_map<size_t, std::string> _idx_map;
			std::vector<value_> container;

			AU_INLINE value_& operator[] (std::string key) {
				if (_map.count(key) == 0) {
					size_t idx = container.size();
					_map[key] = idx;
					_idx_map[idx] = key;
					container.push_back(value_());
				}
				return container[_map[key]];
			}

			AU_INLINE bool HasKey(std::string key) {
				return _map.count(key) != 0;
			}

			AU_INLINE void DeleteKey(std::string key) {
				if (_map.count(key) == 0) return;

				std::unordered_map<std::string, size_t>::iterator map_it_find = _map.find(key);
				std::unordered_map<std::string, size_t>::iterator map_it = _map.begin();
				size_t delete_idx = map_it_find->second;

				container.erase(container.begin() + delete_idx);

				while (map_it != _map.end()) {
					if (map_it->second > delete_idx)
						map_it->second--;
					map_it++;
				}
				_map.erase(map_it_find);

				_idx_map.clear();

				map_it = _map.begin();
				while (map_it != _map.end()) {
					_idx_map[map_it->second] = map_it->first;
					map_it++;
				}

			}
		};

		class IniData {
		public:
			std::unordered_map<std::string, size_t> _map;
			std::unordered_map<size_t, std::string> _idx_map;
			std::vector<IniKeyValue> container;
			std::string path;

			AU_INLINE IniKeyValue& operator[] (std::string section) {
				return KeyValues(section);
			}
			AU_INLINE IniKeyValue& KeyValues(std::string section) {

				if (_map.count(section) == 0){
					size_t idx = container.size();
					_map[section] = idx;
					_idx_map[idx] = section;
					container.push_back(IniKeyValue());
				}
				return container[_map[section]];
			}

			AU_INLINE void DeleteSection(std::string section) {
				if (_map.count(section) == 0) return;

				std::unordered_map<std::string, size_t>::iterator map_it_find = _map.find(section);
				std::unordered_map<std::string, size_t>::iterator map_it = _map.begin();
				size_t delete_idx = map_it_find->second;

				container.erase(container.begin() + delete_idx);

				while (map_it != _map.end()) {
					if (map_it->second > delete_idx)
						map_it->second--;
					map_it++;
				}
				_map.erase(map_it_find);

				_idx_map.clear();

				map_it = _map.begin();
				while (map_it != _map.end()) {
					_idx_map[map_it->second] = map_it->first;
					map_it++;
				}

			}
			AU_INLINE void DeleteKey(std::string section, std::string key) {
				if (_map.count(section) != 0) {
					container[_map[section]].DeleteKey(key);
				}
			}

			AU_INLINE bool HasSection(std::string section) {
				return _map.count(section) != 0;
			}
			AU_INLINE bool HasKey(std::string section, std::string key) {
				if (_map.count(section) != 0) {
					return  container[_map[section]].HasKey(key);
				}
				else
					return false;
			}
			
			AU_INLINE void ParserLine(std::string line) {
				line = std::regex_replace(line, std::regex("\\s"), "");
				if (line.empty()) return;
				size_t pos = line.find_first_of(';');
				if (pos != std::string::npos)
					line = line.substr(0, pos);

				if (std::regex_match(line, std::regex("\\[.*\\]"))) {
					line = line.substr(1, line.length() - 2);
					parser_section_recorder = line;
					size_t idx = container.size();
					_map[line] = idx;
					_idx_map[idx] = line;
					container.push_back(IniKeyValue());
				}
				else if(std::regex_match(line, std::regex(".*=.*"))) {
					pos = line.find_first_of('=');
					std::string key = line.substr(0, pos);
					std::string val = line.substr(pos + 1, line.size() - 1);
					size_t idx = _map[parser_section_recorder];
					size_t key_idx = container[idx].container.size();

					container[idx]._map[key] = key_idx;
					container[idx]._idx_map[key_idx] = key;
					container[idx].container.push_back(value_());
					container[idx][key] = val;
				}
			}
		private:
			std::string parser_section_recorder;
		};

		class IniFile 
		{
		public:
			AU_INLINE static IniData Read(std::string _path) {
				IniData data;
				data.path = _path;

				std::fstream file;
				file.open(_path, std::ios::in | std::ios::app);
				std::string line;
				while (!file.eof())
				{
					std::getline(file, line);
					data.ParserLine(line);
				}

				file.close();
				return data;
			}

			AU_INLINE static void Write(IniData& _data) {
				std::fstream file;
				file.open(_data.path, std::ios::out | std::ios::trunc);

				for (int i = 0; i < _data.container.size(); i++) {
					file << "[" << _data._idx_map[i] << "]" << "\n";
					for (int j = 0; j < _data.container[i].container.size(); j++) {
						file << _data.container[i]._idx_map[j] << " = " << _data.container[i].container[j].as<std::string>() << " ;\n";
					}
					file << ";------------------------------------------------------------\n";
				}
				file.close();
			}
			AU_INLINE static void DirectChange(std::string _path, std::string _section, std::string _key, std::string _val) {
				std::fstream file;
				file.open(_path, std::ios::in | std::ios::app);
				std::string line;
				std::vector<std::string> list_line;
				while (!file.eof())
				{
					std::getline(file, line);
					list_line.emplace_back(line);
				}
				file.close();

				std::string match_section_text = "\\[" + _section + "\\]";
				std::string match_key_text = _key + "=.*";
				bool start = false;
				bool is_found = false;
				for (int i = 0; i < list_line.size(); i++) {
					line = std::regex_replace(list_line[i], std::regex("\\s"), "");

					if (!start) {
						if (std::regex_match(line, std::regex(match_section_text))) {
							start = true;
						}
						else
							continue;
					}
					else {
						if (std::regex_match(line, std::regex(match_key_text))) {
							list_line[i] = _key + " = " + _val + " ;";
							is_found = true;
							break;
						}
						else if (std::regex_match(line, std::regex("\\[.*\\]"))) {
							if (!is_found) {
								std::string str = _key + " = " + _val + " ;";
								list_line.insert(list_line.begin() + i, str);
								is_found = true;
							}
							break;
						}
					}
				}

				if (!is_found) {
					list_line.push_back("[" + _section + "]");
					list_line.push_back(_key + " = " + _val + " ;");
					list_line.push_back(";------------------------------------------------------------");
					is_found = true;
				}

				file.open(_path, std::ios::out | std::ios::trunc);
				for (auto line : list_line) {
					file << line << "\n";
				}
				file.close();
			}
		protected:

		private:
			
		};

	}
}


#endif //USING_AU_INI
#endif //__AUTILITY_INI_H__