// decorator pattern, taken from
// http://stackoverflow.com/questions/2988066/decorator-pattern-in-c

#include <iostream>
using namespace std;


class Computer
{
public:
  virtual void display()
  {
    cout << "A computer " << endl;
  }
};


class CDDrive : public Computer
{
public:
  CDDrive(Computer * _c)
  {
    c = _c;
  }
  void display()
  {
    c->display();
    cout << "  with a CD drive, " << endl;
  }
private:
  Computer * c;
};


class Printer : public Computer
{
public:
  Printer(Computer * _c)
  {
    c = _c;
  }
  void display()
  {
    c->display();
    cout << "  with a printer, " << endl;
  }
private:
  Computer * c;
};


int main()
{
  Computer * c1 = new Computer();
  Computer * c2 = new CDDrive(dynamic_cast<Computer*>(c1));
  Computer * c3 = new Printer(dynamic_cast<Computer*>(c1));
  Computer * c4 = new Printer(dynamic_cast<Computer*>(c2));

  c2->display();
  c3->display();
  c4->display();
}
