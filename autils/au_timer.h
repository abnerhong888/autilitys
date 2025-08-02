#ifndef __AU_TIMER_H__
#define __AU_TIMER_H__
#include "au_common.h"

#ifdef USE_AU_TIMER

#include <chrono>
#include <ctime>


namespace au {
	namespace time {

		using nano = std::chrono::duration<double, std::chrono::nanoseconds::period>; // 10^-9 s
		using micro = std::chrono::duration<double, std::chrono::microseconds::period>; // 10^-6 s
		using milli = std::chrono::duration<double, std::chrono::milliseconds::period>; // 10^-3 s
		using second = std::chrono::duration<double, std::chrono::seconds::period>; // s
		using minute = std::chrono::duration<double, std::chrono::minutes::period>; // 60s
		using hour = std::chrono::duration<double, std::chrono::hours::period>; // 3600s

		enum class etimer_type : int
		{
			clock,
			system,
		};

		enum class eformat : int 
		{
			nanos,
			micros,
			millis,
			seconds,
			minutes,
			hours,
		};
	}
}
namespace au {
	namespace time {
		namespace global{
			using clock_type = std::chrono::high_resolution_clock;
			using system_type = std::chrono::system_clock;

			

			static etimer_type timer_type;
			
			static clock_type::time_point start_time_clock;
			static system_type::time_point start_time_system;

			static clock_type::time_point end_time_clock;
			static system_type::time_point end_time_system;

			static clock_type::time_point duration_time_clock;
			static system_type::time_point duration_time_system;
		}
	}
}

namespace au {
	namespace time {

		AU_API static auto now() {
			return std::chrono::system_clock::now();
		}

		class AU_API timer
		{
		public:
			AU_INLINE static void start(etimer_type type = etimer_type::clock)
			{
				global::timer_type = type;
				switch (global::timer_type)
				{
				case etimer_type::clock: 
					global::start_time_clock = global::clock_type::now(); 
					break;
				case etimer_type::system:
					global::start_time_system = global::system_type::now();
					break;
				}
			}
			AU_INLINE static void end()
			{
				switch (global::timer_type)
				{
				case etimer_type::clock:
					global::end_time_clock = global::clock_type::now();
					break;
				case etimer_type::system:
					global::end_time_system = global::system_type::now();
					break;
				}
			}

			AU_INLINE static auto get_duration(eformat fmt = eformat::millis)
			{
				if (global::timer_type == etimer_type::clock)
				{
					switch (fmt)
					{
					case au::time::eformat::nanos:
						return std::chrono::duration_cast<nano>(global::end_time_clock - global::start_time_clock).count();
					case au::time::eformat::micros:
						return std::chrono::duration_cast<micro>(global::end_time_clock - global::start_time_clock).count();
					case au::time::eformat::millis:
						return std::chrono::duration_cast<milli>(global::end_time_clock - global::start_time_clock).count();
					case au::time::eformat::seconds:
						return std::chrono::duration_cast<second>(global::end_time_clock - global::start_time_clock).count();
					case au::time::eformat::minutes:
						return std::chrono::duration_cast<minute>(global::end_time_clock - global::start_time_clock).count();
					case au::time::eformat::hours:
						return std::chrono::duration_cast<hour>(global::end_time_clock - global::start_time_clock).count();
					}
				}
				else if (global::timer_type == etimer_type::system)
				{
					switch (fmt)
					{
					case au::time::eformat::nanos:
						return std::chrono::duration_cast<nano>(global::end_time_system - global::start_time_system).count();
					case au::time::eformat::micros:
						return std::chrono::duration_cast<micro>(global::end_time_system - global::start_time_system).count();
					case au::time::eformat::millis:
						return std::chrono::duration_cast<milli>(global::end_time_system - global::start_time_system).count();
					case au::time::eformat::seconds:
						return std::chrono::duration_cast<second>(global::end_time_system - global::start_time_system).count();
					case au::time::eformat::minutes:
						return std::chrono::duration_cast<minute>(global::end_time_system - global::start_time_system).count();
					case au::time::eformat::hours:
						return std::chrono::duration_cast<hour>(global::end_time_system - global::start_time_system).count();
					}
				}
				return (double)-1;
			}
			template<typename T>
			AU_INLINE static T get_duration()
			{
				if (global::timer_type == etimer_type::clock)
				{
					return std::chrono::duration_cast<T>(global::end_time_clock - global::start_time_clock);
				}
				else if (global::timer_type == etimer_type::system)
				{
					return std::chrono::duration_cast<T>(global::end_time_system - global::start_time_system);
				}
				return {};
			}
		protected:

		private:

		};
	}
}

#endif //USE_AU_TIMER

#endif //__AU_TIMER_H__