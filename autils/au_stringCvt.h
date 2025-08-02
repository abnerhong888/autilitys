#ifndef __AUTILITY_STRING_CVT_H__
#define __AUTILITY_STRING_CVT_H__
#include "au_common.h"

#ifdef USE_AU_STRING_CVT

#include <string>
#include <algorithm>
#include <cctype>
#include <regex>

namespace au {
	namespace stringcvt{

		AU_INLINE static std::wstring to_wide(std::string s) { return std::wstring(s.begin(), s.end()); }
		AU_INLINE static std::string to_string(std::wstring ws) { return std::string(ws.begin(), ws.end()); }

		AU_INLINE static std::string to_upper(std::string &&s){
			std::string dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](char c) { return std::toupper(c); });
			return dst;
		}
		AU_INLINE static std::string to_upper(std::string& s) {
			std::string dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](char c) { return std::toupper(c); });
			return dst;
		}


		AU_INLINE static std::wstring to_upper(std::wstring&& s) {
			std::wstring dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](wchar_t c) { return std::toupper(c); });
			return dst;
		}
		AU_INLINE static std::wstring to_upper(std::wstring& s) {
			std::wstring dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](wchar_t c) { return std::toupper(c); });
			return dst;
		}

		AU_INLINE static std::string to_lower(std::string&& s) {
			std::string dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](char c) { return std::tolower(c); });
			return dst;
		}
		AU_INLINE static std::string to_lower(std::string& s) {
			std::string dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](char c) { return std::tolower(c); });
			return dst;
		}

		AU_INLINE static std::wstring to_lower(std::wstring&& s) {
			std::wstring dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](wchar_t c) { return std::tolower(c); });
			return dst;
		}
		AU_INLINE static std::wstring to_lower(std::wstring& s) {
			std::wstring dst;
			dst.resize(s.size());
			std::transform(s.begin(), s.end(), dst.begin(), [](wchar_t c) { return std::tolower(c); });
			return dst;
		}


		AU_INLINE static std::vector<std::string> split(std::string &&s, std::string delimiter) {
			std::regex regexz(delimiter);
			std::sregex_token_iterator start(s.begin(), s.end(), regexz, -1);
			std::sregex_token_iterator end;
			return { start, end }; // std::string(start, end);
		}
		AU_INLINE static std::vector<std::string> split(std::string& s, std::string delimiter) {
			std::regex regexz(delimiter);
			std::sregex_token_iterator start(s.begin(), s.end(), regexz, -1);
			std::sregex_token_iterator end;
			return { start, end }; // std::string(start, end);
		}

		AU_INLINE static std::vector<std::wstring> split(std::wstring&& ws, std::wstring delimiter) {
			std::wregex regexz(delimiter);
			std::wsregex_token_iterator start(ws.begin(), ws.end(), regexz, -1);
			std::wsregex_token_iterator end;
			return { start, end };
		}
		AU_INLINE static std::vector<std::wstring> split(std::wstring& ws, std::wstring delimiter) {
			std::wregex regexz(delimiter);
			std::wsregex_token_iterator start(ws.begin(), ws.end(), regexz, -1);
			std::wsregex_token_iterator end;
			return { start, end };
		}

		// pattern example "a|b|c", will replace a,b,c to target
		AU_INLINE static std::string replace(std::string&& s, std::string pattern, std::string target) {
			return std::regex_replace(std::move(s), std::regex(pattern), target);
		}
		AU_INLINE static std::string replace(std::string& s, std::string pattern, std::string target) {
			return std::regex_replace(s, std::regex(pattern), target);
		}

		// pattern example L"a|b|c", will replace a,b,c to target
		AU_INLINE static std::wstring replace(std::wstring&& ws, std::wstring pattern, std::wstring target) {
			return std::regex_replace(std::move(ws), std::wregex(pattern), target);
		}
		AU_INLINE static std::wstring replace(std::wstring& ws, std::wstring pattern, std::wstring target) {
			return std::regex_replace(ws, std::wregex(pattern), target);
		}

	}
}

#endif //USE_AU_STRING_CVT

#endif //__AUTILITY_STRING_CVT_H__

