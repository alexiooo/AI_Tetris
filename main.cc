//
// aitetris.cc
// from http://www.liacs.leidenuniv.nl/~kosterswa/AI/
// January 22, 2018
// Walter Kosters, w.a.kosters@liacs.leidenuniv.nl
//
// Tetris playing programme
//
// Compile: g++ -Wall -O2 -o aitetris aitetris.cc
//
// Every piece has a unique name (see below), orientation (0/1/2/3),
// and starting position (0...width of game board - 1), indicating
// the column of the leftmost square of the position of the piece.
// Note that possible orientation and position depend on the piece!
// As an example: move 7 (out of 34, being 0..33) for piece LG (a Left
// Gun), on a width 10 board, corresponds with orientation 0, starting
// in column 7.
//
// The program generates a random series of pieces, and then needs
// an orientation and position (random in this version, see the function
// playrandomgame ( )); the piece then drops as required.
// After that rows are cleared, and the board is displayed.
//
// The board is of size h (height) times w (width);
// bottom left is (0,0); the top 3 rows are "outside" the board:
// if part of a piece ends here, the game stops.
//
// If you have a piece, the function possibilities returns the number
// of possible moves p. These moves are denoted by 0,1,...,p-1. Given
// a number n in this range, the function computeorandpos then computes
// the corresponding orientation and position. These can be used in
// the function letitfall to drop the piece.
//

//
// The 7 pieces, with orientations:
//
//  LS  0:  XX     1:  X                          Left Snake
//           XX       XX
//                    X
//
//  RS  0:   XX    1: X                           Right Snake
//          XX        XX
//                     X
//
//  I   0:  XXXX   1:  X                          I
//                     X
//                     X
//                     X
//
//  Sq  always 0: XX                              Square
//                XX
//
//  T   0:   XXX   1:  X     2:   X      3:   X   T
//            X        XX        XXX         XX
//                     X                      X
//
//  LG  0:  XXX    1:  XX    2:  X       3:   X   Left Gun
//            X        X         XXX          X
//                     X                     XX
//
//  RG  0:  XXX    1:  X     2:    X     3:  XX   Right Gun
//          X          X         XXX          X
//                     XX                     X
//

#include <iostream>
#include <ctime>         // for time stuff
#include <cstdlib>       // for rand ( )
#include <climits>       // for INT_MIN
using namespace std;


enum EvalName {Smart, Montecarlo};
const int MONTE_LOOP = 1000; //times each move is tested

enum PieceName {Sq,LG,RG,LS,RS,I,T};

const int wMAX = 20;     // maximum width of the game board
const int hMAX = 15;     // maximum total height of the game board

class Tetris {
  private:
    int h, w;               // actual height and width
    bool info = true;        // usage of play random game
    bool board[hMAX][wMAX]; // the game board; board[i][j] true <=> occupied
    int piececount;         // number of pieces that has been used so far
    int rowscleared;        // number of rows cleared so far
  public:
    void clearrows ( );
    void displayboard ( );
    void letitfall (PieceName piece, int orientation, int position);
    void infothrowpiece (PieceName piece, int orientation, int position);
    bool endofgame ( );
    Tetris (int height, int width);
    Tetris ( );
    void statistics ( );
    int possibilities (PieceName piece);
    void computeorandpos (PieceName piece, int & orientation, int & position, int themove);
    void randomchoice (PieceName piece, int & orientation, int & position);
    void toprow (bool therow[wMAX], int & numberrow, int & empties);
    int numberempties (int numberrow);
    void playrandomgame ( bool info );

    //Added functions
    int evaluateMonteCarlo(PieceName piece, int themove);
    int getPieceCount();
    void printinfo(PieceName piece, int orientation, int position);
    void domove(PieceName piece, int themove, bool info = true);
    int getblockedempties();
    int evaluate();
    int evaluatemove(PieceName piece, int themove);
    void playgame(EvalName);
};//Tetris

// default constructor
Tetris::Tetris ( ) {
  int i, j;
  h = hMAX;
  w = wMAX;
  for ( i = 0; i < hMAX; i++ )
    for ( j = 0; j < wMAX; j++ )
      board[i][j] = false;
}//Tetris::Tetris

// constructor
Tetris::Tetris (int height, int width) {
  int i, j;
  piececount = 0;
  rowscleared = 0;
  if ( height < hMAX )
    h = height;
  else
    h = hMAX;
  if ( 4 <= width && width < wMAX )
    w = width;
  else
    w = wMAX;
  for ( i = 0; i < hMAX; i++ )
    for ( j = 0; j < wMAX; j++ )
      board[i][j] = false;
}//Tetris::Tetris

// some statistics
void Tetris::statistics ( ) {
  cout << endl << "Done!" << endl
       << rowscleared << " row(s) cleared." << endl
       << piececount << " pieces." << endl << endl;
}//Tetris::statistics

// how many empties has row numberrow?
int Tetris::numberempties (int numberrow) {
  int j, theempties = w;
  for ( j = 0; j < w; j++ )
    if ( board[numberrow][j] )
      theempties--;
  return theempties;
}//Tetris::numberempties

// gives number of empties in heighest non-empty row,
// and copies this row into therow; its row index being numberrow
// totalempties contains number of empties in the whole board
// if this is -1, the whole field is empty
void Tetris::toprow (bool therow[wMAX], int & numberrow, int & empties) {
  int i, j, theempties;
  numberrow = -1;
  empties = w;
  for ( i = 0; i < h; i++ ) {
    theempties = numberempties (i);
    if ( theempties < w ) {
      for ( j = 0; j < w; j++ )
        therow[j] = board[i][j];
      empties = theempties;
      numberrow = i;
    }//if
  }//for
}//Tetris::toprow

// checks for full rows --- and removes them
void Tetris::clearrows ( ) {
  int i, j, k;
  bool full;
  for ( i = h-2; i >= 0; i-- ) {
    full = true;
    j = 0;
    while ( full && j < w )
      if ( !board[i][j] )
	full = false;
      else
        j++;
    if ( full ) {
      //cout << "Row cleared ..." << endl;
      rowscleared++;
      for ( k = i; k < h-1; k++ )
	for ( j = 0; j < w; j++ )
	  board[k][j] = board[k+1][j];
      for ( j = 0; j < w; j++ )
	board[h-1][j] = false;
    }//if
  }//for
}//Tetris::clearrows

// displays current board on the screen
void Tetris::displayboard ( ) {
  int i, j;
  for ( i = h-1; i >= 0; i-- ) {
    if ( i < h-3 )
      cout << "|";
    else
      cout << " ";
    for ( j = 0; j < w; j++ )
      if ( board[i][j] )
        cout << "X";
      else
	cout << " ";
    if ( i < h-3 )
      cout << "|" << endl;
    else
      cout << endl;
  }//for
  for ( j = 0; j < w+2; j++ )
    cout << "-";
  cout << endl;
  cout << " ";
  for ( j = 0; j < w; j++ )
    cout << j % 10;
  cout << endl;
}//Tetris::displayboard

// let piece fall in position and orientation given
// assume it still fits in top rows
void Tetris::letitfall (PieceName piece, int orientation, int position) {
  int x[4] = {0};
  int y[4] = {0};
  int i;
  piececount++;
  switch (piece) {
    case Sq: x[0] = position; y[0] = h-2;
             x[1] = position; y[1] = h-1;
	     x[2] = position+1; y[2] = h-2;
	     x[3] = position+1; y[3] = h-1;
	     break;
    case LG: switch (orientation) {
	       case 0: x[0] = position+2; y[0] = h-2;
                   x[1] = position+2; y[1] = h-1;
	               x[2] = position+1; y[2] = h-1;
	               x[3] = position; y[3] = h-1;
		       break;
	       case 1: x[0] = position; y[0] = h-3;
                   x[1] = position; y[1] = h-2;
	               x[2] = position; y[2] = h-1;
	               x[3] = position+1; y[3] = h-1;
		       break;
	       case 2: x[0] = position; y[0] = h-2;
                   x[1] = position+1; y[1] = h-2;
	               x[2] = position+2; y[2] = h-2;
	               x[3] = position; y[3] = h-1;
		       break;
	       case 3: x[0] = position; y[0] = h-3;
                   x[1] = position+1; y[1] = h-1;
	               x[2] = position+1; y[2] = h-2;
	               x[3] = position+1; y[3] = h-3;
		       break;
	     }//switch
	     break;
    case RG: switch (orientation) {
	       case 0: x[0] = position; y[0] = h-2;
                   x[1] = position+2; y[1] = h-1;
	               x[2] = position+1; y[2] = h-1;
	               x[3] = position; y[3] = h-1;
		       break;
	       case 1: x[0] = position; y[0] = h-3;
                   x[1] = position; y[1] = h-2;
	               x[2] = position; y[2] = h-1;
	               x[3] = position+1; y[3] = h-3;
		       break;
	       case 2: x[0] = position; y[0] = h-2;
                   x[1] = position+1; y[1] = h-2;
	               x[2] = position+2; y[2] = h-2;
	               x[3] = position+2; y[3] = h-1;
		       break;
	       case 3: x[0] = position+1; y[0] = h-3;
                   x[1] = position+1; y[1] = h-1;
	               x[2] = position+1; y[2] = h-2;
	               x[3] = position; y[3] = h-1;
		       break;
	     }//switch
	     break;
    case LS: switch (orientation) {
	       case 0: x[0] = position+1; y[0] = h-2;
                   x[1] = position+1; y[1] = h-1;
	               x[2] = position+2; y[2] = h-2;
	               x[3] = position; y[3] = h-1;

		       break;
	       case 1: x[0] = position; y[0] = h-3;
                   x[1] = position; y[1] = h-2;
	               x[2] = position+1; y[2] = h-1;
	               x[3] = position+1; y[3] = h-2;
		       break;
	     }//switch
	     break;
    case RS: switch (orientation) {
	       case 0: x[0] = position+1; y[0] = h-2;
                   x[1] = position+1; y[1] = h-1;
	               x[2] = position+2; y[2] = h-1;
	               x[3] = position; y[3] = h-2;
		       break;
	       case 1: x[0] = position+1; y[0] = h-3;
                   x[1] = position; y[1] = h-2;
	               x[2] = position+1; y[2] = h-2;
	               x[3] = position; y[3] = h-1;
		       break;
	     }//switch
	     break;
    case I : switch (orientation) {
	       case 0: x[0] = position; y[0] = h-1;
                   x[1] = position+1; y[1] = h-1;
	               x[2] = position+2; y[2] = h-1;
	               x[3] = position+3; y[3] = h-1;
		       break;
	       case 1: x[0] = position; y[0] = h-4;
                   x[1] = position; y[1] = h-3;
	               x[2] = position; y[2] = h-2;
	               x[3] = position; y[3] = h-1;
		       break;
	     }//switch
	     break;
    case T : switch (orientation) {
	       case 0: x[0] = position+1; y[0] = h-2;
                   x[1] = position; y[1] = h-1;
	               x[2] = position+1; y[2] = h-1;
	               x[3] = position+2; y[3] = h-1;
		       break;
	       case 1: x[0] = position; y[0] = h-3;
                   x[1] = position; y[1] = h-2;
	               x[2] = position; y[2] = h-1;
	               x[3] = position+1; y[3] = h-2;
		       break;
	       case 2: x[0] = position; y[0] = h-2;
                   x[1] = position+1; y[1] = h-2;
	               x[2] = position+2; y[2] = h-2;
	               x[3] = position+1; y[3] = h-1;
		       break;
	       case 3: x[0] = position+1; y[0] = h-3;
                   x[1] = position+1; y[1] = h-2;
	               x[2] = position+1; y[2] = h-1;
	               x[3] = position; y[3] = h-2;
		       break;
	     }//switch
	     break;
  }//switch
  while ( y[0] > 0 && !board[y[0]-1][x[0]]
          && !board[y[1]-1][x[1]] && !board[y[2]-1][x[2]]
          && !board[y[3]-1][x[3]] )
    for ( i = 0; i < 4; i++ )
      y[i]--;
  for ( i = 0; i < 4; i++ )
    board[y[i]][x[i]] = true;
}//Tetris::letitfall

// give piece a chance: info to the screen
void Tetris::infothrowpiece (PieceName piece, int orientation, int position) {
  int j;
  cout << endl;
  for ( j = 0; j < w+5; j++ )
    cout << "=";
  if ( piececount < 10 )
    cout << "   ";
  else if ( piececount < 100 )
    cout << "  ";
  else
    cout << " ";
  cout << piececount << ": ";
  switch ( piece ) {
    case Sq: cout << "Square      "; break;
    case LG: cout << "Left gun    "; break;
    case RG: cout << "Right gun   "; break;
    case LS: cout << "Left snake  "; break;
    case RS: cout << "Right snake "; break;
    case I:  cout << "I           "; break;
    case T:  cout << "T           "; break;
  }//switch
  cout << orientation << " " << position << endl;
}//Tetris::infothrowpiece

// check whether top 3 rows are somewhat occupied (so game has ended?)
bool Tetris::endofgame ( ) {
  int j;
  for ( j = 0; j < w; j++ )
    if ( board[h-3][j] )
      return true;
  return false;
}//Tetris::endofgame

// how many possibilities has piece?
int Tetris::possibilities (PieceName piece){
  if ( piece == Sq )
    return (w-1);
  else if ( piece == LS || piece == RS || piece == I )
    return (2*w-3);
  else
    return (4*w-6);
}//Tetris::possibilities

// compute orientation and position for move themove from piece
void Tetris::computeorandpos (PieceName piece, int & orientation, int & position, int themove) {
  orientation = 0;
  position = themove;
  switch ( piece ) {
    case LS:
    case RS: if ( themove > w-3 ) {
               orientation = 1;
               position = themove - (w-2);
             }//if
	     break;
    case I:  if ( themove > w-4 ) {
               orientation = 1;
               position = themove - (w-3);
             }//if
	     break;
    case Sq: break;
    case T:
    case LG:
    case RG: if ( themove > 3*w-6 ) {
               orientation = 3;
               position = themove - (3*w-5);
             }//if
             else if ( themove > 2*w-4 ) {
               orientation = 2;
               position = themove - (2*w-3);
             }//if
             else if ( themove > w-3 ) {
               orientation = 1;
               position = themove - (w-2);
             }//if
         break;
  }//switch
}//Tetris::computeorandpos

// now choose (random) orientation and position for piece
void Tetris::randomchoice (PieceName piece, int & orientation, int & position) {
  int themove = rand ( ) % possibilities (piece);
  computeorandpos (piece,orientation,position,themove);
}//Tetris::randomchoice

// generate a random piece
void getrandompiece (PieceName & piece) {
    int intpiece = rand ( ) % 7;
    switch (intpiece) {
      case 0: piece = LS; break;
      case 1: piece = RS; break;
      case 2: piece = I; break;
      case 3: piece = Sq; break;
      case 4: piece = T; break;
      case 5: piece = LG; break;
      case 6: piece = RG; break;
    }//switch
}//getrandompiece

//play a random game
void Tetris::playrandomgame (bool info) {
  PieceName piece;
  int orientation;
  int position;

  if( info )
    displayboard ( );

  while ( ! endofgame ( ) ) {
    getrandompiece (piece);                    // obtain some piece
    randomchoice (piece,orientation,position); // how to drop it?
    letitfall (piece,orientation,position);    // let it go
    clearrows ( );                             // clear rows

    if ( info )
      printinfo(piece,orientation,position);
  }//while
}//Tetris::playrandomgame


// /////////////////////////////////////
//
// End of code taken from aitetris.cc
//
// ////////////////////////////////////


//print some info about the board
void Tetris::printinfo(PieceName piece, int orientation, int position) {
    int nr, emp;
    bool therow[wMAX];

    infothrowpiece (piece,orientation,position); // some text
    displayboard ( );                            // print the board
    toprow (therow,nr,emp);                      // how is top row?
    if ( nr != -1 )
      cout << "Top row " << nr << " has " << emp << " empties" << endl;
}//Tetris::printinfo

