#include <iostream>

class InterfaceTerminal {
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
}

InterfaceTerminal::~InterfaceTerminal() {
}
