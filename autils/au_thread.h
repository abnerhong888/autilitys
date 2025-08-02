#ifndef __AUTILITY_AUTHREAD_H__
#define __AUTILITY_AUTHREAD_H__
#include "au_common.h"

#ifdef USING_AU_THREAD

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <string>
#include <list>
#include <memory>

namespace au {
	namespace thread{

	using thread_fptr = void(*)(void);
	using thread_static_entry_fptr = void(*)(void* UserData);

	// Only work in windows system
	enum class eThreadPriority : int
	{
		HIGHER = 0,
		SAME,
		LOWER,
	};

#if defined(_MSC_VER) && defined(_WIN32)
	void WIN32_Thread_Setting(HWND thread_handle, eThreadPriority priority, std::string threadName, int threadID);
#endif

	// Interface Thread Run
	class IThreadRun
	{
	public:
		virtual void Run() {};
	};
	
	// Interface Thread
	class IThread: public IThreadRun
	{
	public:
		IThread() {}
		virtual bool AddJob(thread_fptr fptr) = 0;
		virtual bool DeleteJob(thread_fptr FuncPtr) = 0;
		virtual void Start() = 0;
		virtual void Pause() = 0;
		virtual void Terminate() = 0;
		virtual bool isWorking() = 0;
		virtual void EnableOneTimeJob() = 0;
		virtual void DisableOneTimeJob() = 0;
		virtual std::string ThreadName() = 0;
		virtual int ThreadID() = 0;
		virtual void ThreadLock() = 0;
		virtual void ThreadUnLock() = 0;

		bool paused = false;
		bool terminated = false;
		bool stopped = false;
		bool onetime_job = false;
		bool is_working = false;
		std::string m_thread_name;
	protected:
		std::vector<thread_fptr> func_ptrs;
	private:
	};

#pragma region cppThread

	// thread name and thread priority only support on windows 
	class cppThread : public IThread
	{
	public:
		cppThread( std::string _thread_name = "", eThreadPriority _priority = eThreadPriority::SAME,
			thread_static_entry_fptr _static_thread_entry = nullptr, void* _user_data = nullptr)
		{
			Initialize(_thread_name, _priority, _static_thread_entry, _user_data);
		}

		void Initialize( std::string _thread_name = "", eThreadPriority _priority = eThreadPriority::SAME,
			thread_static_entry_fptr _static_thread_entry = nullptr, void* _user_data = nullptr
		);
		

		bool AddJob(thread_fptr _func_ptr) override;
		bool DeleteJob(thread_fptr _func_ptr) override;
		void Start() override;
		void Pause() override;
		void Terminate() override;
		bool isWorking() override;
		void EnableOneTimeJob() override;
		void DisableOneTimeJob() override;
		std::string ThreadName() override;
		int ThreadID() override;
		void ThreadLock() override;
		void ThreadUnLock() override;

		std::thread* m_thread = nullptr;
		std::thread::id m_threadId;
		std::mutex m_mutex_lock;
		std::mutex m_mutex_signal;
		int m_payload = 0;
		std::condition_variable m_condition;
		bool is_thread_ready = false;

		
	protected:
		template<typename T>
		AU_INLINE static void CustomStaticThreadEntry(void* user_data)
		{
			(reinterpret_cast<T*>(user_data))->CustomThreadEntry(user_data);
		}

		template<typename T>
		AU_INLINE void CustomThreadEntry(void* user_data)
		{
			thread_running_rule<T>(user_data);
		}

	private:
		AU_INLINE static void StaticThreadEntry(void* user_data)
		{
			(reinterpret_cast<cppThread*>(user_data))->ThreadEntry(user_data);
		}
		

		AU_INLINE void ThreadEntry(void* user_data)
		{
			thread_running_rule<IThreadRun>((void*)user_data);
		}

