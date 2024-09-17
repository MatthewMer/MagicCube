#pragma once

#include <thread>
#include <windows.h>

namespace Threading {
	enum class ThreadPriority {
		TH_BACKGROUND_BEGIN = THREAD_MODE_BACKGROUND_BEGIN,
		TH_BACKGROUND_END = THREAD_MODE_BACKGROUND_END,
		TH_ABOVE_NORMAL = THREAD_PRIORITY_ABOVE_NORMAL,
		TH_BELOW_NORMAL = THREAD_PRIORITY_BELOW_NORMAL,
		TH_HIGHEST = THREAD_PRIORITY_HIGHEST,
		TH_IDLE = THREAD_PRIORITY_IDLE,
		TH_LOWEST = THREAD_PRIORITY_LOWEST,
		TH_NORMAL = THREAD_PRIORITY_NORMAL,
		TH_TIME_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL
	};

	class Thread : public std::thread
	{
	public:
		using std::thread::thread;

		ThreadPriority GetPriority() {
			return static_cast<ThreadPriority>(GetThreadPriority(handle));
		}

		bool SetPriority(ThreadPriority const& prio) {
			return SetThreadPriority(handle, static_cast<int>(prio));
		}

	private:
		native_handle_type handle = native_handle();
	};
}