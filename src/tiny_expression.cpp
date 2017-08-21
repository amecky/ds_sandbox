#include "tiny_expression.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

namespace vm {

	const char* TOKEN_NAMES[] = { "EMPTY", "NUMBER", "FUNCTION", "VARIABLE", "LEFT_PARENTHESIS", "RIGHT_PARENTHESIS" };

	const char* CONSTANT_NAMES[] = { "pi","two_pi" };

	const float CONSTANT_VALUES[] = { 123.0f, 456.0f };

	const char* get_token_name(Token::TokenType type) {
		return TOKEN_NAMES[type % 6];
	}

	struct CompilerEnvironment {

		uint16_t num_functions;
		const char **function_names;
		uint16_t num_constants;
		const char **constant_names;

		static uint16_t find_string(const char *s, uint16_t len, uint16_t num_strings, const char **strings) {
			for (uint16_t i = 0; i < num_strings; ++i) {
				if (strncmp(s, strings[i], len) == 0 && strlen(strings[i]) == len) {
					return i;
				}
			}
			return UINT16_MAX;
		}

		/// Finds a token representing the identifier in the environment.
		Token token_for_identifier(const char *identifier, unsigned len) const {
			uint16_t i;
			//if ((i = find_string(identifier, len, num_variables, variable_names)) != UINT_MAX)
				//return Token(Token::VARIABLE, i);
			if ((i = find_string(identifier, len, num_constants, constant_names)) != UINT16_MAX) {
				return Token(Token::NUMBER, CONSTANT_VALUES[i]);
			}
			else if ((i = find_string(identifier, len, num_functions, function_names)) != UINT16_MAX)
				return Token(Token::FUNCTION, i);
			else {
				return Token();
			}
		}

		/// Finds a token representing the identifier in the environment.
		Token token_for_identifier(const char *identifier) const {
			return token_for_identifier(identifier, strlen(identifier));
		}

		/// True if there is a function matching the specified identifier.
		bool has_function(char * identifier) const {
			return find_string(identifier, strlen(identifier), num_functions, function_names) != UINT16_MAX;
		}
	};

	static bool isNumeric(const char c) {
		return ((c >= '0' && c <= '9'));
	}

