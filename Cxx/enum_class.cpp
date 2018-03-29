enum class Option;


template<Option option> class Class
{
public:
  bool isOff();
};


enum class Option { off, on, };


template<Option option>
bool
Class<option>::
isOff()
{
  return (option == Option::off);
}


int main()
{
  Class<Option::off> c;
}
