#ifndef __AUTILITY_LOGGER_H__
#define __AUTILITY_LOGGER_H__

#include "au_common.h"

#ifdef USE_AU_LOGGER

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <unordered_map>
#include <regex>
#include <chrono>
#include <fstream>
#include <memory>

#if defined(_MSC_VER)
#include "windows.h"
#endif

#ifdef USE_AU_EXCEPTION
#include "au_exception.h"
#endif

#ifndef USE_AU_FILE_SYSTEM
#error Must define USE_AU_TYPES if using logger define.
#endif

#ifdef USE_AU_THREAD
#include "au_thread.h"
#endif

#include "au_filesystem.h"


namespace au {
	namespace log {
		enum class eLevel : int
		{
			info = 0,
			err,
			warn,
			debug,
			none,
			total,
		};

	}
}

namespace au {
	namespace log {
		namespace global
		{
			static const std::array<const char*, (size_t)eLevel::total> level_string =
			{ "[  INFO   ] ", "[  ERROR  ] ", "[ WARNING ] ", "[  DEBUG  ] ", "" };

			static const std::regex reformat(
				"\\{d\\}|\\{d[1-9]\\}|\\{f\\}|\\{f[1-9]\\}|\\{0x\\}|\\{c\\}|\\{s\\}|\\{\\}",
				std::regex_constants::icase
			);

			static const std::unordered_map<std::string, std::string> type_string =
			{
				{typeid(int).name(),				"%d"},
				{typeid(unsigned int).name(),		"%d"},
				{typeid(short int).name(),			"%d"},
				{typeid(unsigned short int).name(), "%d"},
				{typeid(long).name(),				"%d"},
				{typeid(long long).name(),			"%d"},
				{typeid(float).name(),				"%f"},
				{typeid(double).name(),				"%f"},
				{typeid(char).name(),				"%c"},
				{typeid(unsigned char).name(),		"%c"},
				{typeid(char*).name(),				"%s"},
				{typeid(const char*).name(),		"%s"},
				{typeid(bool).name(),				"%d"},
				{typeid(std::string).name(),		"%s"},
				{typeid(std::chrono::nanoseconds).name(),	"%d ns"},
				{typeid(std::chrono::microseconds).name(),	"%d us"},
				{typeid(std::chrono::milliseconds).name(),	"%d ms"},
				{typeid(std::chrono::seconds).name(),		"%d s"},
				{typeid(std::chrono::minutes).name(),		"%d min"},
				{typeid(std::chrono::hours).name(),			"%d hr"},

				{typeid(std::chrono::duration<double, std::chrono::nanoseconds::period>).name(),	"%f ns"},
				{typeid(std::chrono::duration<double, std::chrono::microseconds::period>).name(),	"%f us"},
				{typeid(std::chrono::duration<double, std::chrono::milliseconds::period>).name(),	"%f ms"},
				{typeid(std::chrono::duration<double, std::chrono::seconds::period>).name(),		"%f s"},
				{typeid(std::chrono::duration<double, std::chrono::minutes::period>).name(),		"%f min"},
				{typeid(std::chrono::duration<double, std::chrono::hours::period>).name(),			"%f hr"},
				
				{"{d}", "%d"},{"{d1}", "%1d"},{"{d2}", "%2d"},{"{d3}", "%3d"},{"{d4}", "%4d"},{"{d5}", "%5d"},
				{"{d6}", "%6d"},{"{d7}", "%7d"},{"{d8}", "%8d"},{"{d9}", "%9d"},

				{"{f}", "%f"},{"{f1}", "%.1f"},{"{f2}", "%.2f"},{"{f3}", "%.3f"},{"{f4}", "%.4f"},{"{f5}", "%.5f"},
				{"{f6}", "%.6f"},{"{f7}", "%.7f"},{"{f8}", "%.8f"},{"{f9}", "%.9f"},

				{"{0x}", "0x%x"},
				{"{c}", "%c"},
				{"{s}", "%s"},

			};

#define LOG_FOLDER  ".\\Log\\"
			static bool is_apply_to_log_console = false;
			static bool is_console_enable = true;

		}
	}
}

namespace au {
	namespace log {

		AU_INLINE std::ostream& operator<< (std::ostream& o, const eLevel& l)
		{
			o << global::level_string[static_cast<size_t>(l)];
			return o;
		}
	}
}