		template<typename T>
		AU_INLINE void thread_running_rule(void* user_data)
		{
			if (!is_thread_ready)
				is_thread_ready = true;
		l_Begin:
			m_payload = 0;
			std::unique_lock<std::mutex> lk(m_mutex_signal);
			m_condition.wait(lk, [&]() {return m_payload == 1; });

			while (!terminated)
			{
				is_working = true;
				m_mutex_lock.lock();
				//Working Area --
				if (user_data)
					(reinterpret_cast<T*>(user_data))->Run();

				for (thread_fptr fptr : func_ptrs)
					fptr();
				//--
				m_mutex_lock.unlock();

				if (paused || onetime_job)
				{
					is_working = false;
					goto l_Begin;
				}

				au::sleep_ms(1);
			}
			is_working = false;
			stopped = true;
		}
	

	};

	AU_INLINE void cppThread::Initialize( std::string _thread_name, eThreadPriority _priority, 
		thread_static_entry_fptr _static_thread_entry, void* _user_data)
	{
		if (m_thread == nullptr)
		{
			m_thread = new std::thread(
				_static_thread_entry ? _static_thread_entry : cppThread::StaticThreadEntry,
				_user_data ? _user_data : this
			);
		}

		m_threadId = m_thread->get_id();

#if defined(_MSC_VER) && defined(_WIN32)
		HWND thread_hwd = (HWND)m_thread->native_handle();
		m_thread_name = _thread_name;
		WIN32_Thread_Setting(thread_hwd, _priority, m_thread_name, ThreadID());
#endif

		m_thread->detach();

		while (!is_thread_ready) { au::sleep_ms(1); }

	}

	AU_INLINE bool cppThread::AddJob(thread_fptr _func_ptr)
	{
		m_mutex_lock.lock();
		func_ptrs.push_back(_func_ptr);
		m_mutex_lock.unlock();
		return true;
	}
	AU_INLINE bool cppThread::DeleteJob(thread_fptr _func_ptr)
	{
		if (func_ptrs.size() > 0)
		{
			bool r = false;
			m_mutex_lock.lock();

			std::vector<thread_fptr>::iterator it = std::find(func_ptrs.begin(), func_ptrs.end(), _func_ptr);

			if (it != func_ptrs.end())
			{
				func_ptrs.erase(it);
				r = true;
			}
			else
				r = false;

			m_mutex_lock.unlock();
			return r;

		}
		return false;
	}

	AU_INLINE void cppThread::Start()
	{
		paused = false;
		terminated = false;
		stopped = false;
		m_payload = 1;
		m_condition.notify_one();
	}
	AU_INLINE void cppThread::Pause()
	{
		paused = true;
	}
	AU_INLINE void cppThread::Terminate()
	{
		terminated = true;

		while (!stopped) {
			m_payload = 1;
			m_condition.notify_one();
			au::sleep_ms(1);
		}

		if (m_thread)
		{
			delete m_thread;
			m_thread = nullptr;
		}

	}
	AU_INLINE bool cppThread::isWorking()
	{
		return is_working;
	}
	AU_INLINE void cppThread::EnableOneTimeJob()
	{
		onetime_job = true;
	}
	AU_INLINE void cppThread::DisableOneTimeJob()
	{
		onetime_job = false;
	}
	AU_INLINE std::string cppThread::ThreadName()
	{
#if defined(_MSC_VER) && defined(_WIN32)
		return m_thread_name;
#else
		std::ostringstream ss;
		ss << m_threadId;
		m_thread_name = ss.str();
		return m_thread_name;
#endif

	}
	AU_INLINE int cppThread::ThreadID()
	{
		std::ostringstream ss;
		ss << m_threadId;
		return std::stoi(ss.str());
	}

