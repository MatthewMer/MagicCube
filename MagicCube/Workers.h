#pragma once

#include <future>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <utility>
#include <mutex>
#include <tuple>
#include <condition_variable>

namespace Workers {
	namespace {
		static size_t const MAX_THREAD_COUNT = std::thread::hardware_concurrency() * 2;
	}

	template<typename T>
	class ThreadPool {
	public:
		using promise_t = std::promise<T>;
		using future_t = std::future<T>;
		using function_t = std::function<T()>;
		using callback_t = std::function<void(T&)>;
		using job_t = std::tuple<promise_t, future_t, function_t, callback_t>;
		using jobpool_t = std::queue<job_t>;
		using threadpool_t = std::vector<std::thread>;

		ThreadPool();
		ThreadPool(size_t const& thread_count);
		~ThreadPool();
		[[nodiscard]] future_t Enqueue(function_t const& fn);
		[[nodiscard]] void Enqueue(function_t const& fn, callback_t const& cb);
		[[nodiscard]] size_t ThreadCount() const;
		void Stop();
		bool Start(size_t const& thread_count);
		bool Start();
		void Wait();

	private:
		size_t m_threadCount;
		threadpool_t m_threadPool;
		jobpool_t m_jobPool;

		std::condition_variable m_cv;
		std::mutex m_mutex;
		bool m_terminate = false;
		std::condition_variable m_cvwait;
		std::mutex m_mutexwait;

		void Thread();
		void SetThreadCount(size_t const& thread_count);
		void Callback(callback_t const& cb, future_t&& futr);
	};

	template<typename T>
	ThreadPool<T>::ThreadPool() {
		SetThreadCount(0);
		m_terminate = true;
	}

	template<typename T>
	ThreadPool<T>::ThreadPool(size_t const& thread_count) : m_threadCount(thread_count) {
		Start(thread_count);
	}

	template<typename T>
	ThreadPool<T>::~ThreadPool() {
		Stop();
	}

	template<typename T>
	void ThreadPool<T>::Wait() {
		while (m_jobPool.size() > 0) {
			std::unique_lock<std::mutex> lock_wait(m_mutexwait);
			m_cvwait.wait(lock_wait, [this] {
				return m_jobPool.size() == 0;
				});
		}
	}

	template<typename T>
	void ThreadPool<T>::Stop() {
		m_terminate = true;
		m_cv.notify_all();
		for (auto& t : m_threadPool) {
			if (t.joinable()) {
				t.join();
			}
		}
	}

	template<typename T>
	bool ThreadPool<T>::Start(size_t const& thread_count) {
		SetThreadCount(thread_count);
		return Start();
	}

	template<typename T>
	bool ThreadPool<T>::Start() {
		if (!m_terminate) {
			Stop();
		}
		m_terminate = false;
		m_threadPool.resize(m_threadCount);
		for (size_t i = 0; i < m_threadCount; ++i) {
			m_threadPool[i] = std::thread(&ThreadPool<T>::Thread, this);
		}

		for (auto& t : m_threadPool) {
			if (!t.joinable()) {
				Stop();
				return false;
			}
		}
		return true;
	}

	template<typename T>
	void ThreadPool<T>::SetThreadCount(size_t const& thread_count) {
		m_threadCount = thread_count;
		if (m_threadCount < 1 || m_threadCount > MAX_THREAD_COUNT) {
			m_threadCount = MAX_THREAD_COUNT;
		}
		if (m_threadCount < 1) {
			m_threadCount = 4;
		}
	}

	template<typename T>
	void ThreadPool<T>::Thread() {
		using std::get;

		while (true) {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, [this] {
				return !m_jobPool.empty() || m_terminate;
				});
			if (m_terminate) {
				return;
			}

			auto& job = m_jobPool.front();
			auto prms = std::move(get<0>(job));
			auto futr = std::move(get<1>(job));
			auto fn = get<2>(job);
			auto cb = get<3>(job);
			m_jobPool.pop();
			lock.unlock();
			
			prms.set_value(fn());
			if (futr.valid()) {
				Callback(cb, std::move(futr));
			}
			else {
				m_cvwait.notify_all();
			}
		}
	}

	template<typename T>
	void ThreadPool<T>::Callback(callback_t const& cb, future_t&& futr) {
		auto result = futr.get();
		cb(result);
		m_cvwait.notify_all();
	}

	template<typename T>
	auto ThreadPool<T>::Enqueue(function_t const& fn) -> future_t {
		promise_t prms;
		future_t futr(prms.get_future());
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_jobPool.push({ std::move(prms), std::move(future_t()), fn, callback_t()});
		}
		m_cv.notify_one();
		return futr;
	}

	template<typename T>
	auto ThreadPool<T>::Enqueue(function_t const& fn, callback_t const& cb) -> void {
		promise_t prms;
		future_t futr(prms.get_future());
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_jobPool.push({ std::move(prms), std::move(futr), fn, cb });
		}
		m_cv.notify_one();
	}

	template<typename T>
	size_t ThreadPool<T>::ThreadCount() const {
		return m_threadCount;
	}
}