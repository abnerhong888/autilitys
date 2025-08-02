#ifndef __AUTILITY_FILESYSTEM_H__
#define __AUTILITY_FILESYSTEM_H__

#include "au_common.h"

#ifdef USING_AU_FILE_SYSTEM

#if _MSC_VER
#	ifdef AU_VS_2019 
#		ifdef __cpp_lib_filesystem // c++17
#			include <filesystem>
			namespace stdfs = std::filesystem;
#		elif __cpp_lib_experimental_filesystem // c++14
#			define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#			include <experimental/filesystem>
			namespace stdfs = std::experimental::filesystem;
#		else
#			error "no filesystem support"
#		endif
#   elif AU_VS_2017

#   elif AU_VS_2015
#		include <experimental/filesystem>
		namespace stdfs = std::experimental::filesystem;
#	endif
#endif // _MSC_VER

#if defined(__GNUC__)
#	include <experimental/filesystem>
	namespace stdfs = std::experimental::filesystem;
#endif //__GNUC__


#include <vector>
#include <string>
#include <chrono>
#include <regex>
#include <functional>

namespace au {
	namespace fs {
		
		using system_time_type = std::chrono::system_clock::time_point;

		typedef struct file_info
		{
			std::string fullpath;
			std::string path;
			std::string name;
			std::string extension;
			std::string fullname;
			system_time_type last_write_time;

			file_info() = default;
		}file_info;

		
		AU_INLINE static bool is_directory(std::string path) {
			return stdfs::is_directory(path);
		}

		AU_INLINE static bool is_file_exist(std::string path) {
			return stdfs::exists(path);
		}

		AU_INLINE static bool create_dir(std::string path) {
			return stdfs::create_directory(path);
		}


		AU_INLINE static bool create_dir_recursive(std::string _path) {

			std::string path = std::regex_replace(_path, std::regex("\\\\"), "/");
			path = std::regex_replace(path, std::regex("/{2,20}"), "/");
			path = std::regex_replace(path, std::regex("\\./"), "");
			
			size_t pos;
			
			if (path.find_last_of("/") != path.size() - 1)
				path += "/";

			std::string _sub = "";
			bool r = true;
			while ((pos = path.find_first_of("/")) != std::string::npos)
			{
				_sub += path.substr(0, pos + 1);
				if(!stdfs::is_directory(_sub))
					r &= stdfs::create_directory(_sub);

				path = path.substr(pos + 1, path.size());
			}
			return r;
		}

		AU_INLINE static int remove_dir(std::string path) {
			// return N file or directories has been delete recursively
			return static_cast<int>(stdfs::remove_all(path));
		}

		AU_INLINE static system_time_type file_last_write_date(std::string path) {
			return stdfs::last_write_time(path);
		}

		AU_INLINE static int remove_file(std::string path) {
				return stdfs::remove(path);
		}

		AU_INLINE static void remove_files(std::vector<file_info> files) {
			for (file_info file : files)
				remove_file(file.fullpath);
		}

		template<typename T>
		AU_INLINE static int file_count(T path) {
			return (int)std::distance(stdfs::directory_iterator(path), stdfs::directory_iterator{});
		}

		AU_INLINE static std::vector<file_info> list_files(std::string path, std::string ext_filter = "") {

			if (!is_directory(path)) return {};
			std::vector<file_info> files;
			for (const auto& entry : stdfs::directory_iterator(path))
			{
				if (ext_filter != "")
				{
					if (ext_filter != entry.path().extension().string())
						continue;
				}

				file_info info;

				info.fullpath = entry.path().string();
				info.path = info.fullpath.substr(0, info.fullpath.find_last_of("\\") + 1);
				info.fullname = entry.path().filename().string();
				info.name = info.fullname.substr(0, info.fullname.find_last_of("."));
				info.extension = entry.path().extension().string();

				info.last_write_time = file_last_write_date(info.fullpath);

				files.emplace_back(info);
			}
			return files;
		}

		//template<typename _Lambda>
		AU_INLINE static std::vector<file_info> list_files(std::string path, std::function<bool(file_info data)> _lambda_filter) {

			if (!is_directory(path)) return {};

			std::vector<file_info> files;
			for (const auto& entry : stdfs::directory_iterator(path))
			{
				file_info info;

				info.fullpath = entry.path().string();
				info.path = info.fullpath.substr(0, info.fullpath.find_last_of("\\") + 1);
				info.fullname = entry.path().filename().string();
				info.name = info.fullname.substr(0, info.fullname.find_last_of("."));
				info.extension = entry.path().extension().string();

				if (!_lambda_filter(info)) continue;

				info.last_write_time = file_last_write_date(info.fullpath);

				files.emplace_back(info);
			}
			return files;
		}

		AU_INLINE static void sort_by_date(std::vector<file_info> &file_infos){
			std::sort(file_infos.begin(), file_infos.end(), [](file_info a, file_info b) -> bool
				{
					long long atime = system_time_type::clock::to_time_t(a.last_write_time);
					long long btime = system_time_type::clock::to_time_t(b.last_write_time);
					return atime < btime;
				});
		}

		AU_INLINE static std::vector<file_info> check_over_days(std::vector<file_info>& file_infos, int days){
			system_time_type _now = std::chrono::system_clock::now();
			time_t t_now = std::chrono::system_clock::to_time_t(_now);
			std::tm timeinfo_now;
			memcpy(&timeinfo_now, std::localtime(&t_now), sizeof(std::tm));

			std::vector<file_info> find;
			
			for (auto file : file_infos)
			{
				time_t t = std::chrono::system_clock::to_time_t(file.last_write_time);
				std::tm* timeinfo;
				timeinfo = std::localtime(&t);

				int _days_diff = (timeinfo_now.tm_mon - timeinfo->tm_mon) * 30 + (timeinfo_now.tm_mday - timeinfo->tm_mday);
				if (_days_diff >= days)
					find.emplace_back(file);

			}
			return find;
		}
	}
}

#endif //USING_AU_FILE_SYSTEM

#endif // __AUTILITY_FILESYSTEM_H__
