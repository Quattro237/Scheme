#include <parser.h>
#include <error.h>

Number::Number(const int value) : value_(value) {
}

int Number::GetValue() const {
    return value_;
}

Bool::Bool(const bool value) : value_(value) {
}

bool Bool::GetValue() const {
    return value_;
}

Symbol::Symbol(const std::string& name) : name_(name) {
}

const std::string& Symbol::GetName() const {
    return name_;
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    Token current_token;
    try {
        current_token = tokenizer->GetToken();
    } catch (...) {
        throw SyntaxError("SyntaxError in Read_1");
    }
    if (ConstantToken* number_token = std::get_if<ConstantToken>(&current_token)) {
        std::shared_ptr<Number> head = std::make_shared<Number>(number_token->value);
        tokenizer->Next();
        if (!tokenizer->IsEnd()) {
            throw SyntaxError("SyntaxError in Read_2");
        }
        return head;
    } else if (SymbolToken* symbol_token = std::get_if<SymbolToken>(&current_token)) {
        std::shared_ptr<Symbol> head = std::make_shared<Symbol>(symbol_token->name);
        tokenizer->Next();
        if (!tokenizer->IsEnd()) {
            throw SyntaxError("SyntaxError in Read_3");
        }
        return head;
    } else if (BoolToken* bool_token = std::get_if<BoolToken>(&current_token)) {
        std::shared_ptr<Bool> head = std::make_shared<Bool>(bool_token->value);
        tokenizer->Next();
        if (!tokenizer->IsEnd()) {
            throw SyntaxError("SyntaxError in Read_4");
        }
        return head;
    } else if (BracketToken* bracket_token = std::get_if<BracketToken>(&current_token)) {
        if (*bracket_token == BracketToken::CLOSE) {
            throw SyntaxError("SyntaxError in Read_5");
        } else {
            std::shared_ptr<Cell> head = ReadList(tokenizer);
            tokenizer->Next();
//            if (!tokenizer->IsEnd()) {
//                throw SyntaxError("SyntaxError in Read_6");
//            }
            return head;
        }
    }
//    throw SyntaxError("SyntaxError in Read_7");
}

std::shared_ptr<Cell> ReadList(Tokenizer* tokenizer) {
    tokenizer->Next();
    if (tokenizer->IsEnd()) {
        throw SyntaxError("SyntaxError in ReadList_1");
    }
    Token current_token = tokenizer->GetToken();
    std::shared_ptr<Cell> head = std::make_shared<Cell>();
    if (ConstantToken* number_token = std::get_if<ConstantToken>(&current_token)) {
        head->first_ = std::make_shared<Number>(number_token->value);
    } else if (SymbolToken* symbol_token = std::get_if<SymbolToken>(&current_token)) {
        head->first_ = std::make_shared<Symbol>(symbol_token->name);
    } else if (BoolToken* bool_token = std::get_if<BoolToken>(&current_token)) {
        head->first_ = std::make_shared<Bool>(bool_token->value);
    } else if (BracketToken* bracket_token = std::get_if<BracketToken>(&current_token)) {
        if (*bracket_token == BracketToken::CLOSE) {
            head = nullptr;
            return head;
        } else {
            head->first_ = ReadList(tokenizer);
        }
    } else {
        throw SyntaxError("SyntaxError in ReadList_3");
    }

    if (!tokenizer->NextIsDot()) {
        head->second_ = ReadList(tokenizer);
        return head;
    }

    tokenizer->Next();
    if (tokenizer->IsEnd()) {
//        throw SyntaxError("SyntaxError in ReadList_4");
    }

    tokenizer->Next();
    if (tokenizer->IsEnd()) {
//        throw SyntaxError("SyntaxError in ReadList_6");
    }

    current_token = tokenizer->GetToken();
    if (ConstantToken* number_token = std::get_if<ConstantToken>(&current_token)) {
        head->second_ = std::make_shared<Number>(number_token->value);
    } else if (SymbolToken* symbol_token = std::get_if<SymbolToken>(&current_token)) {
        head->second_ = std::make_shared<Symbol>(symbol_token->name);
    } else if (BoolToken* bool_token = std::get_if<BoolToken>(&current_token)) {
        head->second_ = std::make_shared<Bool>(bool_token->value);
    } else if (BracketToken* bracket_token = std::get_if<BracketToken>(&current_token)) {
        if (*bracket_token == BracketToken::CLOSE) {
//            throw SyntaxError("SyntaxError in ReadList_7");
        } else {
            head->second_ = ReadList(tokenizer);
        }
    } else {
//        throw SyntaxError("SyntaxError in ReadList_8");
    }

    tokenizer->Next();
    current_token = tokenizer->GetToken();
//    if (BracketToken* bracket_token = std::get_if<BracketToken>(&current_token);
//        bracket_token == nullptr) {
//        throw SyntaxError("SyntaxError in ReadList_9");
//    }

//    if (tokenizer->IsEnd()) {
//        throw SyntaxError("SyntaxError in ReadList_4");
//    }

    return head;
}