// -*- C++ -*-

#include "Wire.h"
#include "Servo.h"
#include "Adafruit_MotorShield.h"


bool isDebugMode() {
 return true;
}


void debugMessage(char * msg) {
  if (isDebugMode()) {
    Serial.println(msg);
  }
}


class Mainboard {
public:
  static int const pin7() { return 7; }
  static int const pin9() { return 9; }
  static int const pin10() { return 10; }
};


class Motorshield {
public:
  static Adafruit_MotorShield &getInstance() {
    debugMessage("Motorshield::getInstance");
    static Adafruit_MotorShield instance = Adafruit_MotorShield();
    if (!_isInitialized) {
      instance.begin();
      _isInitialized = true;
    }
    return instance;
  }

  static int const motor1() { return 1; }
  static int const motor2() { return 2; }

  static int const servo1() { return Mainboard::pin10(); }
  static int const servo2() { return Mainboard::pin9(); }

private:
  static bool _isInitialized;
};

bool Motorshield::_isInitialized = false;


class Car {
public:
  Car(int drivePin, int steerPin, int collisionPin)
    : _drivePin(drivePin)
    , _steerPin(steerPin)
    , _collisionPin(collisionPin)
    , _steeringDelay(500)
    , _drivingSpeed(200)
  {}

  void start() {
    debugMessage("Car::start");
    steer().attach(_steerPin);
    pinMode(_collisionPin, INPUT);
    stopDriving();
    centraliseSteering();
  }

  void driveForward() {
    debugMessage("Car::driveForward");
    drive().setSpeed(_drivingSpeed);
    drive().run(FORWARD);
  }

  void driveBackward() {
    debugMessage("Car::driveBackward");
    drive().setSpeed(_drivingSpeed);
    drive().run(BACKWARD);
  }

  void stopDriving() {
    debugMessage("Car::stopDriving");
    drive().setSpeed(0);
  }

  void steerLeft() {
    debugMessage("Car::steerLeft");
    steer().write(0);
    delay(_steeringDelay);
  }

  void steerRight() {
    debugMessage("Car::steerRight");
    steer().write(180);
    delay(_steeringDelay);
  }

  void centraliseSteering() {
    debugMessage("Car::centraliseSteering");
    steer().write(90);
    delay(_steeringDelay);
  }

  bool hasCollided() {
    return (digitalRead(_collisionPin) == LOW);
  }

private:
  Adafruit_DCMotor &drive() {
    return *Motorshield::getInstance().getMotor(_drivePin);
  }

  Servo &steer() {
    return _steer;
  }

  int const _drivePin;
  int const _steerPin;
  int const _collisionPin;
  int const _steeringDelay;
  int const _drivingSpeed;
  Servo _steer;
};


Car car(Motorshield::motor1(),
	Motorshield::servo2(),
	Mainboard::pin7());


void setup() {
  Serial.begin(9600);
  debugMessage("setup");
  car.start();
  car.driveForward();
}


void loop() {
  debugMessage("loop");
  if (car.hasCollided())
    {
      car.stopDriving();
      car.steerLeft();
      car.driveBackward();
      delay(1000);
      car.stopDriving();
      car.steerRight();
      car.driveForward();
      delay(1000);
      car.centraliseSteering();
    }
}
