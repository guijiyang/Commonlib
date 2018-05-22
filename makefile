##Commonlib库编译脚本

objects = Task.o MessageLoop.o TimerMessageLoop.o Thread.o ThreadPool.o Log.o
Commonlib = ../
#objects := $(wildcard*.o) 
Commonlib.so : $(objects)
	@g++ -shared -fPIC -lpthread -L/usr/local/log4cxx/lib -llog4cxx $(objects) -o Commonlib.so

Task.o : MessageLoop/Task.cpp MessageLoop/Task.h
	@g++ -Wall -g -c -fPIC -std=c++17 -I$(Commonlib) MessageLoop/Task.cpp -o Task.o
MessageLoop.o :  MessageLoop/MessageLoop.cpp MessageLoop/MessageLoop.h MessageLoop/Private/BlockingQueue.h \
	MessageLoop/Task.h Pattern/CSingleton.h Thread/Mutex.h Noncopyable.h CommonGlobal.h 
	@g++ -Wall -g -c -fPIC -std=c++17 -I$(Commonlib) MessageLoop/MessageLoop.cpp
TimerMessageLoop.o : Thread/Private/TimerMessageLoop.cpp Thread/Private/TimerMessageLoop.h \
 	MessageLoop/Private/TimerQueue.h MessageLoop/Task.h Thread/Mutex.h CommonGlobal.h Noncopyable.h
	@g++ -Wall -g -c -fPIC -std=c++17  -I$(Commonlib) Thread/Private/TimerMessageLoop.cpp
Thread.o : Thread/Thread.cpp Thread/Thread.h Thread/Mutex.h CommonGlobal.h
	@g++ -Wall -g -c -fPIC -std=c++17  -I$(Commonlib) Thread/Thread.cpp
ThreadPool.o : Thread/ThreadPool.cpp Thread/ThreadPool.h MessageLoop/Task.h Thread/Private/ThreadGroup.h \
	MessageLoop/Private/BlockingQueue.h Thread/Private/TimerMessageLoop.h CommonGlobal.h Pattern/CSingleton.h
	@g++ -Wall -g -c -fPIC -std=c++17  -I$(Commonlib) Thread/ThreadPool.cpp
Log.o : Log/Log.cpp Log/Log.h
	@g++ -Wall -g -c -fPIC -std=c++17 -I$(Commonlib) -I/usr/local/log4cxx/include Log/Log.cpp
.PHONY : clean
clean : 
	rm -f Commonlib.so $(objects)
	#rm -f *.so *.o
