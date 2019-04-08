#ifndef AGPU_THREADED_QUEUE_HPP
#define AGPU_THREADED_QUEUE_HPP

#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <assert.h>

namespace AgpuGL
{

typedef std::function<void ()> JobCommand;

/**
 * An asynhronous job that is executed in a separate thread.
 */
class AsyncJob
{
public:
    AsyncJob(const JobCommand &command, bool autoreleased = false)
        : nextJob(nullptr), finished(false), autoreleased(autoreleased), command(command) {}
    ~AsyncJob() { }

    void execute()
    {
        command();
        if(!autoreleased)
        {
            std::unique_lock<std::mutex> l(mutex);
            finished = true;
            finishedCondition.notify_all();
        }
        else
        {
            delete this;
        }
    }

    void wait()
    {
        std::unique_lock<std::mutex> l(mutex);
        while(!finished)
            finishedCondition.wait(l);
    }

	AsyncJob *nextJob;

private:
    bool finished;
    bool autoreleased;
    std::mutex mutex;
    std::condition_variable finishedCondition;
    JobCommand command;
};

/**
 * A thread that processes jobs from a queue.
 */
class JobQueue
{
public:
    JobQueue()
        : isRunning_(false), isShuttingDown_(false), firstPendingJob(nullptr), lastPendingJob(nullptr) {}

    ~JobQueue()
    {
        if(isRunning_)
            shutdown();
    }

    void start()
    {
        assert(!isRunning_);
        isShuttingDown_ = false;
        isRunning_ = true;
        std::thread t([this] {
            jobThreadEntry();
        });

        jobThread.swap(t);
    }

    void shutdown()
    {
        assert(isRunning_);

        {
            std::unique_lock<std::mutex> l(mutex);
            isShuttingDown_ = true;
            moreWorkCondition.notify_all();
        }
        jobThread.join();
        isRunning_ = false;
    }

    void addJob(AsyncJob* job)
    {
        std::unique_lock<std::mutex> l(mutex);
		if (lastPendingJob)
			lastPendingJob->nextJob = job;
		else
			firstPendingJob = job;
		job->nextJob = nullptr;
		lastPendingJob = job;
        moreWorkCondition.notify_all();
    }

private:
    void jobThreadEntry()
    {
        for(;;)
        {
            AsyncJob *nextJob = nullptr;
            {
                std::unique_lock<std::mutex> l(mutex);
                while(!isShuttingDown_ && !firstPendingJob)
                    moreWorkCondition.wait(l);
                if(isShuttingDown_)
                    return;

                nextJob = firstPendingJob;
				firstPendingJob = firstPendingJob->nextJob;
				if (!firstPendingJob)
					lastPendingJob = nullptr;
            }

            if(nextJob)
                nextJob->execute();
        }
    }

    std::thread jobThread;
    std::mutex mutex;
    std::condition_variable moreWorkCondition;

    bool isRunning_;
    bool isShuttingDown_;
	AsyncJob *firstPendingJob;
	AsyncJob *lastPendingJob;
};

} // End of namespace AgpuGL

#endif //AGPU_THREADED_QUEUE_HPP
