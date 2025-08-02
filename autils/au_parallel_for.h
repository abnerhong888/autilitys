#ifndef __AUTILITY_PARALLEL_FOR_H__
#define __AUTILITY_PARALLEL_FOR_H__

#include "au_common.h"

#ifndef USING_AU_THREAD
#error Must define USING_AU_THREAD if using thread define.
#endif

#include "au_thread.h"
#include <functional>
#include <memory>
#ifdef USING_AU_PARALLEL_FOR

namespace au {
	namespace parallel {

		class range
		{
		public:
			int start = 0;
			int end = 0;
			range() = delete;
			range(int _start, int _end) : start(_start), end(_end) {}
			range(const range& in) {
				*this = in;
			}
			AU_INLINE void operator= (const range& in) noexcept {
				this->start = in.start;
				this->end = in.end;
			}
			AU_INLINE int total() { return end - start; }
		};

		class For_Base {
		public:
			virtual void operator() (const range& _range) = 0;
		};

		namespace impl {
			class For_Wrapper {
			public:
				~For_Wrapper() { base = nullptr; }
				For_Wrapper(const For_Base& _base) { base = (For_Base*)&_base; }
				void operator() (const range& _range) { (*base)(_range); }
			private:
				For_Base* base = nullptr;
			};
		}

	}
}
namespace au {
	namespace parallel {
		namespace impl {
			using namespace thread::pool;

			template<typename _Callback>
			class parallelWorker : public TheadWorker
			{
			public:
				range rng;
				_Callback callback;
				parallelWorker() = delete;
				parallelWorker(const range &_rng, _Callback &_callback) : rng(_rng), callback(_callback) {}

				void Run() override
				{
					callback(rng);
				}
			};

			template<typename _Callback>
			AU_INLINE static void for__(range _range, _Callback &_callback, int _numThread) {
				auto _pool = Sequential::CreatePtr();
				int numThreads = (int)std::thread::hardware_concurrency();

				if (_numThread > 0) {
					if (_numThread < numThreads)
						numThreads = _numThread;
				}

				AU_ASSERT(numThreads != 0, "Number of thread is zero.");

				int _divid = (int)((float)(_range.total()) / numThreads);

				int start = 0;
				int end = 0;
				int times = _divid == 0 ? _range.total() : numThreads;
				_divid = _divid == 0 ? 1 : _divid;

				for (int i = 0; i < times; i++) {
					end += _divid;
					if (i == numThreads - 1)
						end = _range.total();

					_pool->Apply(
						{ new parallelWorker<_Callback>(range(start, end), _callback), 0 }
					);
					start = end;
				}

				_pool->Start();

				while (!_pool->IsFinished()) {
					au::sleep_ms(1);
				}
			}
		}
	}
}
namespace au {
	namespace parallel {
		using namespace thread::pool;

		AU_INLINE static void FOR_(range _range, std::function<void(const range& _range)> _func, int _numThread = -1) 
		{
			impl::for__(_range, _func, _numThread);
		}

		AU_INLINE static void FOR_(range _range, const For_Base &_base, int _numThread = -1)
		{
			impl::For_Wrapper wrapper(_base);
			impl::for__(_range, wrapper, _numThread);
		}
	}
}

#endif // USING_AU_PARALLEL_FOR

#endif //__AUTILITY_PARALLEL_FOR_H__