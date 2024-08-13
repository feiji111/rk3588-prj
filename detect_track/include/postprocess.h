#ifndef RK3588_DETECT_TRACK_POSTPROCESS_H_
#define RK3588_DETECT_TRACK_POSTPROCESS_H_

#include <stdint.h>
#include <vector>
#include "common.h"

#define PROP_BOX_SIZE (5 + OBJ_CLASS_NUM)

typedef struct _BOX_RECT
{
    int left;
    int right;
    int top;
    int bottom;
} BOX_RECT;

int post_process(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h, int model_in_w,
                 float conf_threshold, float nms_threshold, BOX_RECT pads, float scale_w, float scale_h,
                 std::vector<int32_t> &qnt_zps, std::vector<float> &qnt_scales,
                 detect_result_group_t *group);
#endif //RK3588_DETECT_TRACK_POSTPROCESS_H_
