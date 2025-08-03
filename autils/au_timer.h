#ifndef __AU_TIMER_H__
#define __AU_TIMER_H__
#include "au_common.h"

#ifdef USE_AU_TIMER

#include <chrono>
#include <ctime>


namespace au {
	namespace time {
		using clock = std::chrono::high_resolution_clock;
		using system = std::chrono::system_clock;
		using steady = std::chrono::steady_clock;

		using nano = std::chrono::nanoseconds; // 10^-9 s
		using micro =  std::chrono::microseconds; // 10^-6 s
		using milli = std::chrono::milliseconds; // 10^-3 s
		using second = std::chrono::seconds; // s
		using minute = std::chrono::minutes; // 60s
		using hour = std::chrono::hours; // 3600s
	}
}
namespace au {
	namespace time {
		namespace global{
			static clock::time_point clock_start;
			static clock::time_point clock_end;

			static system::time_point system_start;
			static system::time_point system_end;

			static steady::time_point steady_start;
			static steady::time_point steady_end;
			
		}
	}
}

namespace au {
	namespace time {

		AU_API static auto get_system_now() {
			return std::chrono::system_clock::now();
		}

		class AU_API measure
		{
		public:
			AU_INLINE static auto now(){
				return steady::now();
			}
			AU_INLINE static void start(){
				global::steady_start = steady::now();
			}
			AU_INLINE static void end()
			{
				global::steady_end = steady::now();
			}
			template<typename T = time::milli>
			AU_INLINE static int64_t get_duration(){
				return std::chrono::duration_cast<T>(global::steady_end - global::steady_start).count();
			}

			template<typename T = time::milli>
			AU_INLINE static int64_t get_duration(steady::time_point start, steady::time_point end){
				return std::chrono::duration_cast<T>(end - start).count();
			}
		protected:

		private:

		};
	}
}

#endif //USE_AU_TIMER

#endif //__AU_TIMER_H__