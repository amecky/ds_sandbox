#pragma once
#include <stdint.h>

namespace vm {
	
	struct Token {

		enum TokenType {EMPTY, NUMBER,FUNCTION,VARIABLE,LEFT_PARENTHESIS, RIGHT_PARENTHESIS};

		Token() : type(EMPTY), value(0.0f) {}
		Token(TokenType t) : type(t) , value(0.0f) {}
		Token(TokenType type, uint16_t id) : type(type), id(id) {}
		Token(TokenType type, float value) : type(type), value(value) {}

		TokenType type;
		union {
			uint16_t id;
			float value;
		};
	};

	enum ValueType {
		VT_VARIABLE,VT_CONSTANT
	};

	struct Variable {
		ValueType type;
		const char* name;
		float value;
	};

	struct Context {

		uint16_t num_variables;
		Variable* variables;
	};

	const char* get_token_name(Token::TokenType type);

	uint16_t parse(const char* source, const Context& ctx, Token* tokens, uint16_t capacity);

	float run(Token* byteCode, uint16_t capacity, const Context& ctx);

	void print_bytecode(Token* byteCode, uint16_t num);

}

#ifdef DS_VM_IMPLEMENTATION

#include <string.h>
#include <stdio.h>
#include <math.h>
namespace vm {

	const static float PI = 3.141592654f;

	// ------------------------------------------------------------------
	// token names
	// ------------------------------------------------------------------
	const char* TOKEN_NAMES[] = { "EMPTY", "NUMBER", "FUNCTION", "VARIABLE", "LEFT_PARENTHESIS", "RIGHT_PARENTHESIS" };
	
	// ------------------------------------------------------------------
	// get token name
	// ------------------------------------------------------------------
	const char* get_token_name(Token::TokenType type) {
		return TOKEN_NAMES[type % 6];
	}

	// ------------------------------------------------------------------
	// op codes
	// ------------------------------------------------------------------
	enum OpCode { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_UNARY_MINUS, OP_NOP, OP_SIN, OP_COS, OP_ABS, OP_RAMP, OP_LERP, OP_RANGE };

	// ------------------------------------------------------------------
	// Function definition
	// ------------------------------------------------------------------
	struct Function {
		const char* name;
		OpCode code;
		uint8_t precedence;
		uint8_t num_parameters;
	};

	// ------------------------------------------------------------------
	// Functions
	// ------------------------------------------------------------------
	const static Function FUNCTIONS[] = {
		{ ",", OP_NOP, 1, 0 },
		{ "+", OP_ADD, 12,2 },
		{ "-", OP_SUB, 12, 2 },
		{ "*", OP_MUL, 13,2 },
		{ "/", OP_DIV, 13,2 },
		{ "u-", OP_UNARY_MINUS, 16, 1 },
		{ "u+", OP_NOP, 1, 0 },
		{ "sin", OP_SIN, 17, 1 },
		{ "cos", OP_COS, 17, 1 },
		{ "abs", OP_ABS, 17, 1 },
		{ "ramp", OP_RAMP, 17, 2 },
		{ "lerp", OP_LERP, 17, 3 },
		{ "range", OP_RANGE, 17, 3 }
	};

	const uint16_t NUM_FUNCTIONS = 13;

	// ------------------------------------------------------------------
	// find variable
	// ------------------------------------------------------------------
	static uint16_t find_variable(const char *s, uint16_t len, const Context& ctx) {
		for (uint16_t i = 0; i < ctx.num_variables; ++i) {
			if (strncmp(s, ctx.variables[i].name, len) == 0 && strlen(ctx.variables[i].name) == len) {
				return i;
			}
		}
		return UINT16_MAX;
	}

	// ------------------------------------------------------------------
	// find function
	// ------------------------------------------------------------------
	static uint16_t find_function(const char *s, uint16_t len) {
		for (uint16_t i = 0; i < NUM_FUNCTIONS; ++i) {
			if (strncmp(s, FUNCTIONS[i].name, len) == 0 && strlen(FUNCTIONS[i].name) == len) {
				return i;
			}
		}
		return UINT16_MAX;
	}

	// ------------------------------------------------------------------
	// get token for identifier
	// ------------------------------------------------------------------
	static Token token_for_identifier(const char *identifier, unsigned len, const Context& ctx) {
		uint16_t i;
		if ((i = find_variable(identifier, len, ctx)) != UINT16_MAX) {
			return Token(Token::VARIABLE, i);
		}
		else if ((i = find_function(identifier, len)) != UINT16_MAX) {
			return Token(Token::FUNCTION, i);
		}
		else {
			return Token();
		}
	}