namespace au {
	namespace log {

		namespace helper {

			template<std::size_t I = 0, typename... Tp>
			AU_INLINE static typename std::enable_if<I == sizeof...(Tp), std::vector<std::string>>::type
				tuples_to_type_map(std::tuple<Tp...>& t)
			{
				return {};
			}

			template<std::size_t I = 0, typename... Tp>
			AU_INLINE static typename std::enable_if < I < sizeof...(Tp), std::vector<std::string>>::type
				tuples_to_type_map(std::tuple<Tp...>& t)
			{
				std::vector<std::string> type_map_value;
				const char* v = typeid(typename  std::decay< decltype(std::get<I>(t)) >::type ).name();

				AU_ASSERT(global::type_string.count(v) > 0,
					"Can not find type name in type_string, try to use au::to_string as Args.");

				type_map_value.emplace_back(global::type_string.at(v));

				std::vector<std::string>_sub = tuples_to_type_map<I + 1, Tp...>(t);
				if (_sub.size() > 0)
					type_map_value.insert(type_map_value.end(), _sub.begin(), _sub.end());

				return type_map_value;
			}

			template<typename... Args>
			AU_INLINE static std::vector<std::string> Args_to_tuples_types_string(Args &&...args) {
				auto tus = std::forward_as_tuple<Args...>(std::forward<Args>(args)...);
				return tuples_to_type_map(tus);
			}

			AU_INLINE static std::string regex_format_replace(std::string& fmt, std::vector<std::string> &tuple_types_string)
			{
				std::string _fmt = fmt;
				std::smatch sm;
				int count = 0;
				while (std::regex_search(_fmt, sm, global::reformat))
				{
					if (sm[0] == "{}")
					{
						if (count < tuple_types_string.size())
							_fmt.replace(sm.prefix().length(), sm.length(), tuple_types_string[count]);
						else
							_fmt.replace(sm.prefix().length(), sm.length(), "");
					}
					else
					{
						AU_ASSERT(global::type_string.count(sm[0]) > 0,
							"Can not find type name in type_string, try to use au::to_string as Args.");
						_fmt.replace(sm.prefix().length(), sm.length(), global::type_string.at(sm[0]));
					}

					count++;
				}
				return _fmt;
			}

			template<typename... Args>
			AU_INLINE static std::string to_format_string(std::string&& fmt, Args &&...args)
			{
				std::string _fmt = fmt;

				auto tuple_types_string = helper::Args_to_tuples_types_string(std::forward<Args>(args)...);
				return regex_format_replace(_fmt, tuple_types_string);

			}

			template<typename... Tp>
			AU_INLINE static std::string to_format_string_tuple(std::string&& fmt, std::tuple<Tp...> tus)
			{
				std::string _fmt = fmt;

				auto tuple_types_string = tuples_to_type_map(tus);
				return regex_format_replace(_fmt, tuple_types_string);
			}
		}

		class logger_thread;

		class AU_API logger
		{
		public:
			logger(std::string&& _name, int delete_file_over_day = -1){
				_initialize(std::move(_name), LOG_FOLDER);
				DeleteLogs(delete_file_over_day);
			}
			logger(std::string&& _name, std::string _folder_path, int delete_file_over_day = -1){
				_initialize(std::move(_name), _folder_path);
				DeleteLogs(delete_file_over_day);
			}
			virtual ~logger() {}

			template<typename... Args>
			void Write(eLevel lvl, std::string&& fmt, Args &&...args);

			template<typename... Args>
			void Write(eLevel lvl, std::string& fmt, Args &&...args);

			void DeleteLogs(int day) {
				if (day < 0) return;
				auto files = fs::list_files(folder_path,
					[&](fs::file_info info) -> bool {
						size_t pos;
						if (info.extension != ".txt") return false;
						int cnt = 0;
						while ((pos = info.name.find_last_of('_')) != std::string::npos) {
							info.name = info.name.substr(0, pos);
							cnt++;
							if (cnt == 6) break;
						}

						return info.name == name;
					}
				);

				auto files_overdays = fs::check_over_days(files, day);
				fs::remove_files(files_overdays);
			}
		protected:
			template<typename Ty>
			logger(std::string&& _name, Ty *v, int delete_file_over_day = -1) {
				_initialize(std::move(_name), LOG_FOLDER);
				is_logger_thread = std::is_same<logger_thread, Ty>::value;
				DeleteLogs(delete_file_over_day);
			}

