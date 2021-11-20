
#line 1 "/home/etaoin/wtsyc/lexer.rl"
// ragel_subtype=cpp

#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <climits>

#include "lexer.hpp"

using namespace std;


#line 60 "/home/etaoin/wtsyc/lexer.rl"


namespace {
// Static, const tables of data.

#line 24 "/home/etaoin/wtsyc/lexer.cpp"
static const char _rule_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	13, 1, 14, 1, 15, 1, 16, 1, 
	17, 1, 18, 1, 19, 1, 20, 1, 
	21, 1, 22, 1, 23, 1, 24, 1, 
	25, 1, 26, 1, 27, 1, 28, 1, 
	29, 1, 30, 1, 31, 1, 32, 1, 
	33, 1, 34, 1, 35, 1, 36, 1, 
	37, 1, 38, 1, 39, 1, 40, 1, 
	41, 1, 42, 2, 2, 3, 2, 2, 
	4, 2, 2, 5, 2, 2, 6, 2, 
	2, 7, 2, 2, 8, 2, 2, 9, 
	2, 2, 10, 2, 2, 11, 2, 2, 
	12
};

static const short _rule_key_offsets[] = {
	0, 0, 1, 7, 9, 10, 49, 50, 
	54, 56, 62, 64, 65, 66, 67, 74, 
	82, 90, 98, 106, 114, 122, 131, 139, 
	147, 155, 163, 171, 179, 187, 195, 204, 
	212, 220, 228, 236, 244, 252, 260, 268, 
	276, 284, 292, 300
};

static const char _rule_trans_keys[] = {
	38, 48, 57, 65, 70, 97, 102, 48, 
	57, 124, 10, 32, 33, 37, 38, 40, 
	41, 42, 43, 44, 45, 46, 47, 48, 
	59, 60, 61, 62, 91, 93, 95, 98, 
	99, 101, 105, 114, 118, 119, 123, 124, 
	125, 9, 13, 49, 57, 65, 90, 97, 
	122, 61, 88, 120, 48, 55, 48, 55, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	61, 61, 61, 95, 48, 57, 65, 90, 
	97, 122, 95, 114, 48, 57, 65, 90, 
	97, 122, 95, 101, 48, 57, 65, 90, 
	97, 122, 95, 97, 48, 57, 65, 90, 
	98, 122, 95, 107, 48, 57, 65, 90, 
	97, 122, 95, 111, 48, 57, 65, 90, 
	97, 122, 95, 110, 48, 57, 65, 90, 
	97, 122, 95, 115, 116, 48, 57, 65, 
	90, 97, 122, 95, 116, 48, 57, 65, 
	90, 97, 122, 95, 105, 48, 57, 65, 
	90, 97, 122, 95, 110, 48, 57, 65, 
	90, 97, 122, 95, 117, 48, 57, 65, 
	90, 97, 122, 95, 101, 48, 57, 65, 
	90, 97, 122, 95, 108, 48, 57, 65, 
	90, 97, 122, 95, 115, 48, 57, 65, 
	90, 97, 122, 95, 101, 48, 57, 65, 
	90, 97, 122, 95, 102, 110, 48, 57, 
	65, 90, 97, 122, 95, 116, 48, 57, 
	65, 90, 97, 122, 95, 101, 48, 57, 
	65, 90, 97, 122, 95, 116, 48, 57, 
	65, 90, 97, 122, 95, 117, 48, 57, 
	65, 90, 97, 122, 95, 114, 48, 57, 
	65, 90, 97, 122, 95, 110, 48, 57, 
	65, 90, 97, 122, 95, 111, 48, 57, 
	65, 90, 97, 122, 95, 105, 48, 57, 
	65, 90, 97, 122, 95, 100, 48, 57, 
	65, 90, 97, 122, 95, 104, 48, 57, 
	65, 90, 97, 122, 95, 105, 48, 57, 
	65, 90, 97, 122, 95, 108, 48, 57, 
	65, 90, 97, 122, 95, 101, 48, 57, 
	65, 90, 97, 122, 0
};

