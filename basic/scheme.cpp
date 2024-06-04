#include "scheme.h"
#include "error.h"

#include <memory>
#include <sstream>
#include <set>
#include <functional>
#include <map>
#include <cassert>
#include <string>

namespace {
int Sum(const int a, const int b) {
    return a + b;
}
int Sub(const int a, const int b) {
    return a - b;
}
int Prod(const int a, const int b) {
    return a * b;
}
int Div(const int a, const int b) {
    return a / b;
}
int Max(const int a, const int b) {
    return std::max(a, b);
}
int Min(const int a, const int b) {
    return std::min(a, b);
}
bool GEQ(const int a, const int b) {
    return a >= b;
}
bool GR(const int a, const int b) {
    return a > b;
}
bool LEQ(const int a, const int b) {
    return a <= b;
}
bool LE(const int a, const int b) {
    return a < b;
}
bool EQ(const int a, const int b) {
    return a == b;
}
bool IsNumber(const std::shared_ptr<Object> object) {
    return Is<Number>(object);
}
int Abs(const int a) {
    return std::abs(a);
}
void PredicateCorrectnessCheck(const std::string& func_name, std::shared_ptr<Object> head) {
    if (head == nullptr) {
        throw RuntimeError("1 argument is expected for " + func_name);
    }

    if (!Is<Cell>(head)) {
        throw RuntimeError("Invalid call for " + func_name);
    }

    if (As<Cell>(head)->GetSecond() != nullptr) {
        throw RuntimeError("Too many arguments for " + func_name);
    }
}
}  // namespace

static const std::map<std::string, std::function<bool(int, int)>> kCompOperations = {
    {">=", GEQ}, {">", GR}, {"<=", LEQ}, {"<", LE}, {"=", EQ}};
static const std::map<std::string, std::function<int(int, int)>> kIntOperations = {
    {"+", Sum}, {"-", Sub}, {"*", Prod}, {"/", Div}, {"max", Max}, {"min", Min}};

std::shared_ptr<Object> Interpreter::GetAST(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        throw RuntimeError("Cannot call without command");
    }

    if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
        return head;
    }

    if (Is<Quote>(head)) {
        return As<Quote>(head)->next_;
    }

    assert(Is<Cell>(head));

    return Evaluate(As<Cell>(head));
}

std::shared_ptr<Object> Interpreter::Evaluate(std::shared_ptr<Cell> head) {
    if (head == nullptr) {
        throw RuntimeError("Cannot call without command");
    }

    if (!Is<Symbol>(head->GetFirst())) {
        throw RuntimeError("Cannot call without command");
    }

    std::string func_name = As<Symbol>(head->GetFirst())->GetName();

    if (auto cmp_iter = kCompOperations.find(func_name); cmp_iter != kCompOperations.end()) {
        return CmpHandler(head->GetSecond(), kCompOperations.at(func_name));
    } else if (auto int_iter = kIntOperations.find(func_name); int_iter != kIntOperations.end()) {
        return IntHandler(head->GetSecond(), kIntOperations.at(func_name));
    } else if (func_name == "number?") {
        return NumberHandler(head->GetSecond());
    } else if (func_name == "abs") {
        return AbsHandler(head->GetSecond());
    } else if (func_name == "quote") {
        if (!Is<Cell>(head->GetSecond())) {
            throw RuntimeError("Invalid quote use");
        }

        if (As<Cell>(head->GetSecond())->GetSecond() != nullptr) {
            throw RuntimeError("Invalid number of arguments for quote");
        }

        return As<Cell>(head->GetSecond())->GetFirst();
    } else if (func_name == "and") {
        return AndHandler(head->GetSecond());
    } else if (func_name == "or") {
        return OrHandler(head->GetSecond());
    } else if (func_name == "not") {
        return NotHandler(head->GetSecond());
    } else if (func_name == "boolean?") {
        return BooleanHandler(head->GetSecond());
    } else if (func_name == "pair?") {
        return PairHandler(head->GetSecond());
    } else if (func_name == "null?") {
        return NullHandler(head->GetSecond());
    } else if (func_name == "list?") {
        return IsListHandler(head->GetSecond());
    } else if (func_name == "cons") {
        return ConsHandler(head->GetSecond());
    } else if (func_name == "car") {
        return CarHandler(head->GetSecond());
    } else if (func_name == "cdr") {
        return CdrHandler(head->GetSecond());
    } else if (func_name == "list") {
        return ListHandler(head->GetSecond());
    } else if (func_name == "list-ref") {
        return ListRefHandler(head->GetSecond());
    } else if (func_name == "list-tail") {
        return ListTailHandler(head->GetSecond());
    }

    throw RuntimeError("passed through in Evaluate");
}

std::vector<int> Interpreter::ToIntVector(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        return {};
    }

    if (Is<Symbol>(head) || Is<Bool>(head)) {
        throw RuntimeError("Should be only integers in arithmetic functions");
    }

    if (Is<Quote>(head)) {
        if (Is<Number>(As<Quote>(head)->next_)) {
            return {As<Number>(As<Quote>(head)->next_)->GetValue()};
        } else {
            throw RuntimeError("Should be only integers in arithmetic functions");
        }
    }

    if (Is<Number>(head)) {
        return {As<Number>(head)->GetValue()};
    }

    assert(Is<Cell>(head));

    std::vector<int> result;

    while (head != nullptr) {
        if (Is<Number>(head)) {
            result.push_back(As<Number>(head)->GetValue());
            break;
        } else if (Is<Quote>(head)) {
            if (Is<Number>(As<Quote>(head)->next_)) {
                return {As<Number>(As<Quote>(head)->next_)->GetValue()};
            } else {
                throw RuntimeError("Should be only integers in arithmetic functions");
            }
        } else if (!Is<Cell>(head)) {
            throw RuntimeError("Should be only integers in arithmetic functions");
        }

        assert(Is<Cell>(head));

        std::shared_ptr<Object> left_son = GetAST(As<Cell>(head)->GetFirst());

        if (!Is<Number>(left_son)) {
            throw RuntimeError("Should be only integers in arithmetic functions");
        }

        result.push_back(As<Number>(left_son)->GetValue());

        head = As<Cell>(head)->GetSecond();
    }

    return result;
}

std::shared_ptr<Bool> Interpreter::CmpHandler(std::shared_ptr<Object> head,
                                              std::function<bool(int, int)> comparator) {
    std::vector<int> values = ToIntVector(head);

    bool result = true;
    for (size_t i = 0; i + 1 < values.size(); ++i) {
        result = result & comparator(values[i], values[i + 1]);
    }

    return std::make_shared<Bool>(result);
}

std::shared_ptr<Number> Interpreter::IntHandler(std::shared_ptr<Object> head,
                                                std::function<int(int, int)> comparator) {
    std::vector<int> values = ToIntVector(head);

    if (values.empty()) {
        if (*comparator.target<int (*)(int, int)>() == Sum) {
            return std::make_shared<Number>(0);
        } else if (*comparator.target<int (*)(int, int)>() == Prod) {
            return std::make_shared<Number>(1);
        } else {
            throw RuntimeError("Not enough arguments for arithmetic function");
        }
    }

    if (values.size() < 2 && (*comparator.target<int (*)(int, int)>() == Sub ||
                              *comparator.target<int (*)(int, int)>() == Div)) {
        throw RuntimeError("Not enough arguments for arithmetic function");
    }

    int result = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        result = comparator(result, values[i]);
    }

    return std::make_shared<Number>(result);
}

std::shared_ptr<Bool> Interpreter::NumberHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("number?", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    return std::make_shared<Bool>(IsNumber(argument));
}

std::shared_ptr<Number> Interpreter::AbsHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("abs", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    if (!Is<Number>(argument)) {
        throw RuntimeError("Expected number for abs");
    }

    return std::make_shared<Number>(Abs(As<Number>(argument)->GetValue()));
}