			template<typename Ty>
			logger(std::string&& _name, std::string _folder_path, Ty* v, int delete_file_over_day = -1) {
				_initialize(std::move(_name), _folder_path);
				is_logger_thread = std::is_same<logger_thread, Ty>::value;
				DeleteLogs(delete_file_over_day);
			}

			void _initialize(std::string&& _name, std::string _folder_path) {
				name = std::move(_name);
				folder_path = _folder_path;
				path = folder_path + name + au::to_string(std::chrono::system_clock::now(), "_%Y_%m_%d_%H_%M_%S.txt");
				fs::create_dir_recursive(folder_path);
				file.open(path, std::ios::out | std::ios::trunc);
				file.close();
			}

			template<typename... Args>
			AU_INLINE void _Write_Buffer(eLevel level, std::string&& fmt, Args &&...args)
			{
				std::string str = au::to_string(std::chrono::system_clock::now()) + " ";
				str += std::move(global::level_string[static_cast<size_t>(level)]);
				str += helper::to_format_string(std::move(fmt), std::forward<Args>(args)...) + "\n";
				sprintf(text_buf, str.c_str(), args...);
			}
			template<typename... Args>
			AU_INLINE void _Write_Buffer(eLevel level, std::string& fmt, Args &&...args)
			{
				std::string str = au::to_string(std::chrono::system_clock::now()) + " ";
				str += std::move(global::level_string[static_cast<size_t>(level)]);
				str += helper::to_format_string(std::move(fmt), std::forward<Args>(args)...) + "\n";
				sprintf(text_buf, str.c_str(), args...);
			}

			AU_INLINE void _Write_File(const char* _buf) {
				file.open(path, std::ios::out | std::ios::app);
				file << _buf;
				file.close();
			}

			std::string name;
			std::string path;
			std::string folder_path;
			std::ofstream file;
			char text_buf[1024] = { 0 };
		private:
			

			bool is_logger_thread = false;
		}; // class logger

#ifdef USE_AU_THREAD

		enum class eMode : int
		{
			queue,
		};

		class AU_API logger_thread : public logger, thread::cppThread
		{
		public:
			logger_thread(std::string&& _name, eMode _mode, int delete_file_over_day = -1, int _queue_size = 50) :
				queue_size(_queue_size), mode(_mode), 
				logger(std::move(_name), this, delete_file_over_day), cppThread("logger_thread")
			{
				Start();
			}

			logger_thread(std::string&& _name, std::string _folder_path, eMode _mode, int delete_file_over_day = -1, int _queue_size = 50):
				queue_size(_queue_size), mode(_mode), 
				logger(std::move(_name), _folder_path, this, delete_file_over_day), cppThread("logger_thread")
			{
				Start();
			}

			~logger_thread() override {

				Flush();
				while (true) {
					this->ThreadLock();
					if (!force_flush)
						break;
					this->ThreadUnLock();

					au::sleep_ms(1); 
				}
				this->ThreadUnLock();

				this->Terminate();
			}

			template<typename... Args>
			AU_INLINE void Write(eLevel lvl, std::string&& fmt, Args &&...args){
				_Write_Buffer(lvl, std::move(fmt), std::forward<Args>(args)...);
				this->ThreadLock();
				string_queue.push(std::string(text_buf));
				this->ThreadUnLock();
			}

			template<typename... Args>
			AU_INLINE void Write(eLevel lvl, std::string& fmt, Args &&...args) {
				_Write_Buffer(lvl, fmt, std::forward<Args>(args)...);
				this->ThreadLock();
				string_queue.push(std::string(text_buf));
				this->ThreadUnLock();
			}

			void Flush() {
				this->ThreadLock();
				force_flush = true;
				this->ThreadUnLock();
			}
		protected:
			void Run() override
			{
				if (force_flush) {
					while (string_queue.size() > 0)
					{
						_Write_File(string_queue.front().c_str());
						string_queue.pop();
					}
					
					force_flush = false;
				}

				if (string_queue.size() > queue_size) {
					_Write_File(string_queue.front().c_str());
					string_queue.pop();
				}
			}
		private:
			int queue_size = 0;
			eMode mode;
			bool force_flush = false;
			std::queue<std::string> string_queue;
		};

#endif