//do the given move
void Tetris::domove(PieceName piece, int themove, bool info) {
    int orientation, position;
    bool therow[wMAX];

    computeorandpos(piece, orientation, position, themove);
    letitfall(piece, orientation, position);
    clearrows();

    if (info)
        printinfo(piece, orientation, position);
}//Tetris::domove

//return the amount of empties that have non-empties above them
int Tetris::getblockedempties() {
    int blocked = 0;
    bool isFree[wMAX];
    for (int i=0; i<w; i++)
        isFree[i] = true;

    for (int row = 0; row < w; row++) {
        for (int col=0; col < h; col++) {
            if (board[col][row]) {
                isFree[col] = false;
            } else if (!isFree[col]) {
                blocked += 1;
            }
        }
    }
    return blocked;
}

const int SCORE_TOPROW      = -10;
const int SCORE_EMPTIES     = -4;
const int SCORE_CLEARS      = 100;
const int SCORE_BLOCEKED    = -3;

//give the board a score, higher is better
int Tetris::evaluate() {
    bool therow[wMAX];
    int nr, emp;
    int block;

    if ( endofgame() )
        return INT_MIN;

    toprow (therow,nr,emp);

    block = getblockedempties();

    return
        SCORE_TOPROW * nr
        + SCORE_EMPTIES * emp
        + SCORE_CLEARS * rowscleared
        + SCORE_BLOCEKED * block;
}//Tetris::evaluate

//give themove a score, based on state after the move
int Tetris::evaluatemove(PieceName piece, int themove) {
    Tetris tetris = *this;

    tetris.domove(piece, themove, false);
    return tetris.evaluate();
}

//play a smart(ish) game
//loop over all
void Tetris::playgame(EvalName eval) {
    PieceName piece;
    int numPossible;
    int score;
    int maxScore, bestMove;

    while (! endofgame()) {
        maxScore = INT_MIN; bestMove = 0;

        getrandompiece(piece);
        numPossible = possibilities(piece);
        for (int possibleMove = 0; possibleMove < numPossible; possibleMove++) {

            switch (eval){
                case Montecarlo:
                    score = evaluateMonteCarlo(piece, possibleMove);
                    break;

                case Smart:
                    score = evaluatemove(piece, possibleMove);
                    break;
            }
            //cout << "Move " << possibleMove << " has score " << score << endl;
            if (score > maxScore){
                maxScore = score;
                bestMove = possibleMove;

                //cout << "\tnew best!" << endl;
            }
        }
        domove(piece, bestMove, true);
    }

}//Tetris::playgame


int Tetris::evaluateMonteCarlo(PieceName piece, int themove){
    int score = 0;
    Tetris tetris = *this;
    Tetris mcTetris;
    tetris.domove(piece, themove);
    for ( int i = 0; i < MONTE_LOOP; i++ ){
        mcTetris = tetris;
        mcTetris.playrandomgame( false );
        score += mcTetris.getPieceCount();
    }
    return score;
}

int Tetris::getPieceCount(){
    return piececount;
}


int main (int argc, char* argv[ ]) {
  if ( argc != 4 && argc != 5 ) {
    cout << "Usage: " << argv[0] << " <height> <width> < r(andom) | s(mart) | m(onte-carlo) >"
         << endl;
    cout << "Or:    " << argv[0] << " <height> <width> < r(andom) | s(mart) | m(onte-carlo) > <seed>"
         << endl;
    return 1;
  }//if
  int h = atoi (argv[1]);
  int w = atoi (argv[2]);
  Tetris board (h,w);

  uint seed;
  if ( argc == 4 ) {
    seed = time (NULL);
  } else {
    seed = atoi (argv[4]);
  }
  srand (seed);

  string mode = argv[3];
  switch (mode.at(0)) {
    case 'r':
        board.playrandomgame(true);
        break;

    case 's':
        board.playgame(Smart);
        break;

    case 'm':
        board.playgame(Montecarlo);
        break;
  }

  board.statistics ( );
  cout << "Seed for this game was: " << seed << endl;

  return 0;

}//main
