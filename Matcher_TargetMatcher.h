#ifndef TARGETMATCHER_H
#define TARGETMATCHER_H

template<typename S, typename T> class TargetMatcher{
public:
	virtual ~TargetMatcher() throw(){}
	virtual S computeScore(const typename T::Data&, const typename T::Data&) = 0;
};

#endif // TARGETMATCHER_H
