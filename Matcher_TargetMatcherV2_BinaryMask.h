#ifndef MATCHER_TARGETMATCHERV2_BINARYMASK_H
#define MATCHER_TARGETMATCHERV2_BINARYMASK_H


#include "Target_CondensationTargetV1.h"
#include "Matcher_TargetMatcher.h"

class TargetMatcherV2_BinaryMask : public TargetMatcher<int, CondensationTargetV1>{
public:
	typedef CondensationTargetV1 Target;

public:
	TargetMatcherV2_BinaryMask();
	virtual ~TargetMatcherV2_BinaryMask() throw();
	virtual int computeScore(const Target::Data& a, const Target::Data& b);
};

#endif // MATCHER_TARGETMATCHERV2_BINARYMASK_H
