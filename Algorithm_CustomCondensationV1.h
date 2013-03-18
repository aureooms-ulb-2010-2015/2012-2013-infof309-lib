#ifndef ALGORITHM_CUSTOMCONDENSATIONV1_H
#define ALGORITHM_CUSTOMCONDENSATIONV1_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Algorithm_CustomCondensationTemplateV1.h"

#include "Matcher_TargetMatcherV2_BinaryMask.h"
#include "Target_CondensationTargetV1.h"

#include <chrono>
#include <vector>
#include <random>
#include "Algorithm_Sub_Tagging.h"

class CustomCondensationV1: public CustomCondensationTemplateV1<TargetMatcherV2_BinaryMask>{};

#endif // ALGORITHM_CUSTOMCONDENSATIONV1_H
