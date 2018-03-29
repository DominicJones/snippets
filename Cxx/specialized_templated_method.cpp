#include <vector>
#include <iostream>

class Cell {};
class Face {};
class Vertex {};

class Variable {};
class ScalarVar : public Variable {};


class LocatedVariable
{
public:
  LocatedVariable(Variable * variable,
                  bool atCell = true,
                  bool atFace = true,
                  bool atVertex = true)
    : _variable(variable)
    , _atCell(atCell)
    , _atFace(atFace)
    , _atVertex(atVertex)
  {}

  ~LocatedVariable() { delete _variable; }

private:
  template <class Where> class Overload {};

public:
  Variable &var() { return *_variable; }
  template <class Where> bool at() { return at(Overload<Where>()); }

private:
  template <class Where> bool at(Overload<Where>);
  bool at(Overload<Cell>) { return _atCell; }
  bool at(Overload<Face>) { return _atFace; }
  bool at(Overload<Vertex>) { return _atVertex; }

  Variable * _variable;
  bool _atCell;
  bool _atFace;
  bool _atVertex;
};


int main()
{
  typedef LocatedVariable Item;
  typedef std::vector<Item *> Collection;

  Collection collection;
  collection.push_back(new Item(new ScalarVar(), true, false, false));

  for (Collection::iterator it = collection.begin(); it != collection.end(); ++it)
    {
      std::cout << (**it).at<Cell>() << std::endl;
      std::cout << (**it).at<Face>() << std::endl;
      std::cout << (**it).at<Vertex>() << std::endl;
    }
}