	static bool isWhitespace(const char c) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			return true;
		}
		return false;
	}

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

	const char* FUNCTION_NAMES[] = { ",","+","-","*","/","u-","u+","sin","cos","abs" };

	enum OpCode { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_UNARY_MINUS, OP_NOP, OP_SIN, OP_COS, OP_ABS };

	struct Function {
		OpCode code;
		uint8_t precedence;
		uint8_t num_parameters;
	};

	const static Function FUNCTIONS[] = {
		{ OP_NOP, 1, 0 },
		{ OP_ADD, 12,2},
		{ OP_SUB, 12, 2},
		{ OP_MUL, 13,2},
		{ OP_DIV, 13,2},
		{ OP_UNARY_MINUS, 16, 1 },		
		{ OP_NOP, 1, 0 },
		{ OP_SIN, 17, 1},
		{ OP_COS, 17, 1},
		{ OP_ABS, 17, 1 }
	};

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

	static void prepare_environment(CompilerEnvironment* env) {
		env->function_names = FUNCTION_NAMES;
		env->num_functions = 10;
		env->num_constants = 2;
		env->constant_names = CONSTANT_NAMES;
	}

	struct Stack {

		float* data;
		uint16_t size;
		uint16_t capacity;

		inline float pop() { return data[--size]; }
		inline void push(float f) { data[size++] = f; }
	};

	uint16_t parse(const char * source, Token * tokens, uint16_t capacity) {

		CompilerEnvironment env;
		prepare_environment(&env);
		// Determines if the next + or - is a binary or unary operator.
		bool binary = false;
		const char* p = source;
		unsigned num_tokens = 0;
		unsigned overflow_tokens = 0;

		while (*p != 0) {
			Token token(Token::EMPTY);

			// Numbers
			if (*p >= '0' && *p <= '9') {
				char *out;
				token = Token(Token::NUMBER, strtof(p, &out));
				p = out;
				binary = true;
				// Identifiers
			}
			else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_')) {
				const char *identifier = p;
				while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_') || (*p >= '0' && *p <= '9'))
					p++;
				token = env.token_for_identifier(identifier, p - identifier);
				binary = true;
				// Operators
			}
			else {
				switch (*p) {
				case '(': token = Token(Token::LEFT_PARENTHESIS); binary = false; break;
				case ')': token = Token(Token::RIGHT_PARENTHESIS); binary = true; break;
				case ' ': case '\t': case '\n': case '\r': break;
				case '-': token = env.token_for_identifier(binary ? "-" : "u-"); binary = false; break;
				case '+': token = env.token_for_identifier(binary ? "+" : "u+"); binary = false; break;

				default: {
					char s1[2] = { *p,0 };
					char s2[3] = { *p, *(p + 1), 0 };
					if (s2[1] && env.has_function(s2)) {
						token = env.token_for_identifier(s2);
						++p;
					}
					else
						token = env.token_for_identifier(s1);
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
		Token rpl[256];
		FunctionStackItem function_stack[256];

		unsigned num_function_stack = 0;

		int par_level = 0;
		for (unsigned i = 0; i<num_tokens; ++i) {
			Token &token = tokens[i];
			switch (token.type) {
			case Token::NUMBER:
			case Token::VARIABLE:
				rpl[num_rpl++] = token;
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
					rpl[num_rpl++] = function_stack[--num_function_stack].token;
				function_stack[num_function_stack++] = f;
				break;
			}
			}
		}

		while (num_function_stack>0)
			rpl[num_rpl++] = function_stack[--num_function_stack].token;


		printf("num: %d\n", num_tokens);
		for (uint16_t i = 0; i < num_tokens; ++i) {
			if (tokens[i].type == vm::Token::NUMBER) {
				printf("%d = %s (%g)\n", i, vm::get_token_name(tokens[i].type), tokens[i].value);
			}
			else {
				printf("%d = %s\n", i, vm::get_token_name(tokens[i].type));
			}
		}
		printf("--------------------------------------\n");
		for (uint16_t i = 0; i < num_rpl; ++i) {
			if (rpl[i].type == vm::Token::NUMBER) {
				printf("%d = %s (%g)\n", i, vm::get_token_name(rpl[i].type), rpl[i].value);
			}
			else if (rpl[i].type == vm::Token::FUNCTION) {
				printf("%d = %s (%s)\n", i, vm::get_token_name(rpl[i].type), FUNCTION_NAMES[rpl[i].id]);
			}
			else {
				printf("%d = %s\n", i, vm::get_token_name(rpl[i].type));
			}
		}

		float stack_data[256] = { 0.0f };
		Stack stack = { stack_data,0,256 };

		for (uint16_t i = 0; i < num_rpl; ++i) {
			if (rpl[i].type == vm::Token::NUMBER) {
				stack.push(rpl[i].value);
			}
			else if (rpl[i].type == vm::Token::FUNCTION) {
				uint16_t id = rpl[i].id;
				const Function& f = FUNCTIONS[id];
				if (f.code == OP_ADD) {
					stack.push(stack.pop() + stack.pop());
				}
				if (f.code == OP_SUB) {
					stack.push(stack.pop() - stack.pop());
				}
				if (f.code == OP_MUL) {
					stack.push(stack.pop() * stack.pop());
				}
				if (f.code == OP_DIV) {
					stack.push(stack.pop() / stack.pop());
				}
				if (f.code == OP_SIN) {
					stack.push(sin(stack.pop()));
				}
			}
		}
		printf("result: %g\n", stack.pop());

		return num_tokens + overflow_tokens;

	}

}
