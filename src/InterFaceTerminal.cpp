#include <iostream>
#include "./game.cpp"

class InterfaceTerminal {
  Game gameClass;

public:
  InterfaceTerminal();
  InterfaceTerminal(InterfaceTerminal &&) = default;
  InterfaceTerminal(const InterfaceTerminal &) = default;
  InterfaceTerminal &operator=(InterfaceTerminal &&) = default;
  InterfaceTerminal &operator=(const InterfaceTerminal &) = default;
  ~InterfaceTerminal();

private:
  
};

InterfaceTerminal::InterfaceTerminal() {
  gameClass.run();
}

InterfaceTerminal::~InterfaceTerminal() {
}
