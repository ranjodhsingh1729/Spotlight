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

inline void parse_args(
  int argc, char** argv, PipelineConfig& cfg
)
{
  static struct option long_opts[] = {
    {"mode", required_argument, nullptr, 'm'},
    {"n-threads", required_argument, nullptr, 'n'},

    {"in-dev", required_argument, nullptr, 'i'},
    {"in-fmt", required_argument, nullptr, 3},
    {"in-w", required_argument, nullptr, 4},
    {"in-h", required_argument, nullptr, 5},
    {"in-fps", required_argument, nullptr, 6},

    {"out-dev", required_argument, nullptr, 'o'},
    {"out-fmt", required_argument, nullptr, 8},
    {"out-w", required_argument, nullptr, 9},
    {"out-h", required_argument, nullptr, 10},
    {"out-fps", required_argument, nullptr, 11},

    {"bg-img", required_argument, nullptr, 'b'},

    {nullptr, 0, nullptr, 0}
  };

  int opt;
  int long_index = 0;
  while (
    (
      opt = getopt_long(
        argc, argv, "m:n:i:o:b:", long_opts, &long_index
      )
    ) != -1
  )
  {
    switch (opt)
    {
    case 'm':
    case 'n':
    case 'i':
    case 'o':
    case 'b':
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
      cfg.set(long_opts[long_index].name, optarg);
      break;
    default:
        throw_err("Invalid Command Line Argument!!!");
    }
  }
}

} // namespace spotlight

#endif // PARSE_HPP
