#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;


class Controller
{
public:
  Controller(int i) : Index(i)
  {
    wcout << L"Creating Controller " << Index << endl;
  }

  ~Controller()
  {
    wcout << L"Destroying Controller " << Index << endl;
  }

  void status() const
  {
    auto fn = [](weak_ptr<Controller> wp) {
      try {
        auto p = wp.lock();
        wcout << L"Other Index = " << p->Index << endl;
      }
      catch (bad_weak_ptr b) {
        wcout << L"Null object" << endl;
      }
    };

    for_each(Others.begin(), Others.end(), fn);
  }

  int Index;
  vector<weak_ptr<Controller> > Others;
};


int main()
{
  vector<shared_ptr<Controller> > controllers;

  controllers.push_back(make_shared<Controller>(0));
  controllers.push_back(make_shared<Controller>(1));
  controllers.push_back(make_shared<Controller>(2));

  for (int i = 0 ; i < controllers.size(); ++i)
  {
    auto fn = [controllers, i](shared_ptr<Controller> p) {
      if(p->Index != i) {
        controllers[i]->Others.push_back(weak_ptr<Controller>(p));
        wcout << L"controller[" << i << "] Index = " << p->Index << endl;
      }
    };

    for_each(controllers.begin(), controllers.end(), fn);
  }

  {
    auto fn = [](shared_ptr<Controller> &p) {
      wcout << L"use_count = " << p.use_count() << endl;
      p->status();
    };

    for_each(controllers.begin(), controllers.end(), fn);
  }
}