	AU_INLINE void cppThread::ThreadLock()
	{
		m_mutex_lock.lock();
	}
	AU_INLINE void cppThread::ThreadUnLock()
	{
		m_mutex_lock.unlock();
	}

#pragma endregion


#if defined(_MSC_VER) && defined(_WIN32)
	AU_INLINE static void WIN32_Thread_Setting(HWND thread_handle, eThreadPriority priority, std::string threadName, int threadID)
	{
		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // must be 0x1000
			LPCSTR szName; // pointer to name (in user addr space)
			DWORD dwThreadID; // thread ID (-1=caller thread)
			DWORD dwFlags; // reserved for future use, must be zero
		} THREADNAME_INFO;

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName != "" ? threadName.c_str() : "WorkingThread";
		info.dwThreadID = threadID;
		info.dwFlags = 0;

		//__try
		//{
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
		//}
		//__except (EXCEPTION_CONTINUE_EXECUTION)
		//{

		//}

		// Set the priority based on required priority
		bool is;
		switch (priority)
		{
		case eThreadPriority::HIGHER:
			is = SetThreadPriority(thread_handle, THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case eThreadPriority::LOWER:
			is = SetThreadPriority(thread_handle, THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case eThreadPriority::SAME:
		default:
			break;
		}
	}
#endif

	} // namesapce thread
}// namespace au


namespace au {
	namespace thread {
		namespace pool {

			enum class eThreadPoolMode : int {
				// Execute worker immediately
				Immediate,
				// Sequential execute worker
				Sequential
			};

			class TheadWorker : public cppThread
			{
			public:
				TheadWorker() : cppThread(
					"auThread_Worker",
					eThreadPriority::SAME,
					TheadWorker::StaticThreadEntry,
					this
				)
				{
				}
				virtual void Run() = 0;

			protected:

				AU_INLINE static void StaticThreadEntry(void* user_data)
				{
					(reinterpret_cast<TheadWorker*>(user_data))->ThreadEntry(user_data);
				}
				AU_INLINE void ThreadEntry(void* user_data)
				{
					thread_running_rule((void*)user_data);
				}

				AU_INLINE void thread_running_rule(void* user_data)
				{
					if (!is_thread_ready)
						is_thread_ready = true;
					m_payload = 0;
					std::unique_lock<std::mutex> lk(m_mutex_signal);
					m_condition.wait(lk, [&]() {return m_payload == 1; });

					is_working = true;

					m_mutex_lock.lock();

					if (user_data)
						(reinterpret_cast<TheadWorker*>(user_data))->Run();

					m_mutex_lock.unlock();

					is_working = false;
					stopped = true;

				}
			protected:
			};

			class Immediate : public cppThread
			{
			private:
				using worker_sptr = std::shared_ptr<TheadWorker>;

				std::list<worker_sptr> thread_workers_immediate;

				Immediate() :
					cppThread("auThreadPool_Immediate", eThreadPriority::SAME,
						Immediate::StaticThreadEntry,this)
				{
					Start();
				}
			public:
				~Immediate()
				{
					while (thread_workers_immediate.size() != 0) { au::sleep_ms(1); }
					Terminate();
				}

				AU_INLINE static std::shared_ptr<Immediate> CreatePtr() {
					return std::shared_ptr<Immediate>(new Immediate());
				}

				AU_INLINE static std::shared_ptr<Immediate> Instance()
				{
					static std::shared_ptr<Immediate> instance_ptr;

					if (instance_ptr == nullptr)
						instance_ptr = CreatePtr();
					return instance_ptr;
				}

				AU_INLINE void Execute(TheadWorker* _worker)
				{
					ThreadLock();
					thread_workers_immediate.emplace_back(worker_sptr(_worker));
					ThreadUnLock();
				}
			protected:
				AU_INLINE static void StaticThreadEntry(void* user_data)
				{
					(reinterpret_cast<Immediate*>(user_data))->ThreadEntry(user_data);
				}
				AU_INLINE void ThreadEntry(void* user_data)
				{
					if (!is_thread_ready)
						is_thread_ready = true;
				l_Begin:
					paused = true;
					m_payload = 0;
					std::unique_lock<std::mutex> lk(m_mutex_signal);
					m_condition.wait(lk, [&]() {return m_payload == 1; });
					paused = false;

					while (!terminated)
					{
						is_working = true;

						m_mutex_lock.lock();

						for (worker_sptr workers : thread_workers_immediate)
						{
							if (!workers->isWorking() && !workers->stopped)
								workers->Start();
							else if (workers->stopped)
								workers->Terminate();
						}

						thread_workers_immediate.remove_if([](worker_sptr ptr) {
							return ptr->terminated;
							}
						);

						m_mutex_lock.unlock();

						if (paused || onetime_job)
						{
							is_working = false;
							goto l_Begin;
						}

						au::sleep_ms(1);
					}

					is_working = false;
					stopped = true;
				}
			};


