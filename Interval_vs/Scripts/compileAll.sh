SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# R-tree
g++ -std=c++11 "$SCRIPT_DIR"/../src/RTest.cpp -o "$SCRIPT_DIR"/../rtest.out

# Interval-tree
g++ -std=c++11 "$SCRIPT_DIR"/../src/ITest.cpp -o "$SCRIPT_DIR"/../itest.out

# Schmidt
g++ -std=c++11 -I "$SCRIPT_DIR"/../src/includes/schmidt/OGDF/ -L "$SCRIPT_DIR"/../src/includes/schmidt/OGDF/_release/ "$SCRIPT_DIR"/../src/includes/schmidt/FastStabbing.cpp "$SCRIPT_DIR"/../src/STest.cpp -lOGDF -o "$SCRIPT_DIR"/../stest.out

# Succint I
g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib "$SCRIPT_DIR"/../src/C1Test.cpp -o "$SCRIPT_DIR"/../c1test.out -lsdsl -ldivsufsort -ldivsufsort64
