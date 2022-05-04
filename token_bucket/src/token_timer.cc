#include "token_timer.h"
#include <cstring>
#define US_TO_SEC  (1000 * 1000UL)
#define EPOLL_LISTEN_CNT        256
#define EPOLL_LISTEN_TIMEOUT	500



namespace tool {

Timer::Timer(uint64_t interval_time_us, worker fn, void * arg): _interval_time_us(interval_time_us), _fn(fn),_arg(arg),
	   _epoll_fd(0), _timer_fd(0) {
	assert(interval_time_us > 0);
	

}

void Timer::init() {
	init_timer_fd();
	init_epoll_fd();
	assert(_timer_fd > 0);
	add_fd_to_epoll(_timer_fd);

}

int Timer::start() {
	int ret = 0;
	ret = pthread_create( &_pid, NULL, run, (void *)this);
	if (ret < 0) {
		printf("create timer fd failed\n");
		return ret;
	}

	ret = pthread_join (_pid, NULL );
	if ( ret < 0) {
		printf("error join thread.");
		return ret;
	}

    return 0;
}

int Timer::get_epoll_fd() {
	return _epoll_fd;
}

int Timer::get_timer_fd() {
	return _timer_fd;
}

worker Timer::get_fn() {
	return _fn;
}

void* Timer::get_fn_arg() {
	return _arg;
}

void Timer::init_epoll_fd() {
	int fd = 0;
	int ret = 0;
	fd = epoll_create(EPOLL_LISTEN_CNT);
	if (fd < 0) {
		printf("create timer fd failed\n");
		return;
	}

	_epoll_fd = fd;
	printf("epoll fd init %d success\n", _epoll_fd);


}

void Timer::init_timer_fd() {
	int ret = 0;
	int fd = 0;
	fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (fd < 0) {
		printf("create timer fd failed\n");
		return;
	}

	struct itimerspec timebuf;
	uint64_t sec = _interval_time_us / US_TO_SEC;
	uint64_t tv_nsec =(_interval_time_us % US_TO_SEC ) * 1000;
	timebuf.it_interval.tv_sec = sec;
	timebuf.it_interval.tv_nsec = tv_nsec;
	timebuf.it_value.tv_sec = sec;
	timebuf.it_value.tv_nsec = tv_nsec;

	ret = timerfd_settime(fd, 0, &timebuf, NULL);
	if (ret < 0) {
		printf("set timer fd failed\n");
		return;
	}
	_timer_fd = fd;
	printf("timer fd init %d sec %lu nsec %lu success\n", _timer_fd, sec, tv_nsec);

}

void Timer::add_fd_to_epoll(int fd) {
	int ret = 0;
	struct epoll_event events;
	events.data.fd = fd;
	events.events = EPOLLIN|EPOLLET;

	ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &events);
	if (ret < 0) {
		printf("epoll_ctl failed\n");
		return;
	}
	printf("epoll_ctl add fd %d success\n", fd);
}


 void* Timer::run(void * arg) {
	Timer *timer = (Timer *)arg;
	int fd_cnt = 0;
	struct epoll_event events[EPOLL_LISTEN_CNT];  
	
	while(1) {
		memset(events, 0, sizeof(events));
		fd_cnt = epoll_wait(timer->get_epoll_fd(), events, EPOLL_LISTEN_CNT, 
			EPOLL_LISTEN_TIMEOUT);
		if ( fd_cnt < 0) {
			printf("epoll wait falied\n");
			return nullptr;
		}
		
		//printf("epoll_wait get fd_cnt %d success\n", fd_cnt);
		for (int i = 0; i < fd_cnt; i++) {
			if (events[i].events & EPOLLIN) {
				//printf("data fd %d\n", events[i].data.fd);
				if (events[i].data.fd == timer->get_timer_fd()) {
					uint64_t exp = 0;
					read(timer->get_timer_fd(), &exp, sizeof(uint64_t)); 
					worker fn = timer->get_fn();
					void * arg = timer->get_fn_arg();
					if (fn != nullptr) {
						fn(arg);
				 	}
				}
		    }
		}
	}
	return nullptr;
}


}








