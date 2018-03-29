// A small program to demonstrate some C++11 features
// and to test the performance of calcuating field data
// by inlining vs delegating to a function vs delegating
// to a lambda function
//
// Compiling as
//   g++ -std=c++11 <file name>
// gives timings of
//   Inline elapsed time:   1.08001s
//   Lambda elapsed time:   1.12134s
//   Function elapsed time: 3.03957s
//
// Compiling as
//   g++ -O3 -std=c++11 <file name>
// gives timings of
//   Inline elapsed time:   0.40761s
//   Lambda elapsed time:   0.40804s
//   Function elapsed time: 0.304468s
//
// g++ --version
//   g++ (GCC) 4.7.0 20120507

#include <iostream>
#include <tuple>
#include <array>
#include <vector>
#include <utility>
#include <type_traits>
#include <chrono>
#include <ctime>
#include <cmath>
#include <cassert>
#include <functional>
#include <map>


// provide compile-time switches for various
// evaluation implementations
enum class Mode { Inline, Function, Lambda, };

static std::map<Mode, std::string> mode_name{
  {Mode::Inline, "Inline"},{Mode::Function, "Function"},{Mode::Lambda, "Lambda"}
};


// base numerical type
typedef double num_type;


// some variable types
typedef std::array<num_type, 1> Var1;
typedef std::array<num_type, 2> Var2;
typedef num_type Var3;
typedef num_type Var4;


// fields of variables
typedef std::vector<Var1> Field1;
typedef std::vector<Var2> Field2;
typedef std::vector<Var3> Field3;
typedef std::vector<Var4> Field4;

Field1 field1;
Field2 field2;
Field3 field3;
Field4 field4;

static size_t init_fields()
{
  auto const field_size = 10000000;

  for (auto it = 0; it != field_size; ++it)
    {
      num_type val = it + 1;
      Var1 var1{val};     field1.push_back(var1);
      Var2 var2{val,val}; field2.push_back(var2);
      Var3 var3{0};       field3.push_back(var3);
      Var4 var4{0};       field4.push_back(var4);
    }

  return field_size;
}

static size_t const field_size = init_fields();


// loop-local evaluation in separate function
// to force caching and facilitate unit-testing
static auto
eval_local(std::tuple<Var1, Var2> const &input)
  -> std::tuple<Var3, Var4>
{
  auto const &input1 = std::get<0>(input);
  auto const &input2 = std::get<1>(input);

  Var3 const output1 = sqrt(pow(input1[0], 2)) * sqrt(pow(input2[0], 2));
  Var4 const output2 = sqrt(pow(input1[0], 2)) + sqrt(pow(input2[1], 2));

  return std::make_tuple(output1, output2);
}


// a unit-test to verify the behaviour of
// the local evaluation function
static bool unittest()
{
  auto const input = std::make_tuple(Var1{1}, Var2{2,2});
  auto output = eval_local(input);
  assert(std::get<0>(output) == 2);
  assert(std::get<1>(output) == 3);
  return true;
}

static bool const isVerified = unittest();


// A lambda approach
// This approach appears to offer no benefits.
void eval_lambda()
{
  size_t it;

  auto eval_lambda = [&] ()
    {
      field3[it] = sqrt(pow(field1[it][0], 2)) * sqrt(pow(field2[it][0], 2));
      field4[it] = sqrt(pow(field1[it][0], 2)) + sqrt(pow(field2[it][1], 2));
    };

  for (it = 0; it != field_size; ++it)
    {
      eval_lambda();
    }
}


// compute data for all field entries
// and time the complete calculation
template <Mode mode> void eval()
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  if (mode == Mode::Lambda)
    {
      eval_lambda();
    }
  else
    {
      for (auto it = 0; it != field_size; ++it)
        {
          if (mode == Mode::Inline)
            {
              field3[it] = sqrt(pow(field1[it][0], 2)) * sqrt(pow(field2[it][0], 2));
              field4[it] = sqrt(pow(field1[it][0], 2)) + sqrt(pow(field2[it][1], 2));
            }
          else if (mode == Mode::Function)
            {
              std::tie(field3[it], field4[it]) = eval_local(std::make_tuple(field1[it], field2[it]));
            }
        }
    }

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << mode_name[mode]
            << " elapsed time: " << elapsed_seconds.count() << "s"
            << std::endl;
}


int main()
{
  eval<Mode::Inline>();
  eval<Mode::Lambda>();
  eval<Mode::Function>();
}
