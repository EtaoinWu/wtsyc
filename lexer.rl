// ragel_subtype=cpp

#include <string>
#include <vector>
#include <memory>

#include "lexer.hpp"

using namespace std;

%%{
    machine rule;
    main := |*
        space;

        "="     { return SysY::Parser::make_ASSIGN(tokenRange()); };
    *|;
}%%

namespace {
// Static, const tables of data.
%% write data;
}

namespace SysY {

    Lexer::Lexer(string_view source) : source(source), p(source.begin()), pe(source.end()), eof(source.end()),
        cs(rule_start), ts(source.begin()), te(source.begin()), act(0) {}

    Range Lexer::tokenRange() const {
        return {ts - source.begin(), te - source.begin()};
    }

    SysY::Parser::symbol_type Lexer::exec() {
        %% write exec;
        return SysY::Parser::make_EOF(tokenRange());
    }

    SysY::Parser::symbol_type Lexer::operator()() {
        auto result = this->exec();
        this->p++;
        return result;
    }

}
