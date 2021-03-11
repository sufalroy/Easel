#include "Precompiled.h"
#include "Easel/Core/JobSystem.h"
#include "Easel/Maths/Maths.h"

#include <atomic>
#include <thread>
#include <condition_variable>
#include <deque>

#ifdef EASEL_PLATFORM_WINDOWS
#define NOMINMAX
#include <Windows.h>
#include <comdef.h>
#endif

namespace Easel {

	namespace System {

		// Fixed size very simple thread safe ring buffer
		template <typename T, size_t capacity>
		class ThreadSafeRingBuffer {
		public:
			// Push an item to the end if there is free space
			//	Returns true if succesful
			//	Returns false if there is not enough space
			inline bool push_back(const T& item) {

				EASEL_PROFILE_FUNCTION();
				bool result = false;
				lock.lock();
				size_t next = (head + 1) % capacity;
				if (next != tail) {
					data[head] = item;
					head = next;
					result = true;
				}
				lock.unlock();
				return result;
			}

			// Get an item if there are any
			//	Returns true if succesful
			//	Returns false if there are no items
			inline bool pop_front(T& item) {

				EASEL_PROFILE_FUNCTION();
				bool result = false;
				lock.lock();
				if (tail != head) {
					item = data[tail];
					tail = (tail + 1) % capacity;
					result = true;
				}
				lock.unlock();
				return result;
			}

		private:
			T data[capacity];
			size_t head = 0;
			size_t tail = 0;
			std::mutex lock;
		};

		namespace JobSystem {

			struct Job {
				Context* ctx;
				std::function<void(JobDispatchArgs)> task;
				uint32_t groupID;
				uint32_t groupJobOffset;
				uint32_t groupJobEnd;
				uint32_t sharedmemory_size;
			};

			uint32_t numThreads = 0;
			ThreadSafeRingBuffer<Job, 256> jobQueue;
			std::condition_variable wakeCondition;
			std::mutex wakeMutex;

			inline bool work() {

				EASEL_PROFILE_FUNCTION();
				Job job;
				if (jobQueue.pop_front(job)) {
					JobDispatchArgs args;
					args.groupID = job.groupID;
					if (job.sharedmemory_size > 0)
						args.sharedmemory = alloca(job.sharedmemory_size);
					else
						args.sharedmemory = nullptr;

					for (uint32_t i = job.groupJobOffset; i < job.groupJobEnd; ++i) {
						
						EASEL_PROFILE_SCOPE("Group Loop");
						args.jobIndex = i;
						args.groupIndex = i - job.groupJobOffset;
						args.isFirstJobInGroup = (i == job.groupJobOffset);
						args.isLastJobInGroup = (i == job.groupJobEnd - 1);
						job.task(args);
					}

					job.ctx->counter.fetch_sub(1);
					return true;
				}

				return false;
			}
			
			void OnInit() {

				// Retrieve the number of hardware threads is this System:
				auto numCores = std::thread::hardware_concurrency();

				// Calculate the actual number of worker threads we want:
				numThreads = Easel::Maths::Max(1U, numCores - 1);

				for (uint32_t threadID = 0; threadID < numThreads; ++threadID) {
					std::thread worker([threadID] {
						std::stringstream ss;
						ss << "JobSystem_" << threadID;
						EASEL_PROFILE_SETTHREADNAME(ss.str().c_str());

						while (true) {
							if (!work()) {
								// no job, put thread to sleep.
								std::unique_lock<std::mutex> lock(wakeMutex);
								wakeCondition.wait(lock);
							}
						}
					});

#ifdef EASEL_PLATFORM_WINDOWS
					// Do Windows-specific thread setup:
					HANDLE handle = (HANDLE)worker.native_handle();

					// Put each thread on to dedicated core
					DWORD_PTR affinityMask = 1ull << threadID;
					DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
					EASEL_ASSERT(affinity_result > 0, "");

					// Increase thread priority.
					BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
					EASEL_ASSERT(priority_result != 0, "");

					// Name the thread:
					std::wstringstream wss;
					wss << "JobSystem_" << threadID;
					HRESULT hr = SetThreadDescription(handle, wss.str().c_str());
					EASEL_ASSERT(SUCCEEDED(hr), "");
#endif

					worker.detach();
				}

				EASEL_CORE_INFO("Initialised JobSystem with [{0} cores] [{1} threads]", numCores, numThreads);
			}

			uint32_t GetThreadCount() {
				return numThreads;
			}

			void Execute(Context& ctx, const std::function<void(JobDispatchArgs)>& task) {
				
				// Context state is updated:
				ctx.counter.fetch_add(1);

				Job job;
				job.ctx = &ctx;
				job.task = task;
				job.groupID = 0;
				job.groupJobOffset = 0;
				job.groupJobEnd = 1;
				job.sharedmemory_size = 0;

				// Try to push a new job until it is pushed successfully:
				while (!jobQueue.push_back(job)) { wakeCondition.notify_all(); work(); }

				// Wake any one thread that might be sleeping:
				wakeCondition.notify_one();
			}

			void Dispatch(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& task, size_t sharedmemory_size) {

				EASEL_PROFILE_FUNCTION();

				if (jobCount == 0 || groupSize == 0) {
					return;
				}

				// Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
				const uint32_t groupCount = DispatchGroupCount(jobCount, groupSize);

				// Context state is updated:
				ctx.counter.fetch_add(groupCount);

				Job job;
				job.ctx = &ctx;
				job.task = task;
				job.sharedmemory_size = (uint32_t)sharedmemory_size;

				for (uint32_t groupID = 0; groupID < groupCount; ++groupID) {
					// For each group, generate one real job:
					job.groupID = groupID;
					job.groupJobOffset = groupID * groupSize;
					job.groupJobEnd = std::min(job.groupJobOffset + groupSize, jobCount);

					// Try to push a new job until it is pushed successfully:
					while (!jobQueue.push_back(job)) { wakeCondition.notify_all(); work(); }
				}
			}

			uint32_t DispatchGroupCount(uint32_t jobCount, uint32_t groupSize) {
				// Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
				return (jobCount + groupSize - 1) / groupSize; 
			}

			bool IsBusy(const Context& ctx) {
				// Whenever the main thread label is not reached by the workers, it indicates that some worker is still alive
				return ctx.counter.load() > 0;
			}

			void Wait(const Context& ctx) {
				wakeCondition.notify_all();
				while (IsBusy(ctx)) { work(); }
			}
		}
	}
}