std::string Interpreter::ASTToString(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        return "()";
    }

    if (Is<Number>(head)) {
        return std::to_string(As<Number>(head)->GetValue());
    } else if (Is<Symbol>(head)) {
        return As<Symbol>(head)->GetName();
    } else if (Is<Bool>(head)) {
        return As<Bool>(head)->GetValue() ? "#t" : "#f";
    } else if (Is<Quote>(head)) {
        throw RuntimeError("Invalid AST passed to ASTToString");
    } else {
        std::string ans;
        ans += "(";
        ans += CellToString(As<Cell>(head));
        ans += ")";
        return ans;
    }
}

std::string Interpreter::CellToString(std::shared_ptr<Cell> head) {
    if (!head->HasSon()) {
        return "()";
    }

    std::string ans;

    std::shared_ptr<Object> current = head;
    while (current != nullptr) {
        if (Is<Number>(current)) {
            ans += " . ";
            ans += std::to_string(As<Number>(current)->GetValue());
            break;
        } else if (Is<Symbol>(current)) {
            ans += " . ";
            ans += As<Symbol>(current)->GetName();
            break;
        } else if (Is<Bool>(current)) {
            ans += " . ";
            ans += As<Bool>(current)->GetValue() ? "#t" : "#f";
            break;
        } else if (Is<Quote>(current)) {
            throw RuntimeError("Invalid AST passed to CellToString");
        }

        assert(Is<Cell>(current));

        std::shared_ptr<Object> left_son = As<Cell>(current)->GetFirst();

        if (Is<Number>(left_son)) {
            ans += ans.empty() ? "" : " ";
            ans += std::to_string(As<Number>(left_son)->GetValue());
        } else if (Is<Symbol>(left_son)) {
            ans += ans.empty() ? "" : " ";
            ans += As<Symbol>(left_son)->GetName();
        } else if (Is<Bool>(left_son)) {
            ans += ans.empty() ? "" : " ";
            ans += As<Bool>(left_son)->GetValue() ? "#t" : "#f";
        } else if (Is<Quote>(left_son)) {
            throw RuntimeError("Invalid AST passed to CellToString");
        } else {
            ans += ans.empty() ? "(" : " (";
            ans += CellToString(As<Cell>(left_son));
            ans += ")";
        }

        current = As<Cell>(current)->GetSecond();
    }

    return ans;
}

std::string Interpreter::Run(const std::string& input) {
    std::stringstream ss{input};
    Tokenizer tokenizer(&ss);
    std::shared_ptr<Object> head = Read(&tokenizer);

    std::shared_ptr<Object> new_head = GetAST(head);

    return ASTToString(new_head);
}

std::shared_ptr<Object> Interpreter::AndHandler(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        return std::make_shared<Bool>(true);
    }

    if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
        return head;
    }

    if (Is<Quote>(head)) {
        if (As<Quote>(head)->next_ == nullptr) {
            throw RuntimeError("Invalid syntax");
        } else {
            return As<Quote>(head)->next_;
        }
    }

    assert(Is<Cell>(head));

    std::shared_ptr<Object> last_element = nullptr;

    while (head != nullptr) {
        if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
            return head;
        } else if (Is<Quote>(head)) {
            if (As<Quote>(head)->next_ == nullptr) {
                throw RuntimeError("Invalid syntax");
            }
            if (Is<Bool>(As<Quote>(head)->next_) && !As<Bool>(As<Quote>(head)->next_)->GetValue()) {
                return std::make_shared<Bool>(false);
            } else {
                return As<Quote>(head)->next_;
            }
        }

        assert(Is<Cell>(head));

        std::shared_ptr<Object> left_son = GetAST(As<Cell>(head)->GetFirst());

        if (Is<Bool>(left_son) && !As<Bool>(left_son)->GetValue()) {
            return left_son;
        }

        last_element = left_son;

        head = As<Cell>(head)->GetSecond();
    }

    return last_element;
}

