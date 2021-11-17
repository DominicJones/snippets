// usage:
//   pragma_guard.exe <files>
//
// action:
//   convert:
//     #ifndef <token>
//     #define <token>
//     ...
//     #endif
//
//   into:
//     #pragma once
//     ...
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <regex>
#include <map>
#include <set>

int main(int argc, char** argv)
{
  for (int iarg = 1; iarg != argc; ++iarg)
  {
    std::string filename = argv[iarg];

    {
      std::regex header_regex("\\.(h|hpp)$");

      if (!std::regex_search(filename, header_regex))
      {
        std::cout << filename << " is not a header file" << std::endl;
        continue;
      }
    }

    std::vector<std::string> lines;
    int line_count = 0;
    lines.reserve(10000);

    // read file
    {
      std::ifstream file;
      file.open(filename);
      std::string line = "";

      while (std::getline(file, line))
      {
        ++line_count;
        lines.push_back(line);
        // std::cout << line_count << ": " << line << std::endl;
      }

      file.close();
    }

    std::set<int> remove_lines;
    std::map<int, std::string> replace_lines;

    // find guards
    {
      auto found_guard_start = false;
      auto found_guard_end = false;

      int it = 0;
      while (it != line_count)
      {
        auto const &line = lines[it];
        ++it;
        // std::cout << it << ": " << line << std::endl;

        if (!found_guard_start)
        {
          std::regex ifndef_regex("^#\\s*ifndef\\s+(\\w+)");
          std::smatch ifndef_match;
          std::string ifndef_name;

          if (std::regex_search(line, ifndef_match, ifndef_regex))
          {
            ifndef_name = ifndef_match[1];
            // std::cout << "found: " << line << ", match: " << ifndef_name << std::endl;

            auto const &line1 = lines[it];
            ++it;

            std::regex define_regex("^#\\s*define\\s+" + ifndef_name);

            if (std::regex_search(line1, define_regex))
            {
              found_guard_start = true;
              remove_lines.insert(it - 2);
              replace_lines[it - 1] = std::string("#pragma once");
              // std::cout << "found: " << line1 << std::endl;
            }
          }
        }
      }

      for (int it = line_count; it-- > 0;)
      {
        auto const &line = lines[it];

        if (found_guard_start && !found_guard_end)
        {
          std::regex endif_regex("^#\\s*endif");

          if (std::regex_search(line, endif_regex))
          {
            remove_lines.insert(it);
            found_guard_end = true;
            // std::cout << "found: " << line << std::endl;
          }
        }
      }
    }

    // write modified file
    {
      std::ofstream file;
      file.open(filename);

      for (int it = 0; it != line_count; ++it)
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
  }
}
