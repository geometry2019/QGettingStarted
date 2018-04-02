#include <iostream>

#include "QGSThreadPool.h"

//msecs
static const unsigned long DEFAULT_SLEEP_TIME(1000);

QGSThreadPool::QGSThreadPool(const int minThreadCount, const int maxThreadCount, QObject *parent)
	: QThread(parent), 
	//mTaskQueueBlock(false), 
	mReleaseThreads(false), 
	mMaxThreadCount(maxThreadCount),
	mMinThreadCount(minThreadCount),
	//mActiveThreadCount(0),
	mExpiryTimeout(DEFAULT_SLEEP_TIME)
{
	if (minThreadCount >= maxThreadCount)
	{
		mMinThreadCount = 1;
	}
	if (maxThreadCount < 1)
	{
		mMaxThreadCount = QGSTaskThread::idealThreadCount();
	}
	mTaskQueue.clear();
	mTimer.moveToThread(this);
}

QGSThreadPool::~QGSThreadPool()
{
	QMutexLocker mutexLocker(&mMutex);

	for (auto & thread : mThreadList)
	{
		thread->deleteLater();
	}
}

QGSThreadPool & QGSThreadPool::getGlobalInstance()
{
	static QGSThreadPool instance(QGSTaskThread::idealThreadCount(), QGSTaskThread::idealThreadCount());
	if (!instance.isRunning())
	{
		instance.start();
	}
	return instance;
}

bool QGSThreadPool::addTaskBack(QGSTask * task)
{
	if (!task)
	{
		return false;
	}

	QMutexLocker mutexLocker(&mMutex);

	if (mReleaseThreads)
	{
		return false;
	}
	task->moveToThread(this);
	mTaskQueue.push_back(task);

	return true;
}

bool QGSThreadPool::addTaskFront(QGSTask * task)
{
	if (!task)
	{
		return false;
	}

	QMutexLocker mutexLocker(&mMutex);

	if (mReleaseThreads)
	{
		return false;
	}
	task->moveToThread(this);
	mTaskQueue.push_front(task);

	return true;
}

QGSThreadPool & QGSThreadPool::setMaxThreadCount(const quint32 maxThreadCount)
{
	QMutexLocker mutexLocker(&mMutex);

	if (maxThreadCount >= 1)
	{
		mMaxThreadCount = maxThreadCount;
	}

	return *this;
}

QGSThreadPool & QGSThreadPool::setMinThreadCount(const quint32 minThreadCount)
{
	QMutexLocker mutexLocker(&mMutex);

	if (minThreadCount <= mMaxThreadCount)
	{
		mMinThreadCount = minThreadCount;
	}

	return *this;
}

QGSThreadPool & QGSThreadPool::setExpiryTimeout(const quint32 msecs)
{
	mExpiryTimeout = msecs;
	return *this;
}

quint32 QGSThreadPool::getExpiryTimeout()
{
	QMutexLocker mutexLocker(&mMutex);

	return mExpiryTimeout;
}

quint32 QGSThreadPool::getMaxThreadCount()
{
	QMutexLocker mutexLocker(&mMutex);

	return mMaxThreadCount;
}

quint32 QGSThreadPool::getMinThreadCount()
{
	QMutexLocker mutexLocker(&mMutex);

	return mMinThreadCount;
}

quint32 QGSThreadPool::getActiveThreadCount()
{
	quint32 ret(0);

	QMutexLocker mutexLocker(&mMutex);
	for (auto & thread : mThreadList)
	{
		if (thread->mActive)
		{
			ret++;
		}
	}

	return ret;
}

int QGSThreadPool::getThreadListSize()
{
	QMutexLocker mutexLocker(&mMutex);

	return mThreadList.size();
}

void QGSThreadPool::run()
{
	init();

	while (true)
	{
		//QGSTaskThread::msleep(mExpiryTimeout);

		//任务分配
		mMutex.lock();
		if (mTaskQueue.size())
		{
			bool allBusy(true);
			for (auto & thread : mThreadList)
			{
				if (!thread->mActive && !thread->mTask && mTaskQueue.size())
				{
					auto * newTask(mTaskQueue.front());

					mTaskQueue.pop_front();
					newTask->moveToThread(thread);
					thread->mTask = newTask;
					//qDebug() << "Thread:" << thread << " new task:" << newTask << " added!";
					allBusy = false;
				}
			}

			if (allBusy)
			{
				//增加线程
				if (mThreadList.size() < mMaxThreadCount)
				{
					auto * newThread(new QGSTaskThread(this));
					if (newThread)
					{
						//qDebug() << "Thread:" << newThread << " added!";
						QObject::connect(newThread, &QGSTaskThread::taskStarted, this, &QGSThreadPool::taskStarted);
						QObject::connect(newThread, &QGSTaskThread::taskFinished, this, &QGSThreadPool::taskFinished);
						newThread->start();

						mThreadList.push_back(newThread);
					}
				}
			}

		}
		mMutex.unlock();

	}
}

