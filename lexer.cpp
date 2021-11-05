
#line 1 "/home/etaoin/wtsyc/lexer.rl"
// ragel_subtype=cpp

#include <string>
#include <vector>
#include <memory>

#include "lexer.hpp"

using namespace std;


#line 19 "/home/etaoin/wtsyc/lexer.rl"


namespace {
// Static, const tables of data.

#line 21 "/home/etaoin/wtsyc/lexer.cpp"
static const char _rule_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4
};

static const char _rule_key_offsets[] = {
	0, 0
};

static const char _rule_trans_keys[] = {
	10, 32, 61, 9, 13, 0
};

static const char _rule_single_lengths[] = {
	0, 3
};

static const char _rule_range_lengths[] = {
	0, 1
};

static const char _rule_index_offsets[] = {
	0, 0
};

static const char _rule_trans_targs[] = {
	1, 1, 1, 1, 0, 0
};

static const char _rule_trans_actions[] = {
	5, 7, 9, 7, 0, 0
};

static const char _rule_to_state_actions[] = {
	0, 1
};

static const char _rule_from_state_actions[] = {
	0, 3
};

static const int rule_start = 1;
static const int rule_first_final = 1;
static const int rule_error = 0;

static const int rule_en_main = 1;


#line 24 "/home/etaoin/wtsyc/lexer.rl"
}

namespace SysY {

  Lexer::Lexer(string_view source) : source(source), p(source.begin()), pe(source.end()), eof(source.end()),
    cs(rule_start), ts(source.begin()), te(source.begin()), act(0) {}

  Range Lexer::tokenRange() const {
    return {ts - source.begin(), te - source.begin()};
  }

  SysY::Parser::symbol_type Lexer::exec() {
    
#line 84 "/home/etaoin/wtsyc/lexer.cpp"
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
#line 105 "/home/etaoin/wtsyc/lexer.cpp"
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
#line 14 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ line_end(p); }}
	break;
	case 3:
#line 15 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;}
	break;
	case 4:
#line 17 "/home/etaoin/wtsyc/lexer.rl"
	{te = p+1;{ return SysY::Parser::make_ASSIGN(tokenRange()); }}
	break;
#line 181 "/home/etaoin/wtsyc/lexer.cpp"
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
#line 194 "/home/etaoin/wtsyc/lexer.cpp"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 37 "/home/etaoin/wtsyc/lexer.rl"
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
