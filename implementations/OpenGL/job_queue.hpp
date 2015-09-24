#ifndef AGPU_THREADED_QUEUE_HPP
#define AGPU_THREADED_QUEUE_HPP

#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>

typedef std::function<void ()> JobCommand;

/**
 * An asynhronous job that is executed in a separate thread.
 */
class AsyncJob
{
public:
    AsyncJob(const JobCommand &command)
        : finished(false), command(command) {}
    ~AsyncJob() { }

    void execute()
    {
        command();
        {
            std::unique_lock<std::mutex> l(mutex);
            finished = true;
            finishedCondition.notify_all();
        }
    }

    void wait()
    {
        std::unique_lock<std::mutex> l(mutex);
        while(!finished)
            finishedCondition.wait(l);
    }
private:
    bool finished;
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
        : isRunning_(false), isShuttingDown_(false) {}

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
        jobs.push_back(job);
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
                while(!isShuttingDown_ && jobs.empty())
                    moreWorkCondition.wait(l);
                if(isShuttingDown_)
                    return;

                nextJob = jobs.back();
                jobs.pop_back();
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
    std::vector<AsyncJob*> jobs;
};

#endif //AGPU_THREADED_QUEUE_HPP