	// ------------------------------------------------------------------
	// get token for identifier
	// ------------------------------------------------------------------
	static Token token_for_identifier(const char *identifier, const Context& ctx) {
		return token_for_identifier(identifier, strlen(identifier), ctx);
	}

	// ------------------------------------------------------------------
	// has function
	// ------------------------------------------------------------------
	static bool has_function(char * identifier) {
		return find_function(identifier, strlen(identifier)) != UINT16_MAX;
	}

	// ------------------------------------------------------------------
	// is numeric
	// ------------------------------------------------------------------
	static bool isNumeric(const char c) {
		return ((c >= '0' && c <= '9'));
	}

	// ------------------------------------------------------------------
	// is whitespace
	// ------------------------------------------------------------------
	static bool isWhitespace(const char c) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------
	// strtof
	// ------------------------------------------------------------------
	static float strtof(const char* p, char** endPtr) {
		while (isWhitespace(*p)) {
			++p;
		}
		float sign = 1.0f;
		if (*p == '-') {
			sign = -1.0f;
			++p;
		}
		else if (*p == '+') {
			++p;
		}
		float value = 0.0f;
		while (isNumeric(*p)) {
			value *= 10.0f;
			value = value + (*p - '0');
			++p;
		}
		if (*p == '.') {
			++p;
			float dec = 1.0f;
			float frac = 0.0f;
			while (isNumeric(*p)) {
				frac *= 10.0f;
				frac = frac + (*p - '0');
				dec *= 10.0f;
				++p;
			}
			value = value + (frac / dec);
		}
		if (endPtr) {
			*endPtr = (char *)(p);
		}
		return value * sign;
	}





	// ------------------------------------------------------------------
	// Function stack item
	// ------------------------------------------------------------------
	struct FunctionStackItem {
		FunctionStackItem() {}
		FunctionStackItem(Token t, int p, int pl) : token(t), precedence(p), par_level(pl) {}
		inline int cmp(const FunctionStackItem &f) const {
			if (par_level != f.par_level) return par_level - f.par_level;
			return precedence - f.precedence;
		}
		inline bool operator<(const FunctionStackItem &other) const { return cmp(other) < 0; }
		inline bool operator<=(const FunctionStackItem &other) const { return cmp(other) <= 0; }
		inline bool operator==(const FunctionStackItem &other) const { return cmp(other) == 0; }
		inline bool operator>=(const FunctionStackItem &other) const { return cmp(other) >= 0; }
		inline bool operator>(const FunctionStackItem &other) const { return cmp(other) > 0; }
		Token token;
		int precedence;
		int par_level;
	};

	// ------------------------------------------------------------------
	// Stack
	// ------------------------------------------------------------------
	struct Stack {

		float* data;
		uint16_t size;
		uint16_t capacity;

		inline float pop() { return data[--size]; }
		inline void push(float f) { data[size++] = f; }
	};

	// ------------------------------------------------------------------
	// parse
	// ------------------------------------------------------------------
	uint16_t parse(const char * source, const Context& ctx, Token * byteCode, uint16_t capacity) {
		printf("source: '%s'\n", source);
		bool binary = false;
		const char* p = source;
		unsigned num_tokens = 0;
		unsigned overflow_tokens = 0;
		Token* tokens = new Token[capacity];
		while (*p != 0) {
			Token token(Token::EMPTY);
			if (*p >= '0' && *p <= '9') {
				char *out;
				token = Token(Token::NUMBER, strtof(p, &out));
				p = out;
				binary = true;
			}
			else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_')) {
				const char *identifier = p;
				while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_') || (*p >= '0' && *p <= '9'))
					p++;
				token = token_for_identifier(identifier, p - identifier, ctx);
				binary = true;
			}
			else {
				switch (*p) {
				case '(': token = Token(Token::LEFT_PARENTHESIS); binary = false; break;
				case ')': token = Token(Token::RIGHT_PARENTHESIS); binary = true; break;
				case ' ': case '\t': case '\n': case '\r': break;
				case '-': token = token_for_identifier(binary ? "-" : "u-", ctx); binary = false; break;
				case '+': token = token_for_identifier(binary ? "+" : "u+", ctx); binary = false; break;

				default: {
					char s1[2] = { *p,0 };
					char s2[3] = { *p, *(p + 1), 0 };
					if (s2[1] && has_function(s2)) {
						token = token_for_identifier(s2, ctx);
						++p;
					}
					else
						token = token_for_identifier(s1, ctx);
					binary = false;
					break;
				}
				}
				++p;
			}

			if (token.type != Token::EMPTY) {
				if (num_tokens == capacity)
					++overflow_tokens;
				else
					tokens[num_tokens++] = token;
			}
		}

