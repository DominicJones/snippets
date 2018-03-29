#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <functional>

const int e_key = 47;
const int d_key = 15;
const int n = 391;


struct RSA_Transform
  : std::binary_function<int, int, int>
{
  int operator()(int input, int key) const
  {
    int result = 1;
    for (int i = 0; i < key; ++i)
      {
        result *= input;
        result %= n;
      }
    return result;
  }
};


int main()
{
  std::string msg = "Drink more Ovaltine.";
  bool readMsg = false;

  std::vector<int> cypherText;
  std::vector<int> plainText;

  for (int i = 0; i != msg.size(); ++i)
    if (readMsg)
      cypherText.push_back(msg[i]);
    else
      plainText.push_back(msg[i]);

  if (readMsg)
    {
      std::transform(cypherText.begin(), cypherText.end(),
                     std::back_inserter(plainText),
                     std::bind2nd(RSA_Transform(), d_key));

      for (int i = 0; i != plainText.size(); ++i)
        std::cout << (char) plainText[i] << "";
      std::cout << "\n";
    }
  else
    {
      std::transform(plainText.begin(), plainText.end(),
                     std::back_inserter(cypherText),
                     std::bind2nd(RSA_Transform(), e_key));

      for (int i = 0; i != cypherText.size(); ++i)
        std::cout << (char) cypherText[i] << "";
      std::cout << "\n";
    }
}
