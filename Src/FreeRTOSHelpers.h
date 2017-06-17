#ifndef _FREERTOSHELPERS_H_
#define _FREERTOSHELPERS_H_

class MutexLocker
{
public:
	MutexLocker(SemaphoreHandle_t mtx)
	{
		mutex = mtx;
		xSemaphoreTake(mutex, portMAX_DELAY);
	}
	
	~MutexLocker()
	{
		xSemaphoreGive(mutex);
	}

private:
	SemaphoreHandle_t mutex;	
};

// A "base" class for threads. Implements common functionality (thread start) and holds
// thread stack and control structures.
//
// Usage:
// Thread<MyThread> myThread;
// myThread.start()
//
// The implementation class must implement run() method and define couple constants
template<class T>
class Thread : public T
{
	// Structure that will hold the TCB of the task being created
	StaticTask_t xTaskTCB;
	// Buffer that the task being created will use as its stack.  Note this is
	// an array of StackType_t variables.  The size of StackType_t is dependent on
	// the RTOS port
	StackType_t xStack[ T::THREAD_STACK_SIZE ];

	// A thread function forwards implementation to a corresponding run() method
	static void vThreadFunc( void * pvParameters )
	{
		static_cast<Thread<T> *>(pvParameters)->run();
	}

public:
	// Thread start function. Set's up a thread with defined parameters
	void start()
	{
		xTaskCreateStatic(
			Thread::vThreadFunc,	// Thread entry point
			T::THREAD_NAME,			// Text name for the task.
			T::THREAD_STACK_SIZE,   // Number of indexes in the xStack array.
			( void * )this,			// Parameter passed into the task.
			T::THREAD_PRIORITY,		// Priority at which the task is created.
			xStack,					// Array to use as the task's stack.
			&xTaskTCB );			// Variable to hold the task's data structure.
	}
};


#endif //_FREERTOSHELPERS_H_
