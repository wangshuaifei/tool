#ifndef TOKEN_TIMER_H
#define TOKEN_TIMER_H
#include <stdio.h>
#include <sys/timerfd.h>
#include <time.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>


namespace tool {

typedef void (*worker) (void * arg);

class Timer {
public:
	Timer(uint64_t interval_time_us,worker fn, void * arg);

	void init();
	int start();
	

	int get_epoll_fd();
	int get_timer_fd();
	worker get_fn();
	void * get_fn_arg();

private:

	void init_epoll_fd();

	void init_timer_fd();
	void add_fd_to_epoll(int fd);


	static void* run(void * arg);
private:
	uint64_t _interval_time_us;
	worker _fn;
	void * _arg;
	int _epoll_fd;
	int _timer_fd;
    pthread_t _pid;
};




















}














#endif




