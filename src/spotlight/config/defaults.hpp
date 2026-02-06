/**
 * @file defaults.hpp
 * @author Ranjodh Singh
 *
 * @brief DEFAULTS.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef DEFAULTS_HPP
#define DEFAULTS_HPP

#include <linux/videodev2.h>
#include <spotlight/config/config.hpp>


// Configurable 
#define NUM_THREADS              1

#define INP_PATH                 "/dev/video0"
#define INP_FOURCC               V4L2_PIX_FMT_MJPEG
#define INP_WIDTH                1280
#define INP_HEIGHT               720
#define INP_FPS                  30.0

#define OUT_PATH                 "/dev/video10"
#define OUT_FOURCC               V4L2_PIX_FMT_MJPEG
#define OUT_WIDTH                1280
#define OUT_HEIGHT               720
#define OUT_FPS                  30.0

#define BG_IMG_PATH              "assets/background.png"

#define PIPELINE_MODE            PipelineMode::BLUR


// Unconfigurable (For Now)
#define FACE_MODEL_PATH          "models/face/face_smpl_320p.tflite"
#define FACE_TOPK                10
#define SCORE_THRESHOLD          0.8
#define IOU_THRESHOLD            0.2
#define TEMPORAL_ALPHA           0.9
#define JERK_THRESHOLD           0.3
#define FRAME_PAD_TOP            0.50
#define FRAME_PAD_BOTTOM         0.25
#define FRAME_PAD_LEFT           0.50
#define FRAME_PAD_RIGHT          0.50

#define SEGM_MODEL_PATH          "models/segm/segm_lite_v681.tflite"
#define MASK_FILTER_RADIUS       2
#define EDGE_FILTER_RADIUS       3
#define BLUR_FILTER_RADIUS       3
#define BLUR_FILTER_COMPONENTS   2
#define BLUR_FILTER_TRANSITION   0.4

#define MJPEG_QUALITY            95
#define MAX_BG_IMG_SIZE          3 * 15360 * 8640


#endif // DEFAULTS_HPP