			class Sequential : public cppThread
			{
			private:
				using worker_sptr = std::shared_ptr<TheadWorker>;

				std::list<std::pair<worker_sptr, int>> thread_workers_sequential;
				int min_priority = 9999999;
				int max_priority = 0;
				bool is_all_finished = false;

				Sequential() :
					cppThread("auThreadPool_Immediate", eThreadPriority::SAME,
						Sequential::StaticThreadEntry, this)
				{
				}
			public:
				~Sequential()
				{
					while (!paused) { au::sleep_ms(1); }
					Terminate();
				}

				AU_INLINE static std::shared_ptr<Sequential> CreatePtr() {
					return std::shared_ptr<Sequential>(new Sequential());
				}

				AU_INLINE static std::shared_ptr<Sequential> Instance()
				{
					static std::shared_ptr<Sequential> instance_ptr;

					if (instance_ptr == nullptr)
						instance_ptr = CreatePtr();
					return instance_ptr;
				}

				AU_INLINE void Apply(std::pair<TheadWorker*, int> _worker)
				{
					if (is_working) return;
					thread_workers_sequential.push_back({ worker_sptr(_worker.first), _worker.second });

					if (min_priority > _worker.second)
						min_priority = _worker.second;
					if (max_priority < _worker.second)
						max_priority = _worker.second;
				}
				AU_INLINE bool IsFinished()
				{
					bool r;
					ThreadLock();
					r = is_all_finished;
					ThreadUnLock();
					return r;
				}
			protected:
				AU_INLINE static void StaticThreadEntry(void* user_data)
				{
					(reinterpret_cast<Sequential*>(user_data))->ThreadEntry(user_data);
				}

				AU_INLINE void ThreadEntry(void* user_data)
				{
					if (!is_thread_ready)
						is_thread_ready = true;
				l_Begin:
					paused = true;
					m_payload = 0;
					std::unique_lock<std::mutex> lk(m_mutex_signal);
					m_condition.wait(lk, [&]() {return m_payload == 1; });
					paused = false;
					is_all_finished = false;
					bool is_stage_finished = false;
					int idx = min_priority;
					while (!terminated)
					{
						is_working = true;

						m_mutex_lock.lock();

						if (!is_all_finished)
						{
							for (auto workers : thread_workers_sequential) {
								if (workers.second == idx) {
									if (!workers.first->isWorking() && !workers.first->stopped)
										workers.first->Start();
								}
							}

							is_stage_finished = true;
							for (auto workers : thread_workers_sequential) {
								if (workers.second == idx) {
									is_stage_finished &= workers.first->stopped;
									if (workers.first->stopped)
										workers.first->Terminate();
								}
							}
							if (is_stage_finished)
							{
								idx++;
								if (idx > max_priority)
									is_all_finished = true;
							}
						}
						else
						{
							m_mutex_lock.unlock();
							thread_workers_sequential.clear();
							is_working = false;
							goto l_Begin;
						}

						m_mutex_lock.unlock();

						if (paused || onetime_job)
						{
							is_working = false;
							goto l_Begin;
						}

						au::sleep_ms(1);
					}

					is_working = false;
					stopped = true;
				}
			};

		} // namespace pool
	} // namespace thread
} // namespace au



#endif // USING_AU_THREAD

#endif // __AUTILITY_AUTHREAD_H__