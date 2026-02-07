/**
 * @file file.hpp
 * @author Ranjodh Singh
 *
 * @brief FILE.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef FILE_HPP
#define FILE_HPP

#include <exception>
#include <string>
#include <fstream>
#include <cstdlib>
#include <getopt.h>

#include <spotlight/config/config.hpp>
#include <spotlight/config/defaults.hpp>
#include <spotlight/utils/error_utils.hpp>


namespace spotlight {

inline bool is_empty_or_comment(const std::string& line)
{
  size_t i = 0;
  while (i < line.size() && std::isspace((unsigned char)line[i]))
    ++i;

  return (i >= line.size()) || (line[i] == '#');
}

inline std::string_view trim(std::string_view s)
{
  while (!s.empty() && std::isspace((unsigned char)s.front()))
    s.remove_prefix(1);

  while (!s.empty() && std::isspace((unsigned char)s.back()))
    s.remove_suffix(1);

  return s;
}

inline bool parse_key_value(
  const std::string& line,
  std::string& key,
  std::string& value
)
{
  auto pos = line.find('=');
  if (pos == std::string_view::npos)
    return false;

  key = trim(line.substr(0, pos));
  value = trim(line.substr(pos + 1));

  return true;
}

inline void parse_config_file(
  const std::string& path, PipelineConfig& cfg
)
{
  std::ifstream file(path);
  if (!file)
  {
    log_err(
      "Can't find configuration file: " + std::string(CONF_FILE)
    );
  }

  std::string line, key, value;
  try
  {
    for (int i = 0; getline(file, line) ; i++)
    {
      if (is_empty_or_comment(line))
        continue;

      if (parse_key_value(line, key, value))
      {
        // TODO: KEY VALID? 
        cfg.set(key, value);
      }
      else
      {
        log_err(
          "Skipping line: " + std::to_string(i) + " in the conf file!"
        );
      }
    }
  }
  catch (...)
  {
    log_err("Failed to parse the configuration file!");
  }
}

} // namespace spotlight

#endif // FILE_HPP
