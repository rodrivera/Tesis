SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

g++ -std=c++11 "$SCRIPT_DIR"/../src/RTest.cpp -o "$SCRIPT_DIR"/../rtest.out
g++ -std=c++11 "$SCRIPT_DIR"/../src/ITest.cpp -o "$SCRIPT_DIR"/../itest.out
g++ -std=c++11 -I "$SCRIPT_DIR"/../src/includes/schmidt/OGDF/ -L "$SCRIPT_DIR"/../src/includes/schmidt/OGDF/_release/ "$SCRIPT_DIR"/../src/includes/schmidt/FastStabbing.cpp "$SCRIPT_DIR"/../src/STest.cpp -lOGDF -o "$SCRIPT_DIR"/../stest.out
