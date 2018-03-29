/**
 * usage:
 *   rdmd new_design.d --sensitivity sens_file.csv --position pos_file.csv
 *
 * output:
 *   pos_file_1.csv
 */

import std.stdio;
import std.string;
import std.path;
import std.file;
import std.getopt;
import std.conv;
import std.csv;
import std.regex;
import std.algorithm;


void main(string[] args)
{
  auto separator = ',';
  auto positionFile = "position.csv";
  auto sensitivityFile = "sensitivity.csv";

  getopt(args,
         "separator", &separator,
         "position|p", &positionFile,
         "sensitivity|s", &sensitivityFile);

  auto position = parseCsvTable(positionFile, separator);
  auto sensitivity = parseCsvTable(sensitivityFile, separator);


  // update new position using the unconstrained steepest descent method
  {
    auto stepSize = getStepSize();

    for (auto i = 0; i != position.contents.length; ++i)
      {
        auto hasWeight = (position.contents[i].length == 7);

        for (auto j = 0; j != 3; ++j)
          {
            auto delta = stepSize * sensitivity.contents[i][j];

            if (hasWeight)
              {
                auto weight = position.contents[i][6];
                delta *= weight;
              }

            auto newPosition = position.contents[i][j + 3];
            position.contents[i][j + 3] = newPosition + delta;
          }
      }
  }


  // write position file
  {
    auto suffix = ".csv";
    auto basename = std.path.baseName(positionFile, suffix);
    auto hasDigits = match(basename, regex(r"_(\d+)$"));

    if (hasDigits.captures.length > 1)
      {
        string digits = hasDigits.captures[1];
        uint designStep = std.conv.parse!uint(digits) + 1;
        basename = hasDigits.pre ~ "_" ~ std.conv.to!string(designStep);
      }
    else
      {
        basename ~= "_1";
      }

    auto filename = basename ~ suffix;
    writeCsvTable(position, filename, separator);
  }
}


CsvTable parseCsvTable(string filename, char separator)
{
  CsvTable table;
  auto reader = csvReader(cast(string) read(filename), separator);

  foreach (item; reader.front())
    {
      ++table.header.length;
      table.header[$-1] = item;
    }

  reader.popFront();

  foreach (record; reader)
    {
      ++table.contents.length;

      foreach (item; record)
        {
          ++table.contents[$-1].length;
          table.contents[$-1][$-1] = std.conv.parse!double(item);
        }
    }

  return table;
}


void writeCsvTable(CsvTable table, string filename, char separator)
{
  string data;
  string record;

  foreach (item; table.header)
    {
      record ~= "\"" ~ item ~ "\"" ~ separator;
    }
  data ~= std.string.chop(record) ~ "\n";

  for (uint i = 0; i != table.contents.length; ++i)
    {
      record = "";
      for (uint j = 0; j != table.contents[i].length; ++j)
        {
          string item = std.conv.to!string(table.contents[i][j]);
          record ~= item ~ separator;
        }
      data ~= std.string.chop(record) ~ "\n";
    }

  std.file.write(filename, data);
}


auto getStepSize(double stepSize = 1.0e-5)
{
  std.stdio.writeln("X_1 = X + {Step size} * {df/dX} * weight");
  std.stdio.writef("Step size (default = %s): ", stepSize);

  {
    string input = chomp(readln());
    if (input.length != 0)
      {
        try {
          stepSize = std.conv.parse!double(input);
        }
        catch (std.conv.ConvException e) {
          write("[" ~ e.msg ~"] ");
        }
      }
  }

  std.stdio.writefln("Step size = %s", stepSize);
  return stepSize;
}


struct CsvTable
{
  string[] header;
  double[][] contents;
}
