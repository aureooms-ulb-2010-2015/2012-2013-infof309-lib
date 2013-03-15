#ifndef STREAM_H
#define STREAM_H

template <typename T>
class Stream{
public:
	virtual ~Stream(){}
	virtual void jumpTo(unsigned long int)=0;
	virtual int next(T&)=0;
};

#endif // STREAM_H
