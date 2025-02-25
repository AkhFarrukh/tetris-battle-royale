//
// Created by farrukh on 25/02/25.
//

#ifndef MENUGAMEMODE_HPP
#define MENUGAMEMODE_HPP



enum MenuGameMode {
    ENDLESS,
    CLASSIC,
    DUEL,
    ROYAL_COMPETITION
};

MenuGameMode& operator++(MenuGameMode& mode);    // Prefix increment (++mode)
MenuGameMode operator++(MenuGameMode& mode, int);// Postfix increment (mode++)
MenuGameMode& operator--(MenuGameMode& mode);    // Prefix decrement (--mode)
MenuGameMode operator--(MenuGameMode& mode, int);// Postfix decrement (mode--)


#endif //MENUGAMEMODE_HPP
