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
#define MODE                     PipelineMode::BLUR
#define N_THREADS                1

#define IN_DEV                   "/dev/video0"
#define IN_FMT                   V4L2_PIX_FMT_MJPEG
#define IN_W                     1280
#define IN_H                     720
#define IN_FPS                   30.0

#define OUT_DEV                  "/dev/video10"
#define OUT_FMT                  V4L2_PIX_FMT_MJPEG
#define OUT_W                    1280
#define OUT_H                    720
#define OUT_FPS                  30.0

#define BG_IMG                   "assets/background.png"

#define MASK_FILTER_RADIUS       2
#define EDGE_FILTER_RADIUS       3
#define BLUR_FILTER_RADIUS       3
#define BLUR_FILTER_COMPONENTS   2
#define BLUR_FILTER_TRANSITION   0.4

#define MJPEG_Q                  95


// Unconfigurable (For Now)
#define CONF_FILE                "/etc/spotlight.conf"

#define FACE_MODEL              "models/face/face_smpl_320p.tflite"
#define FACE_TOPK                10
#define SCORE_THRESHOLD          0.8
#define IOU_THRESHOLD            0.2
#define TEMPORAL_ALPHA           0.9
#define JERK_THRESHOLD           0.3
#define FRAME_PAD_U              0.50
#define FRAME_PAD_D              0.25
#define FRAME_PAD_L              0.50
#define FRAME_PAD_R              0.50

#define SEGM_MODEL               "models/segm/segm_lite_v681.tflite"


#endif // DEFAULTS_HPP
