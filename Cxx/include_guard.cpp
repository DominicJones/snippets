// usage:
//   include_guard.exe <files>
//
// action:
//   convert:
//     #pragma once
//     ...
//
//   into:
//     #ifndef <token>
//     #define <token>
//     ...
//     #endif
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
    std::string guard_name;

    {
      std::regex header_regex("\\.(h|hpp)$");

      if (!std::regex_search(filename, header_regex))
      {
        // std::cout << filename << " is not a header file" << std::endl;
        continue;
      }
    }

    {
      guard_name = filename;

      {
        std::regex guard_regex("\\.|/");
        guard_name = std::regex_replace(guard_name, guard_regex, "_");
      }

      {
        std::regex guard_regex("^_*");
        guard_name = std::regex_replace(guard_name, guard_regex, "");
      }

      // std::cout << guard_name << std::endl;
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

    // find guard
    {
      auto found_guard_start = false;

      for (int it = 0; it != line_count; ++it)
      {
        auto const &line = lines[it];
        // std::cout << it << ": " << line << std::endl;

        if (!found_guard_start)
        {
          std::regex pragma_regex("^#\\s*pragma\\s+once");

          if (std::regex_search(line, pragma_regex))
          {
            // std::cout << "found: " << line << std::endl;

            found_guard_start = true;
            replace_lines[it] = std::string("#ifndef " + guard_name + "\n#define " + guard_name);
          }
        }

        if (found_guard_start && (it == line_count - 1))
        {
          replace_lines[it] = line + "\n#endif // include guard";
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