static const char _rule_single_lengths[] = {
	0, 1, 0, 0, 1, 31, 1, 2, 
	0, 0, 0, 1, 1, 1, 1, 2, 
	2, 2, 2, 2, 2, 3, 2, 2, 
	2, 2, 2, 2, 2, 2, 3, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2
};

static const char _rule_range_lengths[] = {
	0, 0, 3, 1, 0, 4, 0, 1, 
	1, 3, 1, 0, 0, 0, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3
};

static const short _rule_index_offsets[] = {
	0, 0, 2, 6, 8, 10, 46, 48, 
	52, 54, 58, 60, 62, 64, 66, 71, 
	77, 83, 89, 95, 101, 107, 114, 120, 
	126, 132, 138, 144, 150, 156, 162, 169, 
	175, 181, 187, 193, 199, 205, 211, 217, 
	223, 229, 235, 241
};

static const char _rule_indicies[] = {
	0, 1, 3, 3, 3, 2, 4, 1, 
	5, 1, 7, 6, 8, 9, 10, 11, 
	12, 13, 14, 15, 16, 17, 18, 19, 
	21, 22, 23, 24, 26, 27, 25, 28, 
	29, 30, 31, 32, 33, 34, 35, 36, 
	37, 6, 20, 25, 25, 1, 39, 38, 
	42, 42, 41, 40, 41, 40, 3, 3, 
	3, 40, 4, 40, 44, 43, 46, 45, 
	48, 47, 25, 25, 25, 25, 49, 25, 
	51, 25, 25, 25, 50, 25, 52, 25, 
	25, 25, 50, 25, 53, 25, 25, 25, 
	50, 25, 54, 25, 25, 25, 50, 25, 
	55, 25, 25, 25, 50, 25, 56, 25, 
	25, 25, 50, 25, 57, 58, 25, 25, 
	25, 50, 25, 59, 25, 25, 25, 50, 
	25, 60, 25, 25, 25, 50, 25, 61, 
	25, 25, 25, 50, 25, 62, 25, 25, 
	25, 50, 25, 63, 25, 25, 25, 50, 
	25, 64, 25, 25, 25, 50, 25, 65, 
	25, 25, 25, 50, 25, 66, 25, 25, 
	25, 50, 25, 67, 68, 25, 25, 25, 
	50, 25, 69, 25, 25, 25, 50, 25, 
	70, 25, 25, 25, 50, 25, 71, 25, 
	25, 25, 50, 25, 72, 25, 25, 25, 
	50, 25, 73, 25, 25, 25, 50, 25, 
	74, 25, 25, 25, 50, 25, 75, 25, 
	25, 25, 50, 25, 76, 25, 25, 25, 
	50, 25, 77, 25, 25, 25, 50, 25, 
	78, 25, 25, 25, 50, 25, 79, 25, 
	25, 25, 50, 25, 80, 25, 25, 25, 
	50, 25, 81, 25, 25, 25, 50, 0
};

static const char _rule_trans_targs[] = {
	5, 0, 5, 9, 10, 5, 5, 5, 
	6, 5, 1, 5, 5, 5, 5, 5, 
	5, 5, 5, 7, 3, 5, 11, 12, 
	13, 14, 5, 5, 15, 19, 27, 30, 
	32, 37, 40, 5, 4, 5, 5, 5, 
	5, 8, 2, 5, 5, 5, 5, 5, 
	5, 5, 5, 16, 17, 18, 14, 20, 
	21, 22, 23, 14, 24, 25, 26, 14, 
	28, 29, 14, 14, 31, 14, 33, 34, 
	35, 36, 14, 38, 39, 14, 41, 42, 
	43, 14
};

static const char _rule_trans_actions[] = {
	47, 0, 63, 0, 0, 49, 9, 7, 
	0, 37, 0, 15, 17, 33, 29, 23, 
	31, 25, 35, 5, 0, 27, 0, 0, 
	0, 94, 11, 13, 0, 0, 0, 0, 
	0, 0, 0, 19, 0, 21, 57, 45, 
	59, 0, 0, 55, 41, 51, 43, 53, 
	39, 65, 61, 0, 0, 0, 76, 0, 
	0, 0, 0, 85, 0, 0, 0, 79, 
	0, 0, 70, 67, 0, 88, 0, 0, 
	0, 0, 82, 0, 0, 91, 0, 0, 
	0, 73
};

