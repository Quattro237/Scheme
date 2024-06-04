#include <tokenizer.h>

#include <set>
#include <stdexcept>
#include <cctype>
#include <string>

const static std::set<char> kSymbols = {'<', '=', '>', '*', '/', '#', '?', '!', '+', '-'};

namespace {
bool IsSymbolPart(const char ch) {
    if (kSymbols.find(ch) != kSymbols.end() && ch != '+') {
        return true;
    }
    if (isdigit(ch)) {
        return true;
    }
    if (isalpha(ch)) {
        return true;
    }
    return false;
}

bool IsSymbolBegin(const char ch) {
    if (kSymbols.find(ch) != kSymbols.end() && ch != '?' && ch != '!') {
        return true;
    }
    if (isalpha(ch)) {
        return true;
    }
    return false;
}
}  // namespace

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name == other.name;
}

bool BoolToken::operator==(const BoolToken &other) const {
    return value == other.value;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream *in) : input_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

Token Tokenizer::GetToken() {
    if (has_token_) {
        return current_;
    }
    throw std::runtime_error("No token inside");
}

void Tokenizer::Next() {
    while (input_->peek() != EOF) {
        char cur_ch = input_->get();
        if (cur_ch == '(') {
            current_ = BracketToken::OPEN;
            has_token_ = true;
            return;
        } else if (cur_ch == ')') {
            current_ = BracketToken::CLOSE;
            has_token_ = true;
            return;
        } else if (cur_ch == '\'') {
            current_ = QuoteToken();
            has_token_ = true;
            return;
        } else if (cur_ch == '.') {
            current_ = DotToken();
            has_token_ = true;
            return;
        } else if ((cur_ch == '+' && isdigit(input_->peek())) ||
                   (cur_ch == '-' && isdigit(input_->peek())) || isdigit(cur_ch)) {
            std::string str_number;
            str_number += cur_ch;

            while (isdigit(input_->peek())) {
                cur_ch = input_->get();
                str_number += cur_ch;
            }

            int number = std::stoi(str_number);

            current_ = ConstantToken{number};
            has_token_ = true;
            return;
        } else if (IsSymbolBegin(cur_ch)) {
            std::string symbol;
            symbol += cur_ch;

            while (IsSymbolPart(input_->peek())) {
                cur_ch = input_->get();
                symbol += cur_ch;
            }

            if (symbol == "#f") {
                current_ = BoolToken{false};
                has_token_ = true;
                return;
            } else if (symbol == "#t") {
                current_ = BoolToken{true};
                has_token_ = true;
                return;
            }

            current_ = SymbolToken{symbol};
            has_token_ = true;
            return;
        }
    }
    is_end_ = true;
}

bool Tokenizer::NextIsDot() {
    while (input_->peek() != EOF) {
        char cur_ch = input_->peek();
        if (cur_ch == '(') {
            return false;
        } else if (cur_ch == ')') {
            return false;
        } else if (cur_ch == '\'') {
            return false;
        } else if (cur_ch == '.') {
            return true;
        } else if ((cur_ch == '+' && isdigit(input_->peek())) ||
                   (cur_ch == '-' && isdigit(input_->peek())) || isdigit(cur_ch)) {
            return false;
        } else if (IsSymbolBegin(cur_ch)) {
            return false;
        } else if (cur_ch == ' ') {
            input_->get();
        }
    }
    return false;
}
