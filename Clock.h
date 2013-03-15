#ifndef CLOCK_H
#define CLOCK_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <iostream>

#define DELAY 30000

class Clock: public QThread{
	Q_OBJECT

private:
	unsigned long int _delay;
	QWaitCondition _observer;
	QMutex _lock;
	bool _running;

protected:

	virtual void run(){
		while(true){
			this->_lock.lock();
			while(!this->_running){
				this->_observer.wait(&this->_lock);
			}
			this->_lock.unlock();
			emit tick();
			QThread::usleep(this->_delay);
		}
	}

public:


	Clock(unsigned long int delay = DELAY):_delay(delay),_running(true){}


	void stop(){
		this->_lock.lock();
		this->_running = false;
		this->_lock.unlock();
	}

	void start(){
		this->_lock.lock();
		this->_running = true;
		this->_observer.wakeOne();
		this->_lock.unlock();
	}


signals:
	void tick();

};

#endif // CLOCK_H