std::shared_ptr<Object> Interpreter::OrHandler(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        return std::make_shared<Bool>(false);
    }

    if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
        return head;
    }

    if (Is<Quote>(head)) {
        if (As<Quote>(head)->next_ == nullptr) {
            throw RuntimeError("Invalid syntax");
        } else {
            return As<Quote>(head)->next_;
        }
    }

    assert(Is<Cell>(head));

    std::shared_ptr<Object> last_element = nullptr;

    while (head != nullptr) {
        if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
            return head;
        } else if (Is<Quote>(head)) {
            if (As<Quote>(head)->next_ == nullptr) {
                throw RuntimeError("Invalid syntax");
            }
            return As<Quote>(head)->next_;
        }

        assert(Is<Cell>(head));

        std::shared_ptr<Object> left_son = GetAST(As<Cell>(head)->GetFirst());

        if (!(Is<Bool>(left_son) && !As<Bool>(left_son)->GetValue())) {
            return left_son;
        }

        last_element = left_son;

        head = As<Cell>(head)->GetSecond();
    }

    return last_element;
}

std::shared_ptr<Bool> Interpreter::NotHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("not", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    if (Is<Bool>(argument) && !As<Bool>(argument)->GetValue()) {
        return std::make_shared<Bool>(true);
    } else {
        return std::make_shared<Bool>(false);
    }
}

std::shared_ptr<Bool> Interpreter::BooleanHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("boolean?", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    return std::make_shared<Bool>(Is<Bool>(argument));
}

std::shared_ptr<Bool> Interpreter::PairHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("pair?", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    if (!Is<Cell>(argument) || !As<Cell>(argument)->HasSon()) {
        return std::make_shared<Bool>(false);
    } else {
        return std::make_shared<Bool>(true);
    }
}

std::shared_ptr<Bool> Interpreter::NullHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("null?", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    if (argument == nullptr) {
        return std::make_shared<Bool>(true);
    } else {
        return std::make_shared<Bool>(false);
    }
}

std::shared_ptr<Bool> Interpreter::IsListHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("list?", head);

    std::shared_ptr<Object> argument = GetAST(As<Cell>(head)->GetFirst());

    if (argument == nullptr) {
        return std::make_shared<Bool>(true);
    }

    if (!Is<Cell>(argument)) {
        return std::make_shared<Bool>(false);
    }

    while (argument != nullptr) {
        if (!Is<Cell>(argument)) {
            return std::make_shared<Bool>(false);
        }

        assert(Is<Cell>(argument));

        argument = As<Cell>(argument)->GetSecond();
    }

    return std::make_shared<Bool>(true);
}

std::vector<std::shared_ptr<Object>> Interpreter::ToObjVector(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        return {};
    }

    if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
        return {head};
    }

    if (Is<Quote>(head)) {
        if (As<Quote>(head)->next_ == nullptr) {
            throw RuntimeError("Syntax error");
        } else {
            return {As<Quote>(head)->next_};
        }
    }

    assert(Is<Cell>(head));

    std::vector<std::shared_ptr<Object>> result;

    while (head != nullptr) {
        if (Is<Number>(head) || Is<Symbol>(head) || Is<Bool>(head)) {
            result.push_back(head);
        }

        if (Is<Quote>(head)) {
            if (As<Quote>(head)->next_ == nullptr) {
                throw RuntimeError("Syntax error");
            } else {
                result.push_back(As<Quote>(head)->next_);
            }
        }

        assert(Is<Cell>(head));

        std::shared_ptr<Object> left_son = GetAST(As<Cell>(head)->GetFirst());

        result.push_back(left_son);

        head = As<Cell>(head)->GetSecond();
    }

    return result;
}

void Interpreter::ToList(std::vector<std::shared_ptr<Object>>& vec, std::shared_ptr<Cell> head) {
    if (vec.empty()) {
        head = nullptr;
    }

    std::shared_ptr<Cell> cur = head;
    for (size_t i = 0; i < vec.size(); ++i) {
        cur->first_ = vec[i];
        cur->second_ = std::make_shared<Cell>();
        cur = As<Cell>(cur->second_);
    }
}

