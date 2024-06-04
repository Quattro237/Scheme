#pragma once
#include "parser.h"
#include "tokenizer.h"
#include "parser.h"

#include <string>
#include <vector>
#include <functional>

class Interpreter {
public:
    std::string Run(const std::string&);

    std::shared_ptr<Object> GetAST(std::shared_ptr<Object> head);
    std::shared_ptr<Object> Evaluate(std::shared_ptr<Cell> head);
    std::string ASTToString(std::shared_ptr<Object> head);
    std::string CellToString(std::shared_ptr<Cell> head);

    std::vector<int> ToIntVector(std::shared_ptr<Object> head);
    std::vector<std::shared_ptr<Object>> ToObjVector(std::shared_ptr<Object> head);
    void ToList(std::vector<std::shared_ptr<Object>>& vec, std::shared_ptr<Cell> head);

    std::shared_ptr<Bool> CmpHandler(std::shared_ptr<Object> head,
                                     std::function<bool(int, int)> comparator);
    std::shared_ptr<Number> IntHandler(std::shared_ptr<Object> head,
                                       std::function<int(int, int)> comparator);
    std::shared_ptr<Bool> NumberHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Number> AbsHandler(std::shared_ptr<Object> head);

    std::shared_ptr<Object> AndHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Object> OrHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Bool> NotHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Bool> BooleanHandler(std::shared_ptr<Object> head);

    std::shared_ptr<Bool> PairHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Bool> NullHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Bool> IsListHandler(std::shared_ptr<Object> head);

    std::shared_ptr<Cell> ConsHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Object> CarHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Object> CdrHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Cell> ListHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Object> ListRefHandler(std::shared_ptr<Object> head);
    std::shared_ptr<Object> ListTailHandler(std::shared_ptr<Object> head);
};