		template<typename... Args>
		AU_INLINE void logger::Write(eLevel lvl, std::string&& fmt, Args &&...args) {
#ifdef USE_AU_THREAD
			if (is_logger_thread) {
				static_cast<logger_thread*>(this)->Write(lvl, std::move(fmt), std::forward<Args>(args)...);
			}
			else 
#endif
			{
				_Write_Buffer(lvl, std::move(fmt), std::forward<Args>(args)...);
				_Write_File(text_buf);
			}

		}

		template<typename... Args>
		AU_INLINE void logger::Write(eLevel lvl, std::string& fmt, Args &&...args) {
#ifdef USE_AU_THREAD
			if (is_logger_thread) {
				static_cast<logger_thread*>(this)->Write(lvl, fmt, std::forward<Args>(args)...);
			}
			else 
#endif
			{
				_Write_Buffer(lvl, fmt, std::forward<Args>(args)...);
				_Write_File(text_buf);
			}
		}

		class AU_API logger_map
		{
		public:
			using logger_sptr = std::shared_ptr<logger>;
			AU_INLINE static void assign(std::string key, logger* _logger){
				if(log_map.count(key) == 1)
					AU_THROW_ERR("Already have logger key in map.", au::exception::etype::KEY_EXIST);
				log_map.insert({ key, logger_sptr(_logger) });
			}
			AU_INLINE static logger_sptr get(std::string key) {
				if (log_map.count(key) != 1)
					AU_THROW_ERR("No logger key in map.", au::exception::etype::KEY_NO_EXIST);
				return log_map.at(key);
			}
		protected:

		private:
			static std::unordered_map<std::string, logger_sptr> log_map;
		}; // log_manager

		std::unordered_map<std::string, logger_map::logger_sptr> logger_map::log_map;

		class console_impl
		{
		public:

			template<typename... Args>
			AU_INLINE static void Write(eLevel level, std::string&& fmt, Args &&...args)
			{
				std::string str(std::move(global::level_string[static_cast<size_t>(level)]));
				str += helper::to_format_string(std::move(fmt), std::forward<Args>(args)...) + "\n";
				printf(str.c_str(), args...);
			}
			template<typename... Args>
			AU_INLINE static void Write(eLevel level, std::string& fmt, Args &&...args)
			{
				std::string str(std::move(global::level_string[static_cast<size_t>(level)]));
				str += helper::to_format_string(std::move(fmt), std::forward<Args>(args)...) + "\n";
				printf(str.c_str(), args...);
			}
			
		protected:

		private:
			
		}; // class console_impl

		class AU_API console
		{
		public:
			AU_INLINE static void console_enable(bool enable) {
				global::is_console_enable = enable;
			}
			AU_INLINE static void apply_console_to_log(bool enable) {
				global::is_apply_to_log_console = enable;
			}

			AU_INLINE static void Debug(std::string&& fmt) {
				console_impl::Write(eLevel::debug, std::move(fmt));
			}
			AU_INLINE static void Debug(std::string& fmt) {
				console_impl::Write(eLevel::debug, fmt);
			}

			template<typename... Args>
			AU_INLINE static void Debug(std::string&& fmt, Args &&...args) {
				if (!global::is_console_enable) return;
				console_impl::Write(eLevel::debug, std::move(fmt), std::forward<Args>(args)...);
			}
			template<typename... Args>
			AU_INLINE static void Debug(std::string& fmt, Args &&...args) {
				if (!global::is_console_enable) return;
				console_impl::Write(eLevel::debug, fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			AU_INLINE static void Write(eLevel lvl, std::string&& fmt, Args &&...args) {
				if (!global::is_console_enable) return;
				console_impl::Write(lvl, std::move(fmt), std::forward<Args>(args)...);
			}

			template<typename... Args>
			AU_INLINE static void Write(eLevel lvl, std::string& fmt, Args &&...args) {
				if (!global::is_console_enable) return;
				console_impl::Write(lvl, fmt, std::forward<Args>(args)...);
			}
		protected:

		private:
		}; // class console

	}
}

#endif // USE_AU_LOGGER

#endif //__AUTILITY_LOGGER_H__