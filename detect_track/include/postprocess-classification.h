#ifndef RKNN_INCLUDE_POSTPROCESS_H_
#define RKNN_INCLUDE_POSTPROCESS_H_

//C system headers
#include <stdint.h>

//C++ standard library headers

//other libraries' headers
#include "rknn_api.h"

//project headers
#include "inference-classification.h"

int post_process(rknn_app_context_t *app_ctx, rknn_output *outputs);

#endif //RKNN_INCLUDE_POSTPROCESS_H_
