/*
 * ThreadPool.cpp
 *
 *  Created on: Apr 21, 2018
 *      Author: zwk
 */

#include <iostream>
#include <time.h>
#include "ThreadPool.h"
#include <errno.h>
#include <cstdlib>
#include <pthread.h>

using std::cout;
using std::endl;
const time_t maxWaitTime=5;

void *threadRoutine(void* arg);
//初始化线程池
void ThreadPool::initThreadPool()
{
    pthread_t id=0;
    int ret=0;
    this->cond.lock();//创建线程时要加锁
    for(int i=0;i<this->initThreadNum;i++)
    {
    	ret=pthread_create(&id,NULL,&threadRoutine,(void *)this);//id会引用改成现在的id值
    	if(ret==0)//返回0,线程创建成功，存储线程ID
    	{
    		this->threadIDs.insert(this->threadIDs.end(),id);
    		nowThreadNum++;
    	}
    }
    this->cond.unlock();
}
void ThreadPool::destroyThreadPool()//销毁线程池
{
    this->cond.lock();
    if(!isDestroy)
    	isDestroy=true;
    if(this->idleThreadNum>0)
    	this->cond.broadcast();
    if(this->nowThreadNum>0)
    {
    	while(this->nowThreadNum>0)
    	{
    		this->cond.wait();
    	}
    }
    //根据线程id销毁
    auto it=this->threadIDs.begin();
    for(;it!=this->threadIDs.end();it++)
    {
    	pthread_join(*it,nullptr);
    }
}

void *threadRoutine(void* arg)//线程入口函数
{
    ThreadPool *threadpool=(ThreadPool *)arg;
    timespec timeOut = {0};
    int flag=1;
    while(flag)//这个循环什么时候退出？
    {
    	threadpool->cond.lock();

    	//创建了一个新线程，空闲+1
    	(threadpool->idleThreadNum)++;
    	//等到任务到来或者销毁通知
    	while(threadpool->taskQueue.empty()||!threadpool->isDestroy)
    	{
    		cout<<"Thread:"<<(int)pthread_self()<<"is waiting"<<endl;
    		clock_gettime(CLOCK_REALTIME, &timeOut); //设置超时时间
    		timeOut.tv_sec+=maxWaitTime;
    		if(threadpool->cond.timedwait(timeOut)==ETIMEDOUT)
    		{
    			cout<<"Thread"<<pthread_self() << " waiting timeout." << endl;
    			flag=0;
                break; //超时退出
    		}
    	}
    	cout << "Thread " << pthread_self() << " is working." << endl;
    	(threadpool->idleThreadNum)--;

    	//如果任务队列有任务，进行处理
    	if(!threadpool->taskQueue.empty())
    	{
    		Task *task=threadpool->taskQueue.front();
    		threadpool->taskQueue.pop();//取出第一个任务,弹出

    		//任务执行时可以解锁，让其他线程操作
    		threadpool->cond.unlock();
    		task->taskRun();
    		threadpool->cond.lock();
    	}

    	//如果是销毁信号，并且任务队列为空
    	if(threadpool->isDestroy && threadpool->taskQueue.empty())
    	{
    		threadpool->nowThreadNum--;
    		if(threadpool->nowThreadNum<=0)
    			threadpool->cond.signal();
    		flag=0;
    	}
    	threadpool->cond.unlock();
    }

    //退出线程
    cout << "Thread " << pthread_self() << " exited." << endl;
    return arg;
}
void ThreadPool::addTask(Task *task)//添加任务
{
    if(task==nullptr)
    	return ;
    this->cond.lock();
    this->taskQueue.push(task);
    if(this->idleThreadNum>0)//有空闲线程，给等待线程发出唤醒信号
    {
    	this->cond.signal();
    }
    else//没有空闲线程，看情况添加线程
    {
    	if(this->nowThreadNum<this->maxThreadNum)
    	{
    		pthread_t tid;
    		int ret=pthread_create(&tid,nullptr,&threadRoutine,(void *)(this));
    		if(ret==0)
    		{
    			this->threadIDs.insert(this->threadIDs.end(),tid);
    			nowThreadNum++;
    		}
    	}
    }
    this->cond.unlock();
}


