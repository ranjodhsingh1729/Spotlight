/**
 * @file cli.hpp
 * @author Ranjodh Singh
 *
 * @brief CLI.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef CLI_HPP
#define CLI_HPP

#include <map>
#include <type_traits>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>


namespace spotlight {

/**
 * I AM SO SORRY IF YOU HAVE TO READ THIS
 * I POLITELY SUGGEST THAT YOU DON'T - NOTHING GOOD WILL HAPPEN.
 *
 * Parses command line arguments and stores them in a map.
 * This class does not perform any validation on the arguments.
 *
 * All the flags are stored as flag>: "".
 * All the options are stored as <option>: <value>.
 * All the operands are put inside the vector `operands`.
 */
class CLI
{
 public:
  CLI(const int argc, const char **argv)
    : argc(argc), argv(argv)
  {
    /* Nothing To Do Here. */
  }

  void Parse()
  {
    std::string set_opt;
    for (int i = 1; i < argc; i++)
    {
      const char* arg = argv[i];

      if (arg[0] == '-')
      {
        if (arg[1] == '-')
        {
          if (arg[2] == '\0')
          {
            for (i++; i < argc; i++)
              operands.push_back(argv[i]);
            break;
          }
          else
          {
            set_opt = &arg[2];
            options[set_opt] = "";
          }
        }
        else
        {
          if (arg[1] == '\0')
          {
            if (set_opt != "")
            {
              options[set_opt] = arg;
              set_opt.clear();
            }
            else
            {
              operands.push_back(arg);
            }
          }
          else
          {
            int j = 1;
            while (arg[j] != '\0')
              options[std::string(1, arg[j++])] = "";
            set_opt = std::string(1, arg[--j]);
          }
        }
      }
      else
      {
        if (set_opt != "")
        {
          options[set_opt] = arg;
          set_opt.clear();
        }
        else
        {
          operands.push_back(arg);
        }
      }
    }
  }

  template <typename T>
  std::enable_if_t<std::is_integral_v<T>, void>
  set(T& target, const std::string& key, T preset)
  {
    if (options.find(key) != options.end())
    {
      try
      {
        target = (T)std::stoi(options[key]);
      } catch (...)
      {
        target = preset;
      }
    }
    else
    {
      target = preset;
    }
  }

  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, void>
  set(T& target, const std::string& key, T preset)
  {
    if (options.find(key) != options.end())
    {
      try
      {
        target = (T)std::stof(options[key]);
      } catch (...)
      {
        target = preset;
      }
    }
    else
    {
      target = preset;
    }
  }

  void set(
    std::string& target, const std::string& key, const std::string& preset
  )
  {
    if (options.find(key) != options.end())
      target = options[key];
    else
      target = preset;
  }


  const int argc;
  const char** argv;
  std::vector<std::string> operands;
  std::map<std::string, std::string> options;
};

} // namespace spotlight

#endif // CLI_HPP
