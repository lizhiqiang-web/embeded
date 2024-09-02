#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//现在我想有一个功能
//里面会一直打印一句话  当我们需要这个参数的时候可以将参数给传过来
void *func(void * arg)
{
	pthread_detach(pthread_self());//线程还是需要回收资源 
								//因此我们在这里让这个线程分离
	
	int * p = (int *)arg;//在使用的这个地址的时候  这个地址是不能释放的
	
	while(1)
	{
		printf("func %d\n",(*p)++);
		
		sleep(1);
	}
	
}

//由于c语言里面是顺序执行，因此没有办法同时跑两个while
//这个时候就需要两个线程去运行
//程序在执行的时候有一个默认的线程叫主线程 ---main函数
//现在还需要开辟一个线程
//线程开辟出来之后谁先被调度  具体要看系统怎么调度的

#if 0
void pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
	//依照这个attr来开辟这个线程  当attr为NULL的时候
	//attr就直接赋值默认属性  线程开辟完毕会得到这个id
	//*thread = id;
	start_routine(arg);//通过函数指针去调用这个函数
					//传过来的地址是哪个函数的地址  这里就会调用这个函数
	//由于下面传过来的是func的地址
	//这个函数指针的调用函数就相当于是 func(arg);
	//而这个arg获取的是flag的地址
	//因此这个函数的调用就相当于是  fun(&flag)
}
#endif
pthread_t thread;//保存线程id
void hehe()
{
	//static静态修饰   修饰局部变量的时候将这个局部变量的生存期延长到随进程持续而持续
	static int flag = 1024;//局部变量除了局部之后就会释放  因此就不能给别人去使用了
	
	if(pthread_create(&thread,NULL,func,(void *)&flag) != 0)//这种时候传flag就不行了
	{
		perror("pthread_create error");
	}
	
}





int main()
{
	hehe();
	while(1)
	{
		printf("main\n");
		sleep(1);
	}
	
	
	
	return 0;
}





