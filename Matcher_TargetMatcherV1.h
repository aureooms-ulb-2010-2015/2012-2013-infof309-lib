#ifndef MATCHER_TARGETMATCHERV1_H
#define MATCHER_TARGETMATCHERV1_H

#include "Target_CondensationTargetV1.h"
#include "Matcher_TargetMatcher.h"

#include "RobustMatcher.h"

class TargetMatcherV1 : public TargetMatcher<int, CondensationTargetV1>{
public:
	typedef CondensationTargetV1 Target;
private:
	RobustMatcher matcher;
public:
	TargetMatcherV1();
	virtual ~TargetMatcherV1() throw();
	virtual int computeScore(const Target::Data& a, const Target::Data& b);
};

#endif // MATCHER_TARGETMATCHERV1_H
