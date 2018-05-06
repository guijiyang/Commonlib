##Commonlib库编译脚本

objects = Task.o MessageLoop.o TimerMessageLoop.o Thread.o ThreadPool.o
Commonlib = ../
#objects := $(wildcard*.o) 
Commonlib.so : $(objects)
	gcc63 -shared -o Commonlib.so $(objects)

Task.o: MessageLoop/Task.cpp MessageLoop/Task.h
	gcc63 -Wall -g -c -lpthread -shared -fPIC -std=c++14 -I $(Commonlib) MessageLoop/Task.cpp 
MessageLoop.o:  MessageLoop/MessageLoop.cpp MessageLoop/MessageLoop.h MessageLoop/Private/BlockingQueue.h \
	MessageLoop/Task.h Pattern/CSingleton.h Thread/Mutex.h Noncopyable.h CommonGlobal.h 
	gcc63 -Wall -g -c -lpthread -shared -fPIC -std=c++14 -I $(Commonlib) MessageLoop/MessageLoop.cpp
TimerMessageLoop.o: Thread/Private/TimerMessageLoop.cpp Thread/Private/TimerMessageLoop.h \
 	MessageLoop/Private/TimerQueue.h MessageLoop/Task.h Thread/Mutex.h CommonGlobal.h Noncopyable.h
	gcc63 -Wall -g -c -lpthread -shared -fPIC -std=c++14  -I $(Commonlib) Thread/Private/TimerMessageLoop.cpp
Thread.o: Thread/Thread.cpp Thread/Thread.h Thread/Mutex.h CommonGlobal.h
	gcc63 -Wall -g -c -lpthread -shared -fPIC -std=c++14  -I $(Commonlib) Thread/Thread.cpp
ThreadPool.o: Thread/ThreadPool.cpp Thread/ThreadPool.h MessageLoop/Task.h Thread/Private/ThreadGroup.h \
	MessageLoop/Private/BlockingQueue.h Thread/Private/TimerMessageLoop.h CommonGlobal.h Pattern/CSingleton.h
	gcc63 -Wall -g -c -lpthread -shared -fPIC -std=c++14  -I $(Commonlib) Thread/ThreadPool.cpp
.PHONY : clean
clean : 
	rm -f Commonlib.so $(objects)
	#rm -f *.so *.o
