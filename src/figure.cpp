#include "tetrominoshape.cpp"
#include "coordinates.cpp"
#include "block.cpp"

#include <vector>

using namespace std;


class Figure {
    private:
        vector<Block> figBlocks; //4 figure's blocks, THE FIRST is the PIVOT, others are OFFSETS
        Coordinates center;
        TetrominoShape figShape;
    public:
        //Constructor
        Figure(TetrominoShape _figShape);
        //Getters
        vector<Block> getFigBlocks ();
        Coordinates getCenter ();

        //Setters
        void setFigBlocks ();
        void setCenter ();
        //Other Methods

        //Errors handling
        bool invariant ();        //assert (figBlocks.size() == 4)

};