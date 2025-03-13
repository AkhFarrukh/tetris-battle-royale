//
// Created by hamza on 4/03/25.
//

#ifndef CHAT_HPP
#define CHAT_HPP

#include <ncurses.h>
#include <iostream>
#include <string.h>

using namespace std;

class Chat {
  private:

  public:
    Chat();
    ~Chat();
    void chatWith(const std::string& user, const string& Friend);
};



#endif //CHAT_HPP
