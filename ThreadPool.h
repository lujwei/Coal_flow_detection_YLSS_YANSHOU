#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
/*
< thread>: �� C++ 11�������ԣ���Ҫ�������̶߳���std::thread�Ĺ��졣
< mutex > : C++ 11�����ԣ���Ҫ��������Mutex����Ĺ��죬��Ҫ��std::mutex��
< condition_variable > : C++ 11�����ԣ� �������߳��г��õ���������������������notify_one��wait��wait_for�ȵȡ�
< future > : C++ 11�����ԣ����Ի�ȡ�첽����Ľ����������ʵ��ͬ��������std::sync��std::future��
< functional > : C++ 11������һЩ�����ԣ�����˵����ʵ�ֺ���������İ󶨣���bind()������
*/


class ThreadPool {
public:
	ThreadPool(size_t);
	template<class F, class... Args>                                  //����ģ��
	auto enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;     //ͨ��std::future���Է������A���͵��첽����Ľ���� result_of��ȡ��someTask��ִ�н�������͡�
	~ThreadPool();
private:
	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;
	// the task queue
	std::queue< std::function<void()> > tasks;

	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};

// the constructor just launches some amount of workers
/*
1 inline: ���ƺ궨�壬�Ὠ��������Ѻ�����ֱ��չ������ʽ����Ŀ��������������ջ���õ���ʽ��ͨ���������ڴ���Ƚϳ��������ڳ���ѭ��ʱ����ʹ����������������ɴ������͡�
2 workers.emplace_back([this]{��});
emplace_back()��push_back()���ƣ�����ǰ�߸��ʺ��������ݶ�����Ϊ�����Ա��������Ϊ����������ʱ�ڿ�����Ա�ϵĿ�����
����emplace_back()��һ��lambda���ʽ[this]{��}��lambda���ʽ�������һ����������(��û�к������ĺ���)��ͨ����ʽΪ[�����б�](�����б�)->return ��������{������}�����ڱ������е�lambda���ʽ����Ϊһ���̷߳���workers[]�С�
����߳��Ǹ�for(;;)ѭ����
3 for(;;)����: ÿ��ѭ����������һ��std::function< void()> task��task��һ�����Ա���װ�ɶ���ĺ������ڴ���Ϊ��С����λ��Ȼ����{}�����һ��������
4 ����������: ������������н�����һЩ�߳��������߳�״̬���жϡ�
5 lock(this->queue_mutex): ��������ԭ��
6 condition.wait(lock, [this]{��}): ʹ��ǰ�߳̽�������״̬�� ���ڶ�������Ϊfalseʱ��wait()��������ǰ�̣߳�Ϊtrueʱ����������ڱ����е��������ǣ����̳߳����л��������б�Ϊ��ʱ���߳̽�������̬��
Ȼ���жϣ�����̳߳����л��������б�Ϊ����������������������˳����[this]{��}�̺߳�����
std::move()���ƶ����캯�����൱��Ч�ʸ��ߵĿ������캯�������tasks[]������еĵ�һ�������ջ��
7 �뿪������: Ȼ��ִ��task()����ǰһ��ѭ��������
*/
inline ThreadPool::ThreadPool(size_t threads)
	: stop(false)
{
	for (size_t i = 0; i < threads; ++i)
		workers.emplace_back(
			[this]
	{
		for (;;)
		{
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(this->queue_mutex);
				this->condition.wait(lock,
					[this] { return this->stop || !this->tasks.empty(); });
				if (this->stop && this->tasks.empty())
					return;
				task = std::move(this->tasks.front());
				this->tasks.pop();
			}

			task();
		}
	}
	);
}

// add new work item to the pool
/*
1 using �� = typename ��; ��������typedef����return_type����Ϊһ��result_of< F(Args��)>::type���ͣ�������F(Args��)�ķ���ֵ���͡�
2 make_shared < packaged_task < >>(bind()): ���Ǹ��ӵ�Ƕ�ס�
make_shared : ����()������Ϊ<>���ڴ�
packaged_task : ��������,����������return_type
bind : �󶨺���f, ����Ϊargs��
forward : ʹ()ת��Ϊ<>��ͬ���͵���ֵ����ֵ����
����˵����仰�൱�ڰѺ���f�����Ĳ���args�����Ϊһ��ģ���ڶ����task�����ں���������
3 res = task->get_future(): ��ģ�庯���ķ�������һ�£��Ǻ����첽��ִ�н����
4 ��������: ����һ������ԭ��lock()��
Ȼ���Ǹ��쳣�������ֹͣ�Ļ��׳�һ������ʱ�쳣��
����������б�����������[task](){(*task)();}��
5 condition.notify_one(): ���һ�����ڵȴ����ѵ��̵߳�����̬��
6 �����첽���res
*/
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	condition.notify_one();
	return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread &worker : workers)
		worker.join();
}

#endif
