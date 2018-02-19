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

/* ... */
#include <climits>       // for INT_MIN

enum EvalName {Smart, Montecarlo};
const int MONTE_LOOP = 1000; //times each move is tested
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
    bool therow[wMAX];

    infothrowpiece (piece,orientation,position); // some text
    displayboard ( );                            // print the board
    toprow (therow,nr,emp);                      // how is top row?
    if ( nr != -1 )
      cout << "Top row " << nr << " has " << emp << " empties" << endl;
}//Tetris::printinfo

// do the given move
void Tetris::domove(PieceName piece, int themove) {
    int orientation, position;
    bool therow[wMAX];

    computeorandpos(piece, orientation, position, themove);
    letitfall(piece, orientation, position);
    clearrows();

    if (info)
        printinfo(piece, orientation, position);
}//Tetris::domove

// return the amount of empties that have non-empties above them
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

// give the board a score, higher is better
int Tetris::evaluate() {
    bool therow[wMAX];
    int nr, emp;
    int block;

    if ( endofgame() )
        return INT_MIN;

    toprow (therow,nr,emp);

    block = getblockedempties();

    return
        score.toprow * nr
        + score.empties * emp
        + score.clears * rowscleared
        + score.blocked * block;
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
    for ( int i = 0; i < MONTE_LOOP; i++ ){
        mcTetris = tetris;
        mcTetris.playrandomgame();
        score += mcTetris.getPieceCount();
    }
    return score;
}

int Tetris::getPieceCount(){
    return piececount;
}

int calculateMetrics(int argc, char* argv[]) {
    int totalscore;
    int start, limit;
    int loop = 100;
    int w=20, h=15;

    if (argc < 4) {
        cout << "Usage: " << argv[0] << " metrics <start> <stop> <loops> <height> <width> <seed>" << endl
          << "    loops, height, width and seed are optional and will default to 100, 20, 15 and time()" << endl;
        return 1;
    }
    start = atoi(argv[2]);
    limit = atoi(argv[3]);

    if (argc > 4)
        loop = atoi(argv[4]);
    if (argc > 5)
        w = atoi(argv[5]);
    if (argc > 6)
        h = atoi(argv[6]);

    if (argc > 7)
        srand(atoi(argv[7]));
    else
        srand(time(NULL));

    if (start > limit) {
        cout << "Error: given start was bigger than stop";
        return 3;
    }


    for(int a=start; a<limit; a++)
        for(int b=start; b<limit; b++)
        for(int c=start; c<limit; c++)
        for(int d=start; d<limit; d++) {
            TetrisScore sc(a,b,c,d);
            totalscore = 0;

            for(int i=0; i<loop; i++) {
                Tetris board(h, w, sc);
                board.info = false;
                board.playgame(Smart);
                totalscore += board.getPieceCount();
            }
            cout << a << "," << b << "," << c << "," << d << "," << totalscore << "\n";
        }
    return 0;
}


int play (int argc, char* argv[ ]) {
  if ( argc != 5 && argc != 6 ) {
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

        case 'm':
            return calculateMetrics(argc, argv);

        default:
            cout << argv[0] << " " << command << " not recognized, try:" << endl
                << argv[0] << " play" << endl
                << argv[0] << " metrics" << endl;
            return 2;
    }
}