std::shared_ptr<Cell> Interpreter::ConsHandler(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> elements = ToObjVector(head);
    if (elements.size() != 2) {
        throw RuntimeError("Invalid number of arguments for cons");
    }

    std::shared_ptr<Cell> new_head = std::make_shared<Cell>();
    new_head->first_ = elements[0];
    new_head->second_ = elements[1];

    return new_head;
}

std::shared_ptr<Object> Interpreter::CarHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("car", head);

    std::shared_ptr<Object> first_arg = GetAST(As<Cell>(head)->GetFirst());

    if (!Is<Cell>(first_arg)) {
        throw RuntimeError("Invalid call for car");
    }

    return As<Cell>(first_arg)->GetFirst();
}

std::shared_ptr<Object> Interpreter::CdrHandler(std::shared_ptr<Object> head) {
    PredicateCorrectnessCheck("cdr", head);

    std::shared_ptr<Object> first_arg = GetAST(As<Cell>(head)->GetFirst());

    if (!Is<Cell>(first_arg)) {
        throw RuntimeError("Invalid call for car");
    }

    return As<Cell>(first_arg)->GetSecond();
}

std::shared_ptr<Cell> Interpreter::ListHandler(std::shared_ptr<Object> head) {
    if (head == nullptr) {
        return nullptr;
    }
    if (!Is<Cell>(head)) {
        throw RuntimeError("Invalid call for list");
    }
    return As<Cell>(head);
}

std::shared_ptr<Object> Interpreter::ListRefHandler(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> elements = ToObjVector(head);
    if (elements.size() != 2) {
        throw RuntimeError("Invalid number of arguments for list-ref");
    }

    std::shared_ptr<Object> ptr_to_check = elements[0];
    if (ptr_to_check != nullptr) {
        if (!Is<Cell>(ptr_to_check)) {
            throw RuntimeError("Invalid arguments for list-ref");
        }

        while (ptr_to_check != nullptr) {
            if (!Is<Cell>(ptr_to_check)) {
                throw RuntimeError("Invalid arguments for list-ref");
            }

            assert(Is<Cell>(ptr_to_check));

            ptr_to_check = As<Cell>(ptr_to_check)->GetSecond();
        }

        if (!Is<Number>(elements[1])) {
            throw RuntimeError("Invalid arguments for list-ref");
        }
    }

    std::vector<std::shared_ptr<Object>> list_elems = ToObjVector(elements[0]);

    if (As<Number>(elements[1])->GetValue() < 0 ||
        (list_elems.size() <= As<Number>(elements[1])->GetValue())) {
        throw RuntimeError("Invalid index in list-ref");
    }

    return list_elems[As<Number>(elements[1])->GetValue()];
}

std::shared_ptr<Object> Interpreter::ListTailHandler(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> elements = ToObjVector(head);
    if (elements.size() != 2) {
        throw RuntimeError("Invalid number of arguments for list-ref");
    }

    std::shared_ptr<Object> ptr_to_check = elements[0];
    if (ptr_to_check != nullptr) {
        if (!Is<Cell>(ptr_to_check)) {
            throw RuntimeError("Invalid arguments for list-ref");
        }

        while (ptr_to_check != nullptr) {
            if (!Is<Cell>(ptr_to_check)) {
                throw RuntimeError("Invalid arguments for list-ref");
            }

            assert(Is<Cell>(ptr_to_check));

            ptr_to_check = As<Cell>(ptr_to_check)->GetSecond();
        }

        if (!Is<Number>(elements[1])) {
            throw RuntimeError("Invalid arguments for list-ref");
        }
    }

    std::vector<std::shared_ptr<Object>> list_elems = ToObjVector(elements[0]);

    if (As<Number>(elements[1])->GetValue() > list_elems.size()) {
        throw RuntimeError("Invalid index in list-tail");
    }

    size_t skipped_count = 0;
    std::shared_ptr<Cell> new_head = As<Cell>(elements[0]);
    while (skipped_count < As<Number>(elements[1])->GetValue()) {
        new_head = As<Cell>(new_head->GetSecond());
        ++skipped_count;
    }

    return new_head;
}