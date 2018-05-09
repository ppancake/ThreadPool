/*
 * ThreadPool.h
 *
 *  Created on: Apr 12, 2018
 *      Author: zwk
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include<pthread.h>
#include<string>
#include<queue>
#include<set>
#include "Condition.h"
using std::string;
using std::queue;
using std::set;

class Task
{
public:
    Task(int fd) :sock(fd) {}
protected:
    int sock;
};

class ThreadPool
{
public:
	 void initThreadPool();//初始化线程池
	 void destroyThreadPool();//销毁线程池
	 friend void *threadRoutine(void* arg); //线程入口函数
     void addTask(Task *task);//添加任务??之前放到析构函数后面，：：居然不显示？？
	 ThreadPool(int initNum, int maxNum):initThreadNum(initNum),nowThreadNum(0),idleThreadNum(0),maxThreadNum(maxNum),isDestroy(false)
     {
		 initThreadPool();
     }
	 ~ThreadPool()
	 {
		 destroyThreadPool();
	 }

private:
	Condition cond; //锁和条件变量
	int initThreadNum; //初始线程池数量
    int nowThreadNum; //当前线程池线程数量
	int idleThreadNum; //当前空闲线程数量
	int maxThreadNum; //最大允许的线程数量
	queue<Task *> taskQueue; //任务队列
	set<pthread_t> threadIDs; //创建线程的ID集合..
	bool isDestroy; //线程销毁通知
};



#endif /* THREADPOOL_H_ */