static const char _rule_to_state_actions[] = {
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const char _rule_from_state_actions[] = {
	0, 0, 0, 0, 0, 3, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const short _rule_eof_trans[] = {
	0, 0, 3, 0, 0, 0, 39, 41, 
	41, 41, 41, 44, 46, 48, 50, 51, 
	51, 51, 51, 51, 51, 51, 51, 51, 
	51, 51, 51, 51, 51, 51, 51, 51, 
	51, 51, 51, 51, 51, 51, 51, 51, 
	51, 51, 51, 51
};

static const int rule_start = 5;
static const int rule_first_final = 5;
static const int rule_error = 0;

static const int rule_en_main = 5;


#line 65 "/home/etaoin/wtsyc/lexer.rl"
}

namespace SysY {
  Parser::symbol_type make_INT_LITERAL(const std::string &val, Range r) {
    errno = 0;
    long n = strtol (val.c_str(), NULL, 0);
    if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
      throw SysY::Parser::syntax_error(r, "integer is out of range: " + val);
    return SysY::Parser::make_INT_LITERAL(static_cast<int>(n), r);
  }

  Lexer::Lexer(string_view source) : source(source), p(source.begin()), pe(source.end()), eof(source.end()),
    cs(rule_start), ts(source.begin()), te(source.begin()), act(0) {}

  Range Lexer::tokenRange() const {
    return {ts - source.begin(), te - source.begin()};
  }

  SysY::Parser::symbol_type Lexer::exec() {
    
#line 236 "/home/etaoin/wtsyc/lexer.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _rule_actions + _rule_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 1 "NONE"
	{ts = p;}
	break;
#line 257 "/home/etaoin/wtsyc/lexer.cpp"
		}
	}

	_keys = _rule_trans_keys + _rule_key_offsets[cs];
	_trans = _rule_index_offsets[cs];

	_klen = _rule_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _rule_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _rule_indicies[_trans];
_eof_trans:
	cs = _rule_trans_targs[_trans];

	if ( _rule_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _rule_actions + _rule_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 2:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 3:
#line 44 "/home/etaoin/wtsyc/lexer.rl"
	{act = 27;}
	break;
	case 4:
#line 45 "/home/etaoin/wtsyc/lexer.rl"
	{act = 28;}
	break;
	case 5:
#line 46 "/home/etaoin/wtsyc/lexer.rl"
	{act = 29;}
	break;
	case 6:
#line 47 "/home/etaoin/wtsyc/lexer.rl"
	{act = 30;}
	break;
	case 7:
#line 48 "/home/etaoin/wtsyc/lexer.rl"
	{act = 31;}
	break;
	case 8:
#line 49 "/home/etaoin/wtsyc/lexer.rl"
	{act = 32;}
	break;
	case 9:
#line 50 "/home/etaoin/wtsyc/lexer.rl"
	{act = 33;}
	break;
	case 10:
#line 51 "/home/etaoin/wtsyc/lexer.rl"
	{act = 34;}
	break;
	case 11:
#line 52 "/home/etaoin/wtsyc/lexer.rl"
	{act = 35;}
	break;
	case 12:
#line 58 "/home/etaoin/wtsyc/lexer.rl"
	{act = 37;}
	break;
	case 13:
#line 17 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ line_end(p); }}
	break;
	case 14:
#line 18 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;}
	break;
	case 15:
#line 20 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LBRACKET(tokenRange()); }}
	break;
	case 16:
#line 21 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_RBRACKET(tokenRange()); }}
	break;
	case 17:
#line 22 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LPAREN(tokenRange()); }}
	break;
	case 18:
#line 23 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_RPAREN(tokenRange()); }}
	break;
	case 19:
#line 24 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LCURLY(tokenRange()); }}
	break;
	case 20:
#line 25 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_RCURLY(tokenRange()); }}
	break;
	case 21:
#line 26 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_COMMA(tokenRange()); }}
	break;
	case 22:
#line 27 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_PERIOD(tokenRange()); }}
	break;
	case 23:
#line 28 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_SEMICOLON(tokenRange()); }}
	break;
	case 24:
#line 30 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_PLUS(tokenRange()); }}
	break;
	case 25:
#line 31 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_MINUS(tokenRange()); }}
	break;
	case 26:
#line 32 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_STAR(tokenRange()); }}
	break;
	case 27:
#line 33 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_SLASH(tokenRange()); }}
	break;
	case 28:
#line 34 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_MOD(tokenRange()); }}
	break;
	case 29:
#line 36 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_GE(tokenRange()); }}
	break;
	case 30:
#line 38 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LE(tokenRange()); }}
	break;
	case 31:
#line 39 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_EQ(tokenRange()); }}
	break;
	case 32:
#line 40 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_NEQ(tokenRange()); }}
	break;
	case 33:
#line 42 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LOGICAL_AND(tokenRange()); }}
	break;
	case 34:
#line 43 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LOGICAL_OR(tokenRange()); }}
	break;
	case 35:
#line 29 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_ASSIGN(tokenRange()); }}
	break;
	case 36:
#line 35 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_GT(tokenRange()); }}
	break;
	case 37:
#line 37 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_LT(tokenRange()); }}
	break;
	case 38:
#line 41 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_LOGICAL_NOT(tokenRange()); }}
	break;
	case 39:
#line 55 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return make_INT_LITERAL(std::string(ts, te - ts), tokenRange()); }}
	break;
	case 40:
#line 58 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_IDENT(std::string(ts, te - ts), tokenRange()); }}
	break;
	case 41:
#line 55 "/home/etaoin/wtsyc/lexer.rl"
	{{p = ((te))-1;}{ return make_INT_LITERAL(std::string(ts, te - ts), tokenRange()); }}
	break;
	case 42:
#line 1 "NONE"
	{	switch( act ) {
	case 27:
	{{p = ((te))-1;} return SysY::Parser::make_IF(tokenRange()); }
	break;
	case 28:
	{{p = ((te))-1;} return SysY::Parser::make_ELSE(tokenRange()); }
	break;
	case 29:
	{{p = ((te))-1;} return SysY::Parser::make_WHILE(tokenRange()); }
	break;
	case 30:
	{{p = ((te))-1;} return SysY::Parser::make_BREAK(tokenRange()); }
	break;
	case 31:
	{{p = ((te))-1;} return SysY::Parser::make_CONTINUE(tokenRange()); }
	break;
	case 32:
	{{p = ((te))-1;} return SysY::Parser::make_RETURN(tokenRange()); }
	break;
	case 33:
	{{p = ((te))-1;} return SysY::Parser::make_CONST(tokenRange()); }
	break;
	case 34:
	{{p = ((te))-1;} return SysY::Parser::make_TYPE(PrimitiveType::INT, tokenRange()); }
	break;
	case 35:
	{{p = ((te))-1;} return SysY::Parser::make_TYPE(PrimitiveType::VOID, tokenRange()); }
	break;
	case 37:
	{{p = ((te))-1;} return SysY::Parser::make_IDENT(std::string(ts, te - ts), tokenRange()); }
	break;
	}
	}
	break;
#line 519 "/home/etaoin/wtsyc/lexer.cpp"
		}
	}

_again:
	_acts = _rule_actions + _rule_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 0:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 532 "/home/etaoin/wtsyc/lexer.cpp"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _rule_eof_trans[cs] > 0 ) {
		_trans = _rule_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 85 "/home/etaoin/wtsyc/lexer.rl"
    return SysY::Parser::make_EOF(tokenRange());
  }

  SysY::Parser::symbol_type Lexer::operator()() {
    auto result = this->exec();
    this->p++;
    return result;
  }

  void SysY::Lexer::line_end(const std::string_view::const_iterator &it) {
    lines.push_back(it - source.begin());
  }
}
