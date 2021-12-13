
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


#line 65 "/home/etaoin/wtsyc/lexer.rl"


namespace {
// Static, const tables of data.

#line 24 "/home/etaoin/wtsyc/lexer.cpp"
static const char _rule_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 21, 1, 22, 1, 
	23, 1, 24, 1, 25, 1, 26, 1, 
	27, 1, 28, 1, 29, 1, 30, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 1, 37, 1, 38, 1, 
	39, 1, 40, 1, 41, 1, 42, 1, 
	43, 1, 44, 1, 45, 1, 46, 1, 
	47, 1, 48, 2, 3, 5, 2, 3, 
	6, 2, 3, 7, 2, 3, 8, 2, 
	3, 9, 2, 3, 10, 2, 3, 11, 
	2, 3, 12, 2, 3, 13, 2, 3, 
	14, 2, 3, 15, 3, 3, 0, 4
	
};

static const short _rule_key_offsets[] = {
	0, 0, 1, 3, 6, 7, 13, 14, 
	53, 54, 56, 57, 61, 63, 69, 71, 
	72, 73, 74, 81, 89, 97, 105, 113, 
	121, 129, 138, 146, 154, 162, 170, 178, 
	186, 194, 202, 211, 219, 227, 235, 243, 
	251, 259, 267, 275, 283, 291, 299, 307
};

static const char _rule_trans_keys[] = {
	38, 10, 42, 10, 42, 47, 10, 48, 
	57, 65, 70, 97, 102, 124, 10, 32, 
	33, 37, 38, 40, 41, 42, 43, 44, 
	45, 46, 47, 48, 59, 60, 61, 62, 
	91, 93, 95, 98, 99, 101, 105, 114, 
	118, 119, 123, 124, 125, 9, 13, 49, 
	57, 65, 90, 97, 122, 61, 42, 47, 
	10, 88, 120, 48, 55, 48, 55, 48, 
	57, 65, 70, 97, 102, 48, 57, 61, 
	61, 61, 95, 48, 57, 65, 90, 97, 
	122, 95, 114, 48, 57, 65, 90, 97, 
	122, 95, 101, 48, 57, 65, 90, 97, 
	122, 95, 97, 48, 57, 65, 90, 98, 
	122, 95, 107, 48, 57, 65, 90, 97, 
	122, 95, 111, 48, 57, 65, 90, 97, 
	122, 95, 110, 48, 57, 65, 90, 97, 
	122, 95, 115, 116, 48, 57, 65, 90, 
	97, 122, 95, 116, 48, 57, 65, 90, 
	97, 122, 95, 105, 48, 57, 65, 90, 
	97, 122, 95, 110, 48, 57, 65, 90, 
	97, 122, 95, 117, 48, 57, 65, 90, 
	97, 122, 95, 101, 48, 57, 65, 90, 
	97, 122, 95, 108, 48, 57, 65, 90, 
	97, 122, 95, 115, 48, 57, 65, 90, 
	97, 122, 95, 101, 48, 57, 65, 90, 
	97, 122, 95, 102, 110, 48, 57, 65, 
	90, 97, 122, 95, 116, 48, 57, 65, 
	90, 97, 122, 95, 101, 48, 57, 65, 
	90, 97, 122, 95, 116, 48, 57, 65, 
	90, 97, 122, 95, 117, 48, 57, 65, 
	90, 97, 122, 95, 114, 48, 57, 65, 
	90, 97, 122, 95, 110, 48, 57, 65, 
	90, 97, 122, 95, 111, 48, 57, 65, 
	90, 97, 122, 95, 105, 48, 57, 65, 
	90, 97, 122, 95, 100, 48, 57, 65, 
	90, 97, 122, 95, 104, 48, 57, 65, 
	90, 97, 122, 95, 105, 48, 57, 65, 
	90, 97, 122, 95, 108, 48, 57, 65, 
	90, 97, 122, 95, 101, 48, 57, 65, 
	90, 97, 122, 0
};

static const char _rule_single_lengths[] = {
	0, 1, 2, 3, 1, 0, 1, 31, 
	1, 2, 1, 2, 0, 0, 0, 1, 
	1, 1, 1, 2, 2, 2, 2, 2, 
	2, 3, 2, 2, 2, 2, 2, 2, 
	2, 2, 3, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2
};

static const char _rule_range_lengths[] = {
	0, 0, 0, 0, 0, 3, 0, 4, 
	0, 0, 0, 1, 1, 3, 1, 0, 
	0, 0, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3
};

static const short _rule_index_offsets[] = {
	0, 0, 2, 5, 9, 11, 15, 17, 
	53, 55, 58, 60, 64, 66, 70, 72, 
	74, 76, 78, 83, 89, 95, 101, 107, 
	113, 119, 126, 132, 138, 144, 150, 156, 
	162, 168, 174, 181, 187, 193, 199, 205, 
	211, 217, 223, 229, 235, 241, 247, 253
};

static const char _rule_indicies[] = {
	0, 1, 4, 5, 3, 4, 5, 6, 
	3, 9, 8, 11, 11, 11, 10, 12, 
	1, 14, 13, 15, 16, 17, 18, 19, 
	20, 21, 22, 23, 24, 25, 26, 28, 
	29, 30, 31, 33, 34, 32, 35, 36, 
	37, 38, 39, 40, 41, 42, 43, 44, 
	13, 27, 32, 32, 1, 46, 45, 3, 
	8, 47, 9, 8, 51, 51, 50, 49, 
	50, 49, 11, 11, 11, 49, 27, 49, 
	53, 52, 55, 54, 57, 56, 32, 32, 
	32, 32, 7, 32, 59, 32, 32, 32, 
	58, 32, 60, 32, 32, 32, 58, 32, 
	61, 32, 32, 32, 58, 32, 62, 32, 
	32, 32, 58, 32, 63, 32, 32, 32, 
	58, 32, 64, 32, 32, 32, 58, 32, 
	65, 66, 32, 32, 32, 58, 32, 67, 
	32, 32, 32, 58, 32, 68, 32, 32, 
	32, 58, 32, 69, 32, 32, 32, 58, 
	32, 70, 32, 32, 32, 58, 32, 71, 
	32, 32, 32, 58, 32, 72, 32, 32, 
	32, 58, 32, 73, 32, 32, 32, 58, 
	32, 74, 32, 32, 32, 58, 32, 75, 
	76, 32, 32, 32, 58, 32, 77, 32, 
	32, 32, 58, 32, 78, 32, 32, 32, 
	58, 32, 79, 32, 32, 32, 58, 32, 
	80, 32, 32, 32, 58, 32, 81, 32, 
	32, 32, 58, 32, 82, 32, 32, 32, 
	58, 32, 83, 32, 32, 32, 58, 32, 
	84, 32, 32, 32, 58, 32, 85, 32, 
	32, 32, 58, 32, 86, 32, 32, 32, 
	58, 32, 87, 32, 32, 32, 58, 32, 
	88, 32, 32, 32, 58, 32, 89, 32, 
	32, 32, 58, 0
};

static const char _rule_trans_targs[] = {
	7, 0, 7, 2, 2, 3, 7, 7, 
	4, 10, 7, 13, 7, 7, 7, 8, 
	7, 1, 7, 7, 7, 7, 7, 7, 
	7, 9, 11, 14, 7, 15, 16, 17, 
	18, 7, 7, 19, 23, 31, 34, 36, 
	41, 44, 7, 6, 7, 7, 7, 7, 
	7, 7, 12, 5, 7, 7, 7, 7, 
	7, 7, 7, 20, 21, 22, 18, 24, 
	25, 26, 27, 18, 28, 29, 30, 18, 
	32, 33, 18, 18, 35, 18, 37, 38, 
	39, 40, 18, 42, 43, 18, 45, 46, 
	47, 18
};

static const char _rule_trans_actions[] = {
	49, 0, 69, 0, 1, 0, 13, 73, 
	0, 108, 71, 0, 51, 11, 9, 0, 
	39, 0, 19, 21, 37, 33, 27, 35, 
	29, 75, 7, 0, 31, 0, 0, 0, 
	105, 15, 17, 0, 0, 0, 0, 0, 
	0, 0, 23, 0, 25, 63, 47, 57, 
	53, 65, 0, 0, 61, 43, 55, 45, 
	59, 41, 67, 0, 0, 0, 87, 0, 
	0, 0, 0, 96, 0, 0, 0, 90, 
	0, 0, 81, 78, 0, 99, 0, 0, 
	0, 0, 93, 0, 0, 102, 0, 0, 
	0, 84
};

static const char _rule_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 3, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const char _rule_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 5, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const short _rule_eof_trans[] = {
	0, 0, 3, 3, 8, 11, 0, 0, 
	46, 48, 49, 50, 50, 50, 50, 53, 
	55, 57, 8, 59, 59, 59, 59, 59, 
	59, 59, 59, 59, 59, 59, 59, 59, 
	59, 59, 59, 59, 59, 59, 59, 59, 
	59, 59, 59, 59, 59, 59, 59, 59
};

static const int rule_start = 7;
static const int rule_first_final = 7;
static const int rule_error = 0;

static const int rule_en_main = 7;


#line 70 "/home/etaoin/wtsyc/lexer.rl"
}

namespace SysY {
  Parser::symbol_type make_INT_LITERAL(const std::string &val, Range r) {
    errno = 0;
    long n = strtol (val.c_str(), NULL, 0);
    if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
      throw Parser::syntax_error(r, "integer is out of range: " + val);
    return Parser::make_INT_LITERAL(static_cast<int>(n), r);
  }

  Lexer::Lexer(string_view source) : source(source), p(source.begin()), pe(source.end()), eof(source.end()),
    cs(rule_start), ts(source.begin()), te(source.begin()), act(0) {}

  Range Lexer::tokenRange() const {
    return {ts - source.begin(), te - source.begin()};
  }

  Parser::symbol_type Lexer::exec() {
    
#line 243 "/home/etaoin/wtsyc/lexer.cpp"
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
	case 2:
#line 1 "NONE"
	{ts = p;}
	break;
#line 264 "/home/etaoin/wtsyc/lexer.cpp"
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
	case 0:
#line 16 "/home/etaoin/wtsyc/lexer.rl"
	{
    line_end(p);
  }
	break;
	case 3:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 4:
#line 23 "/home/etaoin/wtsyc/lexer.rl"
	{act = 4;}
	break;
	case 5:
#line 38 "/home/etaoin/wtsyc/lexer.rl"
	{act = 18;}
	break;
	case 6:
#line 49 "/home/etaoin/wtsyc/lexer.rl"
	{act = 29;}
	break;
	case 7:
#line 50 "/home/etaoin/wtsyc/lexer.rl"
	{act = 30;}
	break;
	case 8:
#line 51 "/home/etaoin/wtsyc/lexer.rl"
	{act = 31;}
	break;
	case 9:
#line 52 "/home/etaoin/wtsyc/lexer.rl"
	{act = 32;}
	break;
	case 10:
#line 53 "/home/etaoin/wtsyc/lexer.rl"
	{act = 33;}
	break;
	case 11:
#line 54 "/home/etaoin/wtsyc/lexer.rl"
	{act = 34;}
	break;
	case 12:
#line 55 "/home/etaoin/wtsyc/lexer.rl"
	{act = 35;}
	break;
	case 13:
#line 56 "/home/etaoin/wtsyc/lexer.rl"
	{act = 36;}
	break;
	case 14:
#line 57 "/home/etaoin/wtsyc/lexer.rl"
	{act = 37;}
	break;
	case 15:
#line 63 "/home/etaoin/wtsyc/lexer.rl"
	{act = 39;}
	break;
	case 16:
#line 16 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{
    line_end(p);
  }}
	break;
	case 17:
#line 21 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;}
	break;
	case 18:
#line 22 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;}
	break;
	case 19:
#line 25 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LBRACKET(tokenRange()); }}
	break;
	case 20:
#line 26 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_RBRACKET(tokenRange()); }}
	break;
	case 21:
#line 27 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LPAREN(tokenRange()); }}
	break;
	case 22:
#line 28 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_RPAREN(tokenRange()); }}
	break;
	case 23:
#line 29 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LCURLY(tokenRange()); }}
	break;
	case 24:
#line 30 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_RCURLY(tokenRange()); }}
	break;
	case 25:
#line 31 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_COMMA(tokenRange()); }}
	break;
	case 26:
#line 32 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_PERIOD(tokenRange()); }}
	break;
	case 27:
#line 33 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_SEMICOLON(tokenRange()); }}
	break;
	case 28:
#line 35 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_PLUS(tokenRange()); }}
	break;
	case 29:
#line 36 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_MINUS(tokenRange()); }}
	break;
	case 30:
#line 37 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_STAR(tokenRange()); }}
	break;
	case 31:
#line 39 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_MOD(tokenRange()); }}
	break;
	case 32:
#line 41 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_GE(tokenRange()); }}
	break;
	case 33:
#line 43 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LE(tokenRange()); }}
	break;
	case 34:
#line 44 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_EQ(tokenRange()); }}
	break;
	case 35:
#line 45 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_NEQ(tokenRange()); }}
	break;
	case 36:
#line 47 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LOGICAL_AND(tokenRange()); }}
	break;
	case 37:
#line 48 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_LOGICAL_OR(tokenRange()); }}
	break;
	case 38:
#line 23 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;}
	break;
	case 39:
#line 34 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_ASSIGN(tokenRange()); }}
	break;
	case 40:
#line 38 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_SLASH(tokenRange()); }}
	break;
	case 41:
#line 40 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_GT(tokenRange()); }}
	break;
	case 42:
#line 42 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_LT(tokenRange()); }}
	break;
	case 43:
#line 46 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_LOGICAL_NOT(tokenRange()); }}
	break;
	case 44:
#line 60 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return make_INT_LITERAL(std::string(ts, te - ts), tokenRange()); }}
	break;
	case 45:
#line 63 "/home/etaoin/wtsyc/lexer.rl"
	{te = p;p--;{ return SysY::Parser::make_IDENT(std::string(ts, te - ts), tokenRange()); }}
	break;
	case 46:
#line 38 "/home/etaoin/wtsyc/lexer.rl"
	{{p = ((te))-1;}{ return SysY::Parser::make_SLASH(tokenRange()); }}
	break;
	case 47:
#line 60 "/home/etaoin/wtsyc/lexer.rl"
	{{p = ((te))-1;}{ return make_INT_LITERAL(std::string(ts, te - ts), tokenRange()); }}
	break;
	case 48:
#line 1 "NONE"
	{	switch( act ) {
	case 18:
	{{p = ((te))-1;} return SysY::Parser::make_SLASH(tokenRange()); }
	break;
	case 29:
	{{p = ((te))-1;} return SysY::Parser::make_IF(tokenRange()); }
	break;
	case 30:
	{{p = ((te))-1;} return SysY::Parser::make_ELSE(tokenRange()); }
	break;
	case 31:
	{{p = ((te))-1;} return SysY::Parser::make_WHILE(tokenRange()); }
	break;
	case 32:
	{{p = ((te))-1;} return SysY::Parser::make_BREAK(tokenRange()); }
	break;
	case 33:
	{{p = ((te))-1;} return SysY::Parser::make_CONTINUE(tokenRange()); }
	break;
	case 34:
	{{p = ((te))-1;} return SysY::Parser::make_RETURN(tokenRange()); }
	break;
	case 35:
	{{p = ((te))-1;} return SysY::Parser::make_CONST(tokenRange()); }
	break;
	case 36:
	{{p = ((te))-1;} return SysY::Parser::make_TYPE(PrimitiveType::INT, tokenRange()); }
	break;
	case 37:
	{{p = ((te))-1;} return SysY::Parser::make_TYPE(PrimitiveType::VOID, tokenRange()); }
	break;
	case 39:
	{{p = ((te))-1;} return SysY::Parser::make_IDENT(std::string(ts, te - ts), tokenRange()); }
	break;
	default:
	{{p = ((te))-1;}}
	break;
	}
	}
	break;
#line 560 "/home/etaoin/wtsyc/lexer.cpp"
		}
	}

_again:
	_acts = _rule_actions + _rule_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 573 "/home/etaoin/wtsyc/lexer.cpp"
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

#line 90 "/home/etaoin/wtsyc/lexer.rl"
    return Parser::make_EOF(tokenRange());
  }

  Parser::symbol_type Lexer::operator()() {
    auto result = this->exec();
    this->p++;
    return result;
  }

  void Lexer::line_end(const std::string_view::const_iterator &it) {
    lines.push_back(it - source.begin() + 1);
  }

  Position Lexer::toPosition(ptrdiff_t d) const {
    auto it = std::upper_bound(lines.begin(), lines.end(), d) - 1;
    return Position{it - lines.begin() + 1, d - *it + 1};
  }
}
