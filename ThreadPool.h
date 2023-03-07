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
< thread>: 是 C++ 11的新特性，主要包含了线程对象std::thread的构造。
< mutex > : C++ 11新特性，主要包含各种Mutex的类的构造，主要是std::mutex。
< condition_variable > : C++ 11新特性， 包含多线程中常用的条件变量的声明，例如notify_one、wait、wait_for等等。
< future > : C++ 11新特性，可以获取异步任务的结果，可用来实现同步。包括std::sync和std::future。
< functional > : C++ 11增加了一些新特性，简单来说可以实现函数到对象的绑定，如bind()函数。
*/


class ThreadPool {
public:
	ThreadPool(size_t);
	template<class F, class... Args>                                  //函数模板
	auto enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;     //通过std::future可以返回这个A类型的异步任务的结果。 result_of获取了someTask的执行结果的类型。
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
1 inline: 类似宏定义，会建议编译器把函数以直接展开的形式放入目标代码而不是以入栈调用的形式。通常函数体内代码比较长或者体内出现循环时不宜使用内联，这样会造成代码膨胀。
2 workers.emplace_back([this]{…});
emplace_back()与push_back()类似，但是前者更适合用来传递对象，因为它可以避免对象作为参数被传递时在拷贝成员上的开销。
这里emplace_back()了一个lambda表达式[this]{…}。lambda表达式本身代表一个匿名函数(即没有函数名的函数)，通常格式为[捕获列表](参数列表)->return 返回类型{函数体}。而在本代码中的lambda表达式是作为一个线程放入workers[]中。
这个线程是个for(;;)循环。
3 for(;;)里面: 每次循环首先声明一个std::function< void()> task，task是一个可以被封装成对象的函数，在此作为最小任务单位。然后用{}添加了一个作用域。
4 作用域里面: 在这个作用域中进行了一些线程上锁和线程状态的判断。
5 lock(this->queue_mutex): 声明上锁原语
6 condition.wait(lock, [this]{…}): 使当前线程进入阻塞状态： 当第二个参数为false时，wait()会阻塞当前线程，为true时解除阻塞；在本例中的条件就是，当线程池运行或者任务列表为空时，线程进入阻塞态。
然后判断，如果线程池运行或者任务列表为空则继续后续操作，否则退出这个[this]{…}线程函数。
std::move()是移动构造函数，相当于效率更高的拷贝构造函数。最后将tasks[]任务队列的第一个任务出栈。
7 离开作用域: 然后执行task()，当前一轮循环结束。
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
1 using … = typename …; 功能类似typedef。将return_type声明为一个result_of< F(Args…)>::type类型，即函数F(Args…)的返回值类型。
2 make_shared < packaged_task < >>(bind()): 又是复杂的嵌套。
make_shared : 开辟()个类型为<>的内存
packaged_task : 把任务打包,这里打包的是return_type
bind : 绑定函数f, 参数为args…
forward : 使()转化为<>相同类型的左值或右值引用
简单来说，这句话相当于把函数f和它的参数args…打包为一个模板内定义的task，便于后续操作。
3 res = task->get_future(): 与模板函数的返回类型一致，是函数异步的执行结果。
4 新作用域: 先是一个加锁原语lock()。
然后是个异常处理，如果停止的话抛出一个运行时异常。
最后，向任务列表插入这个任务[task](){(*task)();}。
5 condition.notify_one(): 解除一个正在等待唤醒的线程的阻塞态。
6 返回异步结果res
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
