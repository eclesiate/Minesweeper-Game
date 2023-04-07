#include "geesespotter_lib.h"

char *createBoard(std::size_t xdim, std::size_t ydim);
void cleanBoard(char *board);
void printBoard(char* board, std::size_t xdim, std::size_t ydim);
void computeNeighbors(char *board, std::size_t xdim, std::size_t ydim);
void hideBoard(char *board, std::size_t xdim, std::size_t ydim);
int reveal(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc);
int mark(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc);
bool isGameWon(char* board, std::size_t xdim, std::size_t ydim);

char *createBoard(std::size_t xdim, std::size_t ydim)
{
    char *a_board_array { new char [xdim*ydim]{} };
    return a_board_array;
}

void cleanBoard(char *board)
{
    delete[] board;
    board = nullptr;
}

void hideBoard(char *board, std::size_t xdim, std::size_t ydim)
{
    for (std::size_t i{0}; i < ydim*xdim; ++i){
        //the most significant bit should now be toggled to 1
        board[i] ^= hiddenBit();
    }   
}

void printBoard(char* board, std::size_t xdim, std::size_t ydim)
{
    for (std::size_t row{0}; row < ydim; ++row) {
        for(std::size_t col{0}; col < xdim; ++col) {
            //if the entry is marked, print M
            if(board[(row*xdim) + col] & markedBit()){
                std::cout << "M";
            //if the entry is hidden print *
            } else if ( board[(row * xdim) + col] & hiddenBit()){
                std::cout << "*";
            //for non hidden and non marked entries print field value
            } else {
                std::cout << (int) (board[(row * xdim) + col] & valueMask());
            }
        }
        //one row has been printed
        std::cout << std::endl;
    }    
}

int mark(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc)
{
    //if the bit is hidden, we can mark
    if(board[(yloc * xdim) + xloc] & hiddenBit()){
        //if the bit is 0, it will now be 1, vice versa
        board[(yloc * xdim) + xloc] ^= markedBit();
        return 0;
    //you cannot mark a revealed field
    } else {
        return 2;
    }
}

void computeNeighbors(char *board, std::size_t xdim, std::size_t ydim) 
{
    //loop through the whole board
    for(std::size_t row {0}; row < ydim; ++row){
        for (std::size_t col {0}; col < xdim; ++col) {
            //use mask to remove hidden and marked bits, check all adjacent values to goose 
            //update the adj values if they are not geese.
            if( ((board[(row*xdim) + col]) & valueMask()) == 9 ){
                //local vars for the adjacent entries
                int adj_row {}, adj_col{};
                //this nested for loop looks at the adjacent 3x3 square around the goose
                //check the row above, the current row, and the row below
                for(int epsilon_row{-1}; epsilon_row <= 1; ++epsilon_row){
                    adj_row = row + epsilon_row;
                    //if adjacent row is out of bounds, continue to next row
                    if( ( adj_row < 0 ) || ( adj_row > ydim - 1) ){
                        continue;
                    }
                    // otherwise check the col to the left, the current one, and the one to the right 
                    for(int epsilon_col{-1}; epsilon_col <= 1; ++epsilon_col){
                        adj_col = col + epsilon_col;
                        //if the adjacent column is within bounds, check for goose
                        //otherwise the loop will iterate to next col (ex. corner case)
                        if( (adj_col >= 0) && (adj_col <= xdim - 1)){
                            //if that value is not a goose then increment it 
                            if( ( board[(adj_row*xdim) + adj_col] & valueMask() ) != 9){
                                ++board[(adj_row*xdim) + adj_col];
                            }
                        }
                    }
                }      
            //skip the non-geese values         
            } else { continue; }
        }   
    }
}
bool isGameWon(char* board, std::size_t xdim, std::size_t ydim)
{
    int numgeese{};
    for(std::size_t k{}; k < xdim*ydim; ++k){
        //if the field is not a geese
        if((board[k] & valueMask()) != 9){
            //if the field is not a geese, and still hidden game is not won
            if(board[k] & hiddenBit()){
               return false; 
            }
        } else {
            //the field must have a goose then
            ++numgeese;
            //if the number of geese is equal to the board size then the game is won
            if(numgeese == xdim*ydim){
                return true;
            }
        } 
    }
    //if we have gotten to here then the game is won
    return true;
}

int reveal(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc)
{
    //type cast the arithmetic stuff, otherwise narrowing conversion
    int position {(int) ((yloc*xdim) + xloc)};
    //check if the field is unmarked and hidden
    if(!(board[position] & markedBit()) && (board[position] & hiddenBit())) {
        //remove all but the last 4 bts
        board[position] &= valueMask();
        //if there is a goose in the field
        if(board[position] == 9) {
            return 9;
        //for an empty field, we already removed all but the last 4 bits
        } else if(board[position] == 0){
            //declare local vars
            int adj_row {}, adj_col {};
            //similar algorithm to compute neighbours, but instead we reveal the neighbors
            for(int epsilon_row{-1}; epsilon_row <= 1; ++epsilon_row){
                adj_row = (int) yloc + epsilon_row;
                //if adjacent row is out of bounds, continue to next row
                if( ( adj_row < 0 ) || ( adj_row > ydim - 1) ){
                    continue;
                }
                // otherwise loop to reveal the col to the left, the current one, and the right one
                for(int epsilon_col{-1}; epsilon_col <= 1; ++epsilon_col){
                    adj_col = (int) xloc + epsilon_col;
                    //if the adjacent column is within bounds, reveal
                    //otherwise the loop will iterate to next col (ex. corner case)
                    if( (adj_col >= 0) && (adj_col <= xdim - 1)){
                        //if the neighbour is not marked, reveal
                        //we dont care about checking if it has a goose because the orig. field is 0
                        if(!(board[(adj_row*xdim) + adj_col] & markedBit())){
                            board[(adj_row*xdim) + adj_col] &= valueMask();
                        }     
                    }
                }
            }
        } 
    //if the field is marked
    } else if(board[position] & markedBit()){
        return 1;
    //the field is already revealed
    } else if (!(board[position] & hiddenBit())) {
        return 2; 
    }
    return 0;
}