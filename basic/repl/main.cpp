#include <iostream>
#include "../scheme.h"
#include <sstream>

int main() {
    std::string input = "(1 333 23 12 22)";
    std::stringstream ss{input};
    Tokenizer tokenizer(&ss);
    std::shared_ptr<Object> head = Read(&tokenizer);
    Interpreter interp;
    std::vector<int> int_vec = interp.ToIntVector(As<Cell>(head));

    for (int a : int_vec) {
        std::cout << a << ' ';
    }
    return 0;
}
