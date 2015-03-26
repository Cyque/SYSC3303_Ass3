#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include <native/mutex.h>
#include <native/task.h>
#include <native/timer.h>

#define SECOND 1000000000
#define LOG_SIZE 10000

RT_MUTEX m_accel, m_gyro;
RT_TASK t_accel, t_gyro, t_fuse;

char _log[LOG_SIZE];
int sampAccel[5];
int sampGyro[5];
int fusionValue = 0;

int getGyroValue(){
	return 5;
}

int getAccelValue(){
	return 2;
}

int logMsg(const char *str){
	if(strlen(str) + strlen(_log) > LOG_SIZE - 10){
		return 0;
	}

	sprintf(_log + strlen(_log), "%s", str);
	return 1;
}
	
void accel(void *cookie){
	rt_task_set_periodic(NULL, TM_NOW, SECOND/30);

	while(1){
		rt_mutex_acquire(&m_accel,TM_INFINITE);

		int i = 3;
		while(i>=0){
			sampAccel[i+1] = sampAccel[i];
			i--;
		}
		sampAccel[0] = getAccelValue();

		rt_mutex_release(&m_accel);

		if(!logMsg("a ")){
			rt_task_delete(NULL);
			return;
		}

		rt_task_wait_period(NULL);
	}
}

void gyro(void *cookie){
        rt_task_set_periodic(NULL, TM_NOW, SECOND/300);

	while(1){
		rt_mutex_acquire(&m_gyro,TM_INFINITE);

		int i = 3;
		while(i>=0){
			sampGyro[i+1] = sampGyro[i];
			i--;
		}

		sampGyro[0] = getGyroValue();
		rt_mutex_release(&m_gyro);
		
		if(!logMsg("g ")){
			rt_task_delete(NULL);
			return;
		}

		rt_task_wait_period(NULL);
	}
}

void fusion(void *cookie){
        rt_task_set_periodic(NULL, TM_NOW, SECOND/60);

	while(1){

		rt_mutex_acquire(&m_accel,TM_INFINITE);
		rt_mutex_acquire(&m_gyro,TM_INFINITE);
	
		fusionValue = 0;
		int i = 0;
		while(i < 5){
			fusionValue = (sampGyro[i] + sampAccel[i])/(2*5);
			i++;
		}

		rt_mutex_release(&m_gyro);
		rt_mutex_release(&m_accel);

		if(!logMsg("fusion\n============================================\n")){
			rt_task_delete(NULL);
			return;
		}

		rt_task_wait_period(NULL);
	}
}

void cleanup(void){
	rt_mutex_delete(&m_gyro);
	rt_mutex_delete(&m_accel);
	rt_task_delete(&t_accel);
	rt_task_delete(&t_gyro);
	rt_task_delete(&t_fuse);
}

void catch_signal(int sig)
{
	printf("%s\n\nSIGNAL CAUGHT", _log);
	cleanup();
}

int main(int argc, char *argv[]){
	int err;

	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	sprintf(_log, "---------\n");

	mlockall(MCL_CURRENT|MCL_FUTURE);
		
	err = rt_mutex_create(&m_accel, "accelerator mutex");
	err = rt_mutex_create(&m_gyro, "gyro mutex");

	err = rt_task_create(&t_accel, "accelerometer task", 0, 99, T_JOINABLE);
	err = rt_task_create(&t_gyro, "gyroscope task", 0, 99, T_JOINABLE);	
	err = rt_task_create(&t_fuse, "fusion task", 0, 99, T_JOINABLE);

	sprintf(_log+strlen(_log), "created mutexes and tasks\n");

	if(err != 0){
		printf("Error %i\n", err);
		return err;
	}

	sprintf(_log+strlen(_log), "starting tasks...\n");

	rt_task_start(&t_accel, &accel, NULL);
	rt_task_start(&t_gyro, &gyro, NULL);
	rt_task_start(&t_fuse, &fusion, NULL);

	rt_task_join(&t_accel);
	rt_task_join(&t_gyro);
	rt_task_join(&t_fuse);

	cleanup();

	printf("%s", _log);

	return 0;
}
