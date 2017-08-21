#pragma once
#include <stdint.h>

namespace vm {

	struct Token {

		enum TokenType {EMPTY, NUMBER,FUNCTION,VARIABLE,LEFT_PARENTHESIS, RIGHT_PARENTHESIS};

		Token() : type(EMPTY), value(0.0f) {}
		Token(TokenType t) : type(t) , value(0.0f) {}
		Token(TokenType type, uint16_t id) : type(type), id(id) {}
		Token(TokenType type, float value) : type(type), value(value) {}

		TokenType type;		///< Identifies the type of the token
		union {
			uint16_t id;		///< Id for FUNCTION and VARIABLE tokens
			float value;		///< Numeric value for NUMBER tokens
		};
	};

	const char* get_token_name(Token::TokenType type);

	uint16_t parse(const char* source, Token* tokens, uint16_t capacity);
}
