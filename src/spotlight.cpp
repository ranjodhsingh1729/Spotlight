/**
 * @file spotlight.cpp
 * @author Ranjodh Singh
 *
 * @brief SPOTLIGHT.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#include "spotlight/config/file.hpp"
#include <chrono>
#include <iostream>
#include <getopt.h>

#include <spotlight/cli/parse.hpp>
#include <spotlight/config/config.hpp>
#include <spotlight/config/defaults.hpp>
#include <spotlight/v4l2/v4l2_cam.hpp>
#include <spotlight/v4l2/v4l2_vcam.hpp>
#include <spotlight/pipeline/pipeline.hpp>


int main(int argc, char **argv)
{
  // Default Configurations
  spotlight::PipelineConfig cfg;

  // Conf File
  spotlight::parse_config_file(CONF_FILE, cfg);
  
  // Get Configuration from CLI
  spotlight::parse_args(argc, argv, cfg);

  // Initialize Pipeline
  spotlight::Pipeline pipeline(cfg);
  spotlight::V4L2Camera cam(cfg.in_dev, cfg.InpConfig());
  spotlight::V4L2VirtualCamera vcam(cfg.out_dev, cfg.OutConfig());

  // Allocate Required Buffers
  // TODO: Use you own allocator
  std::vector<uint8_t> vec_inp_u;
  std::vector<uint8_t> vec_out_u;
  vec_inp_u.resize(3 * cfg.InpPixels());
  vec_out_u.resize(3 * cfg.OutPixels());
  uint8_t *inp_u = vec_inp_u.data();
  uint8_t *out_u = vec_out_u.data();

  try
  {
    using clock = std::chrono::steady_clock;
    for (;;)
    {
      auto start = clock::now();
      cam.invoke(inp_u);
      pipeline.invoke(inp_u, out_u);
      vcam.invoke(out_u);
      std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count() << " ms" << std::endl;
    }
  }
  catch (...)
  {
    throw;
  }

  return 0;
}
