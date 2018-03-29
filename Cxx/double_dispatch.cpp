#include <iostream>
#include <memory>
#include <functional>
#include <cstddef>


using std::cout;
using std::endl;


class Printer;

class Figure {
public:
  virtual void printOn(Printer* printer) = 0;
};


class Circle;
class Rectangle;

class Printer {
public:
  virtual void printCircle(const Circle* circle) = 0;
  virtual void printRectangle(const Rectangle* rectangle) = 0;
};


class Client {
public:
  void printAllEverywhere(Figure **figures, Printer **printers) {
    Figure  **figure;
    Printer **printer;

    for (figure = figures; *figure; figure++) {
      for (printer = printers; *printer; printer++) {
        Figure *fig = *figure;
        fig->printOn(*printer);
      }
    }
  }
};


class InkjetPrinter : public Printer {
public:
  void printCircle(const Circle *circle) {
    cout << "Inkjet printer prints a circle." << endl;
  }
  void printRectangle(const Rectangle *rectangle) {
    cout << "Inkjet printer prints a rectangle." << endl;
  }
};


class PostscriptPrinter : public Printer {
public:
  void printCircle(const Circle *circle) {
    cout << "PostScript printer prints a cirlce." << endl;
  }
  void printRectangle(const Rectangle *rectangle) {
    cout << "PostScript printer prints a rectangle." << endl;
  }
};


class Circle : public Figure {
public:
  void printOn(Printer* printer) {
    printer->printCircle(this);
  }
};


class Rectangle : public Figure {
public:
  void printOn(Printer* printer) {
    printer->printRectangle(this);
  }
};


int main()
{
  Figure  *figures[]  = { new Circle, new Rectangle, 0 };
  Printer *printers[] = { new PostscriptPrinter, new InkjetPrinter, 0 };

  Client c;
  c.printAllEverywhere(figures, printers);
}
