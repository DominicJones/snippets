#include <vector>
#include <string>
#include <iostream>


class Subject;

class Observer
{
public:
  virtual ~Observer() {};
  virtual void notify(Subject const * s) const = 0;
};


class Subject
{
public:
  virtual ~Subject() {};

  void addObserver(Observer const * o)
  {
    _observers.push_back(o);
  }

protected:
  void notifyObservers() const
  {
    std::vector<Observer const *>::const_iterator it;
    for (it = _observers.begin(); it != _observers.end(); ++it)
      {
        (*it)->notify(this);
      }
  }

private:
  std::vector<Observer const *> _observers;
};


class Alarm;

class AlarmObservable: public Subject
{
public:
  AlarmObservable(Alarm const &host)
    : _host(host)
  {}

  void notifyObservers() const
  {
    Subject::notifyObservers();
  }

  Alarm const &host() const { return _host; }

private:
  Alarm const &_host;
};


class Alarm
{
public:
  Alarm()
    : _isTriggered(false)
    , _alarmObservable(*this)
  {}

  bool isTriggered() const
  {
    return _isTriggered;
  }

  void trigger()
  {
    std::cout << "The alarm has been triggered"  << std::endl;
    _isTriggered = true;
    _alarmObservable.notifyObservers();
  }

  void reset()
  {
    std::cout << "The alarm has been reset" << std::endl;
    _isTriggered = false;
    _alarmObservable.notifyObservers();
  }

  void addAlarmObserver(Observer const &observer)
  {
    _alarmObservable.addObserver(&observer);
  }

private:
  bool _isTriggered;
  AlarmObservable _alarmObservable;
};


class Horn
{
public:
  Horn(int loudness)
    : _loudness(loudness), _alarmObserver(*this)
  {}

private:
  int const _loudness;

  class AlarmObserver: public Observer
  {
  public:
    AlarmObserver(Horn const &outer)
      : _outer(outer)
    {}

    void notify(Subject const * s) const
    {
      if (AlarmObservable const * ao = dynamic_cast<AlarmObservable const *>(s))
        {
          if (ao->host().isTriggered())
            {
              std::cout << "Sound the horn at "
                        << _outer._loudness << "dB"
                        << std::endl;
            }
          else
            {
              std::cout << "Turn off the horn"
                        << std::endl;
            }
        }
    }

  private:
    Horn const &_outer;
  } _alarmObserver;

public:
  AlarmObserver const &alarmObserver() const
  {
    return _alarmObserver;
  }
};


int main()
{
  Alarm a = Alarm();

  Horn h1 = Horn(10);
  Horn h2 = Horn(20);

  a.addAlarmObserver(h1.alarmObserver());
  a.addAlarmObserver(h2.alarmObserver());

  a.trigger();
  a.reset();
}
