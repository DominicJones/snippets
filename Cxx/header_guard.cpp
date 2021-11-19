// C++17

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <regex>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <filesystem>




bool has_option(std::vector<std::string> const &args,
                std::string const &option_name, int &iarg)
{
  std::string rx(option_name);
  std::regex option_regex(rx);
  for (auto it = args.begin(); it != args.end(); ++it)
  {
    if (std::regex_search(std::string(*it), option_regex))
    {
      iarg = std::max(iarg, int(std::distance(args.begin(), it)));
      return true;
    }
  }

  return false;
}


std::optional<std::string>
get_option(std::vector<std::string> const &args,
           std::string const &option_name, int &iarg)
{
  std::string rx(option_name);
  std::regex option_regex(rx);
  for (auto it = args.begin(), end = args.end(); it != end; ++it)
  {
    if (std::regex_search(std::string(*it), option_regex))
    {
      iarg = std::max(iarg, int(std::distance(args.begin(), it)));
      if (it + 1 != end)
      {
        ++iarg;
        return *(it + 1);
      }
    }
  }

  return std::nullopt;
}




int main(int argc, char** argv)
{
  auto const args = std::vector<std::string>(argv, argv + argc);

  auto const emacs_header = std::string("// -*- C++ -*-");
  auto const pragma_once = std::string("#pragma once");

  // options
  auto show_help_opt = std::make_pair("-h|--help", "print help");
  auto is_verbose_opt = std::make_pair("-v|--verbose", "print status");
  auto fix_guards_opt = std::make_pair("-f|--fix-guards", "fix underscores in existing include guards");
  auto use_path_opt = std::make_pair("-p|--use-path", "generate include guards using file path");
  auto use_caps_path_opt = std::make_pair("-P|--use-caps-path", "generate include guards using capitalised file path");
  auto module_name_opt = std::make_pair("-m|--module-name", "generate include guards using module name");

  auto iarg = int{0};
  auto const show_help = has_option(args, show_help_opt.first, iarg);
  auto const is_verbose = has_option(args, is_verbose_opt.first, iarg);
  auto const fix_guards = has_option(args, fix_guards_opt.first, iarg);
  auto const use_path = has_option(args, use_path_opt.first, iarg);
  auto const use_caps_path = has_option(args, use_caps_path_opt.first, iarg);
  auto const module_name = get_option(args, module_name_opt.first, iarg);

  // for testing
  auto const is_extra_verbose = is_verbose && false;

  if (show_help)
  {
    std::string help_msg =
      std::string("Usage: header_guard [options] file...\n") +
      std::string("Options:\n") +
      show_help_opt.first + "  " + show_help_opt.second + "\n" +
      is_verbose_opt.first + "  " + is_verbose_opt.second + "\n" +
      fix_guards_opt.first + "  " + fix_guards_opt.second + "\n" +
      use_path_opt.first + "  " + use_path_opt.second + "\n" +
      use_caps_path_opt.first + "  " + use_caps_path_opt.second + "\n" +
      module_name_opt.first + " <arg>  " + module_name_opt.second + "\n" +
      "\n";
    std::cout << help_msg << std::endl;
    return 0;
  }


  auto const make_include_guard = !fix_guards && (use_path || use_caps_path || module_name.has_value());
  auto const make_pragma_guard = !fix_guards && (!make_include_guard);


  if (is_verbose)
  {
    if (make_pragma_guard)
      std::cout << "generating \"#pragma once\" guards" << std::endl;

    if (make_include_guard)
    {
      if (module_name.has_value())
        std::cout << "generating \"" + module_name.value() + "_\" prefixed include guards" << std::endl;
      else
        std::cout << "generating path-based include guards" << std::endl;
    }

    if (fix_guards)
      std::cout << "fixing existing include guards" << std::endl;
  }


  std::vector<std::string> filenames;
  filenames.reserve(1000);


  // make file list
  {
    std::regex const header_regex("\\.(h|hpp)$");

    for (auto it = iarg + 1; it < argc; ++it)
    {
      auto const fs_path = std::filesystem::path{args[it]};

      if (std::filesystem::is_directory(fs_path))
      {
        for (auto const &fs_entry: std::filesystem::recursive_directory_iterator{fs_path})
        {
          auto const entry = fs_entry.path().string();
          if (std::regex_search(entry, header_regex))
          {
            filenames.push_back(entry);
          }
        }
      }
      else
      {
        if (std::filesystem::is_regular_file(fs_path))
        {
          auto const entry = fs_path.string();
          if (std::regex_search(entry, header_regex))
          {
            filenames.push_back(entry);
          }
        }
        else
        {
          auto const entry = fs_path.string();
          if (std::regex_search(entry, header_regex))
          {
            std::fstream new_file;
            new_file.open(entry, std::ios_base::out);
            new_file.close();
            filenames.push_back(entry);
          }
        }
      }
    }
  }


  for (auto const &filename: filenames)
  {
    if (is_verbose)
    {
      std::cout << filename << std::endl;
    }


    std::string guard_name;
    auto has_include_guard = false;
    auto has_pragma_guard = false;


    // generate guard name
    if (!fix_guards)
    {
      if (module_name.has_value())
      {
        std::string const stem = std::filesystem::path{filename}.stem();
        guard_name = std::string(module_name.value()) + "_" + stem + "_h";
      }
      else
      {
        guard_name = filename;

        {
          std::regex const guard_regex("\\.|/");
          guard_name = std::regex_replace(guard_name, guard_regex, "_");
        }

        {
          std::regex const guard_regex("^_*");
          guard_name = std::regex_replace(guard_name, guard_regex, "");
        }

        if (use_caps_path)
        {
          std::for_each(guard_name.begin(), guard_name.end(), [](char &c){ c = ::toupper(c); });
        }
      }
    }


    std::vector<std::string> lines;
    lines.reserve(10000);


    // read file
    {
      std::ifstream file;
      std::string line;

      file.open(filename);

      while (std::getline(file, line))
      {
        lines.push_back(line);
      }

      file.close();
    }


    if (lines.empty())
    {
      lines.push_back("");
    }


    std::set<int> remove_lines;
    std::map<int, std::string> replace_lines;


    // find include guards
    {
      auto found_include_guard_start = false;
      auto found_include_guard_end = false;

      int it = 0;
      while (it != lines.size())
      {
        auto const &line = lines[it];
        ++it;

        if (!found_include_guard_start)
        {
          std::regex const ifndef_regex("^#\\s*ifndef\\s+(\\w+)");
          std::smatch ifndef_match;
          std::string ifndef_name;

          if (std::regex_search(line, ifndef_match, ifndef_regex))
          {
            ifndef_name = ifndef_match[1];

            auto const &line1 = lines[it];
            ++it;

            std::regex const define_regex("^#\\s*define\\s+" + ifndef_name);

            if (std::regex_search(line1, define_regex))
            {
              found_include_guard_start = true;

              if (make_pragma_guard)
              {
                remove_lines.insert(it - 2);
                replace_lines[it - 1] = pragma_once;
              }

              if (make_include_guard)
              {
                remove_lines.insert(it - 2);
                replace_lines[it - 1] = std::string("#ifndef " + guard_name + "\n#define " + guard_name);
              }

              if (fix_guards)
              {
                {
                  std::regex const leading_underscores_regex("^_*");
                  ifndef_name = std::regex_replace(ifndef_name, leading_underscores_regex, "");
                }

                {
                  std::regex const double_underscores_regex("__");
                  ifndef_name = std::regex_replace(ifndef_name, double_underscores_regex, "_");
                }

                remove_lines.insert(it - 2);
                replace_lines[it - 1] = std::string("#ifndef " + ifndef_name + "\n#define " + ifndef_name);
              }
            }
          }
        }
      }

      for (int it = lines.size(); it-- > 0;)
      {
        auto const &line = lines[it];

        if (found_include_guard_start && !found_include_guard_end)
        {
          std::regex const endif_regex("^#\\s*endif");

          if (std::regex_search(line, endif_regex))
          {
            found_include_guard_end = true;

            if (make_pragma_guard)
            {
              remove_lines.insert(it);
            }
          }
        }
     }

      has_include_guard = found_include_guard_start && found_include_guard_end;

      if (is_extra_verbose && has_include_guard)
        std::cout << "found include guards" << std::endl;
    }


    // find pragma guard
    {
      auto found_pragma_guard_start = false;

      for (int it = 0; it != lines.size(); ++it)
      {
        std::string line = lines[it];

        if (!found_pragma_guard_start)
        {
          std::regex const pragma_regex("^#\\s*pragma\\s+once");

          if (std::regex_search(line, pragma_regex))
          {
            found_pragma_guard_start = true;

            if (make_include_guard)
            {
              line = std::string("#ifndef " + guard_name + "\n#define " + guard_name);
              replace_lines[it] = line;
            }
          }
        }

        if (found_pragma_guard_start && (it == lines.size() - 1))
        {
          if (make_include_guard)
          {
            replace_lines[it] = line + "\n#endif // include guard";
          }
        }
      }

      has_pragma_guard = found_pragma_guard_start;

      if (is_extra_verbose && has_pragma_guard)
        std::cout << "found pragma guard" << std::endl;
    }

    if (!has_pragma_guard && !has_include_guard)
    {
      if (make_include_guard)
      {
        auto const begin = std::string("#ifndef " + guard_name + "\n#define " + guard_name);
        auto const end = std::string("#endif // include guard");
        replace_lines[0] = emacs_header + "\n" + begin + "\n" + end;
      }

      if (make_pragma_guard)
      {
        replace_lines[0] = emacs_header + "\n" + pragma_once;
      }
    }


    // write modified file
    {
      std::ofstream file;
      file.open(filename);

      for (int it = 0; it != lines.size(); ++it)
      {
        auto default_action = true;

        if (default_action)
        {
          auto remove_line = remove_lines.find(it);
          if (remove_line != remove_lines.end())
          {
            default_action = false;
          }
        }

        if (default_action)
        {
          auto replace_line = replace_lines.find(it);
          if (replace_line != replace_lines.end())
          {
            file << replace_line->second << "\n";
            default_action = false;
          }
        }

        if (default_action)
        {
          file << lines[it] << "\n";
        }
      }

      file.close();
    }

  } // file loop
}
