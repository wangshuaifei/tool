#include "token_timer.h"
#include <stdio.h>

void get_cur_time(void *arg) {
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
    tm now_time;
    localtime_r(&time.tv_sec, &now_time);
    char current[1024];
    sprintf(current, "%04d%02d%02d%02d:%02d:%02d", now_time.tm_year + 1900, now_time.tm_mon+1, now_time.tm_mday,      
		       now_time.tm_hour, now_time.tm_min, now_time.tm_sec);
	printf("cur time:%s\n", current);
}



int main() {
	uint64_t interval_us = 1000*1000;//1s
	tool::Timer timer(interval_us,get_cur_time, (void *)nullptr);
	timer.init();
	timer.start();

	printf("timer end\n");
	return 0;
}