		uint16_t num_rpl = 0;
		FunctionStackItem function_stack[256];

		unsigned num_function_stack = 0;

		int par_level = 0;
		for (unsigned i = 0; i<num_tokens; ++i) {
			Token &token = tokens[i];
			switch (token.type) {
				case Token::NUMBER:
				case Token::VARIABLE:
					byteCode[num_rpl++] = token;
					break;
				case Token::LEFT_PARENTHESIS:
					++par_level;
					break;
				case Token::RIGHT_PARENTHESIS:
					--par_level;
					break;
				case Token::FUNCTION: {
					FunctionStackItem f(token, FUNCTIONS[token.id].precedence, par_level);
					while (num_function_stack>0 && function_stack[num_function_stack - 1] >= f)
						byteCode[num_rpl++] = function_stack[--num_function_stack].token;
					function_stack[num_function_stack++] = f;
					break;
				}
			}
		}

		while (num_function_stack > 0) {
			byteCode[num_rpl++] = function_stack[--num_function_stack].token;
		}
		delete[] tokens;
		return num_rpl;

	}

	// ------------------------------------------------------------------
	// print byte code
	// ------------------------------------------------------------------
	void print_bytecode(Token* byteCode, uint16_t num) {
		printf("--------------------------------------\n");
		printf("num: %d\n", num);
		for (uint16_t i = 0; i < num; ++i) {
			byteCode[i] = byteCode[i];
			if (byteCode[i].type == Token::NUMBER) {
				printf("%d = %s (%g)\n", i, get_token_name(byteCode[i].type), byteCode[i].value);
			}
			else if (byteCode[i].type == Token::VARIABLE) {
				printf("%d = %s (%d)\n", i, get_token_name(byteCode[i].type), byteCode[i].id);
			}
			else {
				printf("%d = %s\n", i, get_token_name(byteCode[i].type));
			}
		}
		printf("--------------------------------------\n");
	}

	// ------------------------------------------------------------------
	// run
	// ------------------------------------------------------------------
	float run(Token* byteCode, uint16_t capacity, const Context& ctx) {
		float stack_data[32] = { 0.0f };
		Stack stack = { stack_data, 0, 32 };
		float a, b, t;
		for (uint16_t i = 0; i < capacity; ++i) {
			if (byteCode[i].type == vm::Token::NUMBER) {
				stack.push(byteCode[i].value);
			}
			else if (byteCode[i].type == vm::Token::VARIABLE) {
				stack.push(ctx.variables[byteCode[i].id].value);
			}
			else if (byteCode[i].type == vm::Token::FUNCTION) {
				uint16_t id = byteCode[i].id;
				const Function& f = FUNCTIONS[id];
				switch (f.code) {
					case OP_ADD: stack.push(stack.pop() + stack.pop()); break;
					case OP_SUB: a = stack.pop(); b = stack.pop(); stack.push(b - a);break;
					case OP_MUL: stack.push(stack.pop() * stack.pop()); break;
					case OP_DIV: a = stack.pop(); b = stack.pop(); stack.push(b / a); break;
					case OP_SIN: stack.push(sin(stack.pop())); break;
					case OP_COS: stack.push(cos(stack.pop())); break;
					case OP_ABS: stack.push(abs(stack.pop())); break;
					case OP_RAMP: a = stack.pop(); t = stack.pop(); stack.push(a >= t ? 1.0f : 0.0f); break;
					case OP_RANGE: t = stack.pop(); b = stack.pop(); a = stack.pop(); stack.push(t >= a && t <= b ? 1.0f : 0.0f); break;
					case OP_LERP: t = stack.pop(); a = stack.pop(); b= stack.pop(); stack.push((1.0f - t) * b + t * a); break;
				}
			}
		}
		return stack.pop();
	}

}

#endif
