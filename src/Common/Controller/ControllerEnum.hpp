#ifndef CONTROLLER_ENUM_CPP
#define CONTROLLER_ENUM_CPP


enum Action {
            NONE,
            MOVE_LEFT,
            MOVE_RIGHT,
            ROTATE_LEFT,
            ROTATE_RIGHT,
            SOFT_DROP,
            DROP,
            ESCAPE
        };

enum MalusBonus {
    MalusNONE,
    Bonus_Block1X1,
    BonusReduceFallSpeed,
    MalusIncreaseFallSpeed,
    MalusReverseControls,
    MalusFreezeControls,
    MalusBlackOut,
    MalusThunder
};
        



#endif