void QGSThreadPool::init()
{
	const auto gendThreadCount(mMinThreadCount);
	for (quint32 i = 0; i < gendThreadCount; i++)
	{
		auto * newThread(new QGSTaskThread(this));
		if (!newThread)
		{
			//exception
		}
		QObject::connect(newThread, &QGSTaskThread::taskStarted, this, &QGSThreadPool::taskStarted);
		QObject::connect(newThread, &QGSTaskThread::taskFinished, this, &QGSThreadPool::taskFinished);
		newThread->start();

		mThreadList.push_back(newThread);
	}

	mTimer.moveToThread(this);
	mTimer.singleShot(mExpiryTimeout, this, &QGSThreadPool::adjust);

}

void QGSThreadPool::adjust()
{

	//获取成员变量
	mMutex.lock();

	const bool releaseThreads(mReleaseThreads);
	const int taskQueueSize(mTaskQueue.size());

	mMutex.unlock();

	mAttribMutex.lock();

	const quint32 minThreadCount(mMinThreadCount);
	const quint32 maxThreadCount(mMaxThreadCount);
	const int threadListSize(mThreadList.size());

	mAttribMutex.unlock();

	/*
	//增加线程
	if ((taskQueueSize > minThreadCount&&threadListSize < maxThreadCount) || threadListSize < maxThreadCount)
	{
		quint32 threadAddedCount(qMin(static_cast<quint32>(QGSTaskThread::idealThreadCount()),maxThreadCount - threadListSize));

		for (int i = 0; i < threadAddedCount; i++)
		{
			auto * newThread(new QGSTaskThread(this));
			if (newThread)
			{
				//qDebug() << "Thread:" << newThread << " added!";
				QObject::connect(newThread, &QGSTaskThread::taskStarted, this, &QGSThreadPool::taskStarted);
				QObject::connect(newThread, &QGSTaskThread::taskFinished, this, &QGSThreadPool::taskFinished);
				newThread->start();

				mMutex.lock();
				mThreadList.push_back(newThread);
				mMutex.unlock();

			}
		}
	}
	*/
	if (mTaskQueue.size() < threadListSize&&threadListSize > minThreadCount)
	{
		mMutex.lock();
		int i(0);
		for (auto it = mThreadList.begin(); it != mThreadList.end() && i < minThreadCount; it++, i++)
		{
			if (!(*it)->mActive && !(*it)->mTask)
			{
				(*it)->exit(0);

				mMutex.unlock();

				(*it)->wait();

				mMutex.lock();
				//qDebug() << "Thread:" << (*it) << " released!";
				mThreadList.erase(it);
			}
		}
		mMutex.unlock();

	}

	mTimer.singleShot(mExpiryTimeout, this, &QGSThreadPool::adjust);
}

/*
void QGSThreadPool::releaseThreads()
{
	mMutex.lock();
	mReleaseThreads = true;
	mMutex.unlock();
}
*/

/*
void QGSThreadPool::run()
{
	const auto PregeneratedThreadCount(qMin(static_cast<quint32>(QGSTaskThread::idealThreadCount()),mMaxThreadCount));
	for (quint32 i = 0; i < PregeneratedThreadCount; i++)
	{
		auto * newThread(new QGSTaskThread{this});
		if (!newThread)
		{
			//exception
		}
		newThread->start();
		mCurrentThreadCount++;
	}

	while (!mReleaseThreads)
	{
		QGSTaskThread::msleep(mExpiryTimeout);

		//获取成员变量
		mMutex.lock();
		const bool taskQueueBlock(mTaskQueueBlock);
		const bool releaseThreads(mReleaseThreads);
		const int taskQueueSize(mTaskQueue.size());
		mMutex.unlock();
		mAttribMutex.lock();
		const quint32 minThreadCount(mMinThreadCount);
		const quint32 maxThreadCount(mMaxThreadCount);
		const quint32 activeThreadCount(mActiveThreadCount);
		const quint32 currentThreadCount(mCurrentThreadCount);
		mAttribMutex.unlock();

		//增加线程
		if (taskQueueSize > minThreadCount&&currentThreadCount < maxThreadCount)
		{
			mMutex.lock();
			quint32 threadAddedCount(qMin(static_cast<quint32>(QGSTaskThread::idealThreadCount()),maxThreadCount - currentThreadCount));

			for (int i = 0; i < threadAddedCount; i++)
			{
				auto * newThread(new QGSTaskThread(this));
				if (newThread)
				{
					mAttribMutex.lock();
					mCurrentThreadCount++;
					mAttribMutex.unlock();

					newThread->start();

					//qDebug() << "Thread:" << newThread << " added!";
				}
			}
			mMutex.unlock();

		}
		else if (activeThreadCount * 4 < currentThreadCount&&currentThreadCount > minThreadCount)
		{
			mMutex.lock();
			quint32 threadReleasedCount(qMin(static_cast<quint32>(QGSTaskThread::idealThreadCount()),activeThreadCount));

			mWaitReleasedThreadCount++;
			mTaskQueueStartCondition.wakeOne();
			mMutex.unlock();

		}
	}
}
*/