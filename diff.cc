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
// Basiscode gekregen van:
// aitetris.cc
// from http://www.liacs.leidenuniv.nl/~kosterswa/AI/
// January 22, 2018
// Walter Kosters, w.a.kosters@liacs.leidenuniv.nl
//
// Hieronder staan alleen de verschillen ten opzichte van aitetris.cc
// /* ... */ duidt aan dat code ongewijzigd is overgenomen

/* ... */
#include <climits>       // for INT_MIN

enum EvalName {Smart, Montecarlo};
enum PieceName {Sq,LG,RG,LS,RS,I,T};

struct TetrisScore {
    int toprow, empties, clears, blocked;

    TetrisScore() :
        toprow(-5),
        empties(1),
        clears(2),
        blocked(0)
    {}
    TetrisScore(int t, int e, int c, int b) :
        toprow(t), empties(e), clears(c), blocked(b) {}
};

class Tetris {
  private:
    /* ... */
    TetrisScore score;      // coefficients for evaluate()

  public:
    bool info = true;       // determines whether info is output to stdout
    int monte_loop = 1000; //number of games played per move in montecarlo

    /* ... */
    Tetris (int height, int width, TetrisScore score);
    int evaluateMonteCarlo(PieceName piece, int themove);
    int getPieceCount();
    void printinfo(PieceName piece, int orientation, int position);
    void domove(PieceName piece, int themove);
    int getblockedempties();
    int evaluate();
    int evaluatemove(PieceName piece, int themove);
    void playgame(EvalName);
};//Tetris

// constructor
Tetris::Tetris (int height, int width, TetrisScore score) {
  /* Same as Tetris(int, int) */
  this->score = score;
}//Tetris::Tetris

/* ... */

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
            << argv[0] << " benchCarlo" << endl
            << argv[0] << " smart" << endl;
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
