/**
 * @file parse.hpp
 * @author Ranjodh Singh
 *
 * @brief PARSE.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef PARSE_HPP
#define PARSE_HPP

#include <cstdlib>
#include <getopt.h>
#include <string_view>

#include <spotlight/config/config.hpp>
#include <spotlight/config/defaults.hpp>
#include <spotlight/utils/error_utils.hpp>


namespace spotlight {

inline uint32_t get_fourcc(const std::string_view s)
{
  if (s.size() != 4)
    throw_err("FOURCC must be exactly 4 characters");
  return v4l2_fourcc(s[0], s[1], s[2], s[3]);
}

inline spotlight::PipelineConfig parse_args(int argc, char** argv)
{
  spotlight::PipelineConfig cfg{};

  cfg.num_threads = NUM_THREADS;

  cfg.inp_path = INP_PATH;
  cfg.inp_width = INP_WIDTH;
  cfg.inp_height = INP_HEIGHT;
  cfg.inp_fps = INP_FPS;
  cfg.inp_fourcc = INP_FOURCC;

  cfg.out_path = OUT_PATH;
  cfg.out_width = OUT_WIDTH;
  cfg.out_height = OUT_HEIGHT;
  cfg.out_fps = OUT_FPS;
  cfg.out_fourcc = OUT_FOURCC;

  cfg.bg_img_path = BG_IMG_PATH;
  cfg.pipeline_mode = PIPELINE_MODE;

  static struct option long_opts[] = {
    {"threads", required_argument, nullptr, 'j'},

    {"input-path", required_argument, nullptr, 'i'},
    {"input-width", required_argument, nullptr, 2},
    {"input-height", required_argument, nullptr, 3},
    {"input-fps", required_argument, nullptr, 4},
    {"input-fourcc", required_argument, nullptr, 5},

    {"output-path", required_argument, nullptr, 'o'},
    {"output-width", required_argument, nullptr, 7},
    {"output-height", required_argument, nullptr, 8},
    {"output-fps", required_argument, nullptr, 9},
    {"output-fourcc", required_argument, nullptr, 10},

    {"background", required_argument, nullptr, 'b'},
    {"mode", required_argument, nullptr, 'm'},

    {nullptr, 0, nullptr, 0}
  };

  int opt;
  int long_index = 0;
  while (
    (opt = getopt_long(
        argc, argv, "j:i:o:b:m:", long_opts, &long_index)
    ) != -1
  )
  {
    switch (opt)
    {
    case 'j':
        cfg.num_threads = std::atoi(optarg);
        break;
    case 'i':
        cfg.inp_path = optarg;
        break;
    case 2:
        cfg.inp_width = std::atoi(optarg);
        break;
    case 3:
        cfg.inp_height = std::atoi(optarg);
        break;
    case 4:
        cfg.inp_fps = std::atoi(optarg);
        break;
    case 5:
        cfg.inp_fourcc = get_fourcc(optarg);
        break;
    case 'o':
        cfg.out_path = optarg;
        break;
    case 7:
        cfg.out_width = std::atoi(optarg);
        break;
    case 8:
        cfg.out_height = std::atoi(optarg);
        break;
    case 9:
        cfg.out_fps = std::atoi(optarg);
        break;
    case 10:
        cfg.out_fourcc = get_fourcc(optarg);
        break;
    case 'b':
        cfg.bg_img_path = optarg;
        break;
    case 'm':
        if (std::string_view(optarg) == "blur")
        {
            cfg.pipeline_mode = PipelineMode::BLUR;
        }
        else if (std::string_view(optarg) == "image")
        {
            cfg.pipeline_mode = PipelineMode::IMAGE;
        }
        else if (std::string_view(optarg) == "video")
        {
            cfg.pipeline_mode = PipelineMode::VIDEO;
        }
        else
        {
            throw_err("Invalid PipelineMode!!!");
        }
        break;
    default:
        throw_err("Invalid command line argument");
    }
  }

  return cfg;
}

} // namespace spotlight

#endif // PARSE_HPP
