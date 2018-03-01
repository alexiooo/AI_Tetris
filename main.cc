// Tetris bot
//   Bram Honig s
//   Alex Keizer s2046253 Eerste jaars Informatica
// Getest op Windows 10 met de Cygwin64 GCC 6.4.0
//
// Speelt het klassieke Tetris op een van drie manieren:
//  - "random" speelt willekeurig
//  - "smart" evalueert elke zet ahv het bord na die zet (zie evaluate())
//  - "montecarlo" speelt voor elke zet 1000 willekeurige spelletjes, kiest
//      de zet die het beste gemidelde resultaat had (zie evaluateMonteCarlo())

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

enum PieceName {Sq,LG,RG,LS,RS,I,T};
const int wMAX = 20;     // maximum width of the game board
const int hMAX = 15;     // maximum total height of the game board

struct TetrisScore {
    int toprow, empties, clears, blocked, heightDiff;

    TetrisScore() :
        toprow(-10),
        empties(50),
        clears(150),
        blocked(-200),
        heightDiff(-10)
    {}
    TetrisScore(int t, int e, int c, int b, int d) :
        toprow(t), empties(e), clears(c), blocked(b), heightDiff(d) {}
};

class Tetris {
  private:
    int h, w;               // actual height and width
    bool board[hMAX][wMAX]; // the game board; board[i][j] true <=> occupied
    int piececount;         // number of pieces that has been used so far
    int rowscleared;        // number of rows cleared so far

    TetrisScore score;      // coefficients for evaluate()

  public:
    bool info = true;       // determines whether info is output to stdout
    int monte_loop = 1000; //number of games played per move in montecarlo

    void clearrows ( );
    void displayboard ( );
    void letitfall (PieceName piece, int orientation, int position);
    void infothrowpiece (PieceName piece, int orientation, int position);
    bool endofgame ( );
    Tetris (int height, int width, TetrisScore score);
    Tetris ( );
    void statistics ( );
    int possibilities (PieceName piece);
    void computeorandpos (PieceName piece, int & orientation, int & position, int themove);
    void randomchoice (PieceName piece, int & orientation, int & position);
    void toprow (bool therow[wMAX], int & numberrow, int & empties);
    int numberempties (int numberrow);
    void playrandomgame ( );

    //Added functions
    int evaluateMonteCarlo(PieceName piece, int themove);
    int getPieceCount();
    void printinfo(PieceName piece, int orientation, int position);
    void domove(PieceName piece, int themove);
    void getblockedempties(int & blocked, int & heightDiff);
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

  TetrisScore sc;
  score = sc;
}//Tetris::Tetris

// constructor
Tetris::Tetris (int height, int width, TetrisScore score) {
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

  this->score = score;
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
void Tetris::playrandomgame () {
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


// print some info about the board
void Tetris::printinfo(PieceName piece, int orientation, int position) {
    int nr, emp;
    int block, diff;
    bool therow[wMAX];

    infothrowpiece (piece,orientation,position); // some text
    displayboard ( );                            // print the board
    toprow (therow,nr,emp);                      // how is top row?
    getblockedempties(block, diff);
    if ( nr != -1 )
      cout << "Top row " << nr << " has " << emp << " empties | "
        << block << " blocked empties | " << diff << " height difference"
        << endl;
}//Tetris::printinfo

// do the given move
void Tetris::domove(PieceName piece, int themove) {
    int orientation, position;

    computeorandpos(piece, orientation, position, themove);
    letitfall(piece, orientation, position);
    clearrows();

    if (info)
        printinfo(piece, orientation, position);
}//Tetris::domove

// return the amount of empties that have non-empties above them
void Tetris::getblockedempties(int & blocked, int & heightDiff) {
    blocked = 0; heightDiff = 0;
    int colHeight[wMAX];
    for (int i=0; i<w; i++)
        colHeight[i] = -1;

    for (int row = h-1; row >= 0; row--) {
        for (int col = 0; col < w; col++) {
            if (board[row][col]) {
                if (colHeight[col] == -1){
                    colHeight[col] = row;
                }
            } else if (colHeight[col] > 0) {
                blocked += 1;
            }
        }
    }
    for (int col = 1; col < w; col++) {
        int d = colHeight[col] - colHeight[col-1];
        if (d<0) d = -d;
        heightDiff += d;
    }
}

// give the board a score, higher is better
int Tetris::evaluate() {
    bool therow[wMAX];
    int nr, emp;
    int block, diff;

    if ( endofgame() )
        return INT_MIN;

    toprow (therow,nr,emp);

    getblockedempties(block, diff);

    return
        score.toprow * nr*nr
        + score.empties * emp
        + score.clears * rowscleared
        + score.blocked * block
        + score.heightDiff * diff;
}//Tetris::evaluate

// give themove a score, based on state after the move
int Tetris::evaluatemove(PieceName piece, int themove) {
    Tetris tetris = *this;

    tetris.info = false;
    tetris.domove(piece, themove);
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
        domove(piece, bestMove);
    }

}//Tetris::playgame


int Tetris::evaluateMonteCarlo(PieceName piece, int themove){
    int score = 0;
    Tetris tetris = *this;
    Tetris mcTetris;

    tetris.info = false;
    tetris.domove(piece, themove);
    for ( int i = 0; i < monte_loop; i++ ){
        mcTetris = tetris;
        mcTetris.playrandomgame();
        score += mcTetris.getPieceCount();
    }
    return score;
}

int Tetris::getPieceCount(){
    return piececount;
}

void runningAverage(int score, int & sum, double & avg, double & dev, int & n) {
    double prevavg = avg;

    n += 1;
    sum += score;
    avg = sum / n;
    dev += (score - prevavg) * (score - avg);
}

int benchCarlo(int argc, char* argv[ ]) {
    Tetris kopie;

    if ( argc != 6 && argc != 7){
        cout << "Usage: " << argv[0] << " benchCarlo <height> <width> <monte_loop> <games> <seed>"
         << endl;
         return 1;
    }
    int h = atoi (argv[2]);
    int w = atoi (argv[3]);
    TetrisScore sc;
    Tetris board (h,w, sc);

    board.monte_loop = atoi (argv[4]);
    int games = atoi (argv[5]);
    if (games < 1) games = 1;

    int seed;
    if ( argc < 7 ) {
        seed = time (NULL);
    } else {
        seed = atoi (argv[6]);
    }
    srand (seed);

    board.info = games == 1;

    int sum=0, n=0;
    double avg=0.0, dev = 0.0;
    for (int i=0; i<games; i++){
        kopie = board;
        kopie.playgame(Montecarlo);

        int score = kopie.getPieceCount();
        runningAverage(score, sum, avg, dev, n);

        if(kopie.info) {
            kopie.statistics();
        } else {
            if(i==0) cout << score;
            else     cout << "," << score;
        }
    }
    cout << endl;

    if (!board.info) {
        //cout << "height, width, monte_loop, games, score (total), average, variance" << endl;
        cout << h << ", " << w << ", " << board.monte_loop << ", "
            << games << ", " << sum << ", " << avg << ", " << dev << endl;
    }
    return 0;
}

int benchSmart(int argc, char* argv[ ]) {
    if ( argc < 10 || argc > 11 ) {
        cout << "Usage: " << argv[0] << " smart <height> <width> <toprow> <empties> <clears> <blocked> <heightDiff> <loops> <seed>"
            << endl;
        return 1;
    }//if
    int h = atoi (argv[2]);
    int w = atoi (argv[3]);
    TetrisScore sc(atoi (argv[4]),
                   atoi (argv[5]),
                   atoi (argv[6]),
                   atoi (argv[7]),
                   atoi (argv[8]));
    int loops = atoi (argv[9]);
    if (loops < 1) loops = 1;

    Tetris board (h,w, sc);
    board.info = loops == 1;
    Tetris kopie;

    if ( argc < 11 ) {
        srand( time (NULL) );
    } else {
        srand( atoi (argv[10]) );
    }

    int sum=0, n=0;
    double avg=0.0, dev = 0.0;
    for (int i=0; i<loops; i++){
        kopie = board;
        kopie.playgame(EvalName::Smart);

        int score = kopie.getPieceCount();
        runningAverage(score, sum, avg, dev, n);

        if(kopie.info) {
            kopie.statistics();
        } else {
            if(i==0) cout << score;
            else     cout << "," << score;
        }
    }
    cout << endl;

    if (!board.info){
        cout << sc.toprow << ", " << sc.empties << ", " << sc.clears << ", "
            << sc.blocked << ", " << sc.heightDiff << ", " << loops << ", " << sum << ", " << dev << endl;
        cout << "Avg: " << (double) sum / loops << " pieces per game"
            << endl;
        cout << "Variance: " << dev << endl;
    }
    return 0;
}


int play (int argc, char* argv[ ]) {
  if ( argc < 5 || argc > 6 ) {
    cout << "Usage: " << argv[0] << " play <height> <width> < r(andom) | s(mart) | m(onte-carlo) >"
         << endl;
    cout << "Or:    " << argv[0] << " play <height> <width> < r(andom) | s(mart) | m(onte-carlo) > <seed>"
         << endl;
    return 1;
  }//if
  int h = atoi (argv[2]);
  int w = atoi (argv[3]);
  TetrisScore sc;
  Tetris board (h,w, sc);

  int seed;
  if ( argc == 5 ) {
    seed = time (NULL);
  } else {
    seed = atoi (argv[5]);
  }
  srand (seed);

  string mode = argv[4];

  switch (mode.at(0)) {
    case 'r':
      board.playrandomgame();
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
}

int main(int argc, char* argv[ ]) {
    if (argc < 2) {
        cout << "Please specify a command:" << endl
            << argv[0] << " play" << endl
            << argv[0] << " metrics" << endl;
        return 1;
    }

    string command = argv[1];
    switch (command.at(0)) {
        case 'p':
            return play(argc, argv);

        case 'b':
            return benchCarlo(argc, argv);

        case 's':
            return benchSmart(argc, argv);

        default:
            cout << argv[0] << " " << command << " not recognized, try:" << endl
                << argv[0] << " play" << endl
                << argv[0] << " benchCarlo" << endl
                << argv[0] << " smart" << endl;
            return 2;
    }
}


