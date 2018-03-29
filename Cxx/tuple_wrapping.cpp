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

// base numerical type
typedef long double num_type;

// some variables
struct Var1 { typedef std::array<num_type, 1> type; };
struct Var2 { typedef std::array<num_type, 2> type; };
struct Var3 { typedef num_type type; };
struct Var4 { typedef num_type type; };


// loop-local evaluation in separate function
// to force caching and facilitate unit-testing
static auto
eval_local(std::tuple<Var1::type, Var2::type> const &input)
-> std::tuple<Var3::type, Var4::type>
{
    auto const &input1 = std::get<0>(input);
    auto const &input2 = std::get<1>(input);

    // a relatively expensive evaluation...
    Var3::type const output1 = sqrt(pow(input1[0], 2)) * sqrt(pow(input2[0], 2));
    Var4::type const output2 = sqrt(pow(input1[0], 2)) + sqrt(pow(input2[1], 2));

    auto output = std::make_tuple(std::move(output1), std::move(output2));
    return std::move(output);
}


// a unit-test to verify the behaviour of
// the local evaluation function
static bool unittest()
{
    auto const input = std::make_tuple(Var1::type{1}, Var2::type{2,2});
    auto output = eval_local(input);
    assert(std::get<0>(output) == 2);
    assert(std::get<1>(output) == 3);
    return true;
}

static bool const isVerified = unittest();


// fields of variables
std::vector<Var1::type> field1;
std::vector<Var2::type> field2;
std::vector<Var3::type> field3;
std::vector<Var4::type> field4;

static size_t init_fields()
{
    auto const field_size = 10000;

    for (auto it = 0; it != field_size; ++it)
    {
        num_type val = it + 1;
        field1.push_back({val});
        field2.push_back({val,val});
        field3.push_back({0});
        field4.push_back({0});
    }

    return field_size;
}

static size_t const field_size = init_fields();


// compute data for all field entries
// and time the complete calculation
template <bool isInlined> void eval()
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    for (auto it = 0; it != field_size; ++it)
    {
        if (isInlined)
        {
            std::tie(field3[it], field4[it]) = eval_local(std::make_tuple(field1[it], field2[it]));
        }
        else
        {
            field3[it] = sqrt(pow(field1[it][0], 2)) * sqrt(pow(field2[it][0], 2));
            field4[it] = sqrt(pow(field1[it][0], 2)) + sqrt(pow(field2[it][1], 2));
        }
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::string(isInlined? " (inlined)": " (not inlined)")
              << std::endl;
}


int main()
{
    eval<true>();
    eval<false>();
}
