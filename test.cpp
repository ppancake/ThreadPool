/*
 * test.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: zwk
 */

#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;


//继承抽象类Task，实现其中的RUn方法
class testTask:public Task
{
public:
	testTask(string name,void *printOut): Task(name,printOut){}//调用基类的构造函数
	void taskRun()
	{
		cout<<"任务参数是："<<(char *)taskArg<<endl;//任务参数输出
		cout<<"任务："<<taskName<<"已完成\n";
		sleep(5);
	}
};

int main()
{
    char test[10][30] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    testTask task[10] = {{"Task0", test[0]}, {"Task1", test[1]}, {"Task2", test[2]},
                         {"Task3", test[3]}, {"Task4", test[4]}, {"Task5", test[5]},
                         {"Task6", test[6]}, {"Task7", test[7]}, {"Task8", test[8]},
                         {"Task9", test[9]}};
    ThreadPool testPool(2, 5);
    for (int i = 0; i < 10; i++) {
        testPool.addTask(&task[i]);
    }
    return 0;
}

