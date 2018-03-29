#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <typeinfo>

class Cell {};
class Face {};
class Vertex {};

class Variable {};
class ScalarVar : public Variable {};


namespace LocatedVariableOptions
{
  struct HasCell { enum Option { no, yes, }; };
  struct HasFace { enum Option { no, yes, }; };
  struct HasVertex { enum Option { no, yes, }; };
}

class LocatedVariable
{
private:
  typedef LocatedVariableOptions::HasCell HasCell;
  typedef LocatedVariableOptions::HasFace HasFace;
  typedef LocatedVariableOptions::HasVertex HasVertex;

  typedef std::map<std::string, bool> Map_t;

public:
  template <class T> struct At { typedef T Where; };

  LocatedVariable(Variable const * variable,
                  HasCell::Option const &atCell = HasCell::yes,
                  HasFace::Option const &atFace = HasFace::yes,
                  HasVertex::Option const &atVertex = HasVertex::yes)
    : _variable(variable)
  {
    std::string cell(typeid(At<Cell>::Where).name());
    _at[cell] = atCell;

    std::string face(typeid(At<Face>::Where).name());
    _at[face] = atFace;

    std::string vertex(typeid(At<Vertex>::Where).name());
    _at[vertex] = atVertex;
  }

  template <class At> LocatedVariable(Variable const * variable, At)
  {
    std::string where(typeid(typename At::Where).name());
    _at[where] = true;
  }

  ~LocatedVariable() { delete _variable; }

  Variable const &var() const { return *_variable; }

  template <class Where> bool at() const
  {
    std::string where(typeid(Where).name());
    Map_t::const_iterator it(_at.find(where));
    if (it != _at.end()) { return it->second; }
    return false;
  }

private:
  Variable const * _variable;
  Map_t _at;
};


int main()
{
  typedef LocatedVariable Item;
  typedef std::vector<Item *> Collection;

  using namespace LocatedVariableOptions;

  {
    Collection collection;
    collection.push_back(new Item(new ScalarVar(), HasCell::yes, HasFace::yes, HasVertex::no));

    for (Collection::iterator it = collection.begin(); it != collection.end(); ++it)
      {
        std::cout << (**it).at<Cell>() << std::endl;
        std::cout << (**it).at<Face>() << std::endl;
        std::cout << (**it).at<Vertex>() << std::endl;
      }
  }

  {
    Collection collection;
    collection.push_back(new Item(new ScalarVar(), Item::At<Vertex>()));

    for (Collection::iterator it = collection.begin(); it != collection.end(); ++it)
      {
        std::cout << (**it).at<Cell>() << std::endl;
        std::cout << (**it).at<Face>() << std::endl;
        std::cout << (**it).at<Vertex>() << std::endl;
      }
  }
}
