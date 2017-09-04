#pragma once
#include <stdint.h>

namespace vm {

	struct CharBuffer {

		char* data;
		uint16_t size;
		uint16_t capacity;
		uint16_t num;

		CharBuffer();
		~CharBuffer();

		void* alloc(uint16_t sz);
		void resize(uint16_t newCap);
		uint16_t append(const char* s, int len);
		uint16_t append(const char* s);
		uint16_t append(char s);
		const char* get(uint16_t index) const;

	};

	struct Token {

		enum TokenType { EMPTY, NUMBER, FUNCTION, VARIABLE, LEFT_PARENTHESIS, RIGHT_PARENTHESIS };

		Token() : type(EMPTY), value(0.0f) {}
		Token(TokenType t) : type(t), value(0.0f) {}
		Token(TokenType type, uint16_t id) : type(type), id(id) {}
		Token(TokenType type, float value) : type(type), value(value) {}

		TokenType type;
		union {
			uint16_t id;
			float value;
		};
	};

	enum ValueType {
		VT_VARIABLE, VT_CONSTANT
	};

	struct Variable {
		ValueType type;
		uint16_t nameIndex;
		float value;
	};

	struct Context {

		uint16_t num_variables;
		Variable variables[32];
		CharBuffer names;

		Context() : num_variables(0) {}

		uint16_t add_variable(const char* name, float value = 0.0f) {
			Variable& v = variables[num_variables++];
			v.nameIndex = names.append(name);
			v.type = VT_VARIABLE;
			v.value = value;
			return num_variables - 1;
		}

		uint16_t add_variable(const char* name, int length, float value = 0.0f) {
			Variable& v = variables[num_variables++];
			v.nameIndex = names.append(name, length);
			v.type = VT_VARIABLE;
			v.value = value;
			return num_variables - 1;
		}

		uint16_t add_constant(const char* name, float value) {
			Variable& v = variables[num_variables++];
			v.nameIndex = names.append(name);
			v.type = VT_CONSTANT;
			v.value = value;
			return num_variables - 1;
		}

	};

	struct Expression {
		vm::Token* tokens;
		uint16_t num;
		uint16_t id;

		Expression() : num(0), id(0), tokens(0) {}
		~Expression() {
			if (tokens != 0) {
				delete[] tokens;
			}
		}
	};

	const char* get_token_name(Token::TokenType type);

	uint16_t parse(const char* source, Context& ctx, Token* tokens, uint16_t capacity);

	void parse(const char* source, Context& ctx, Expression& e);

	float run(Token* byteCode, uint16_t capacity, Context& ctx, const char* result_name = 0);

	void print_bytecode(const Context& ctx, Token* byteCode, uint16_t num);

	void print_bytecode(const Context& ctx, const Expression& exp);

}

#ifdef DS_VM_IMPLEMENTATION
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <random>

namespace vm {

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
	enum OpCode { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_UNARY_MINUS, OP_NOP, OP_SIN, OP_COS, OP_ABS, OP_RAMP, OP_LERP, OP_RANGE, OP_SATURATE, OP_RANDOM, OP_CHANNEL };

	const char* FUNCTION_NAMES[] = { ",","+","-","*","/","u-","u+","sin","cos","abs","ramp","lerp","range","saturate","random", "channel" };
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
		{ FUNCTION_NAMES[0], OP_NOP, 1, 0 },
		{ FUNCTION_NAMES[1], OP_ADD, 12,2 },
		{ FUNCTION_NAMES[2], OP_SUB, 12, 2 },
		{ FUNCTION_NAMES[3], OP_MUL, 13,2 },
		{ FUNCTION_NAMES[4], OP_DIV, 13,2 },
		{ FUNCTION_NAMES[5], OP_UNARY_MINUS, 16, 1 },
		{ FUNCTION_NAMES[6], OP_NOP, 1, 0 },
		{ FUNCTION_NAMES[7], OP_SIN, 17, 1 },
		{ FUNCTION_NAMES[8], OP_COS, 17, 1 },
		{ FUNCTION_NAMES[9], OP_ABS, 17, 1 },
		{ FUNCTION_NAMES[10], OP_RAMP, 17, 2 },
		{ FUNCTION_NAMES[11], OP_LERP, 17, 3 },
		{ FUNCTION_NAMES[12], OP_RANGE, 17, 3 },
		{ FUNCTION_NAMES[13], OP_SATURATE, 17, 3 },
		{ FUNCTION_NAMES[14], OP_RANDOM, 17, 2 },
		{ FUNCTION_NAMES[15], OP_RANDOM, 17, 1 }
	};

	const uint16_t NUM_FUNCTIONS = 16;

	const char* get_function_name(uint16_t id) {
		return FUNCTIONS[id].name;
	}

	// ------------------------------------------------------------------
	// find variable
	// ------------------------------------------------------------------
	static uint16_t find_variable(const char *s, uint16_t len, const Context& ctx) {
		for (uint16_t i = 0; i < ctx.num_variables; ++i) {
			const char* varName = ctx.names.get(ctx.variables[i].nameIndex);
			if (strncmp(s, varName, len) == 0 && strlen(varName) == len) {
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
	// get token for identifier
	// ------------------------------------------------------------------
	static Token token_for_identifier(const char *identifier, unsigned len, Context& ctx) {
		uint16_t i;
		char tmp[256];
		const char* p = identifier;
		int l = 0;
		while (l < len) {
			tmp[l++] = *p;
			++p;
		}
		tmp[l] = '\0';
		if ((i = find_variable(identifier, len, ctx)) != UINT16_MAX) {
			return Token(Token::VARIABLE, i);
		}
		else if ((i = find_function(identifier, len)) != UINT16_MAX) {
			return Token(Token::FUNCTION, i);
		}
		else {
			int l = 0;
			const char* p = identifier;
			while (!isWhitespace(*p)) {
				++l;
				++p;
			}
			i = ctx.add_variable(identifier, l);
			return Token(Token::VARIABLE, i);
		}
	}

	// ------------------------------------------------------------------
	// get token for identifier
	// ------------------------------------------------------------------
	static Token token_for_identifier(const char *identifier, Context& ctx) {
		return token_for_identifier(identifier, strlen(identifier), ctx);
	}

	// ------------------------------------------------------------------
	// has function
	// ------------------------------------------------------------------
	static bool has_function(char * identifier) {
		return find_function(identifier, strlen(identifier)) != UINT16_MAX;
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

	// -------------------------------------------------------
	// random
	// -------------------------------------------------------
	static std::mt19937 mt;
	static bool rnd_first = true;

	static void init_random(unsigned long seed) {
		std::random_device r;
		std::seed_seq new_seed{ r(), r(), r(), r(), r(), r(), r(), r() };
		mt.seed(new_seed);
	}


	float random(float min, float max) {
		if (rnd_first) {
			rnd_first = false;
			init_random(0);
		}
		std::uniform_real_distribution<float> dist(min, max);
		return dist(mt);
	}
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
	uint16_t parse(const char * source, Context& ctx, Token * byteCode, uint16_t capacity) {
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
					else {
						token = token_for_identifier(s1, ctx);
					}
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
	// parse
	// ------------------------------------------------------------------
	void parse(const char * source, Context& ctx, Expression& exp) {
		bool binary = false;
		const char* p = source;
		uint16_t num_tokens = 0;
		uint16_t overflow_tokens = 0;
		Token tokens[512];
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
					else {
						token = token_for_identifier(s1, ctx);
					}
					binary = false;
					break;
				}
				}
				++p;
			}

			if (token.type != Token::EMPTY) {
				if (num_tokens == 512)
					++overflow_tokens;
				else
					tokens[num_tokens++] = token;
			}
		}

		exp.tokens = new Token[num_tokens];

		uint16_t num_rpl = 0;
		FunctionStackItem function_stack[256];

		unsigned num_function_stack = 0;

		int par_level = 0;
		for (unsigned i = 0; i<num_tokens; ++i) {
			Token &token = tokens[i];
			switch (token.type) {
			case Token::NUMBER:
			case Token::VARIABLE:
				exp.tokens[num_rpl++] = token;
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
					exp.tokens[num_rpl++] = function_stack[--num_function_stack].token;
				function_stack[num_function_stack++] = f;
				break;
			}
			}
		}

		while (num_function_stack > 0) {
			exp.tokens[num_rpl++] = function_stack[--num_function_stack].token;
		}
		exp.num = num_rpl;
	}

	// ------------------------------------------------------------------
	// print byte code
	// ------------------------------------------------------------------
	void print_bytecode(const Context& ctx, Token* byteCode, uint16_t num) {
		printf("--------------------------------------\n");
		printf("num: %d\n", num);
		printf("variables:\n");
		for (uint16_t i = 0; i < ctx.num_variables; ++i) {
			printf("%d = %s : %g\n", i, ctx.names.get(ctx.variables[i].nameIndex), ctx.variables[i].value);
		}
		for (uint16_t i = 0; i < num; ++i) {
			byteCode[i] = byteCode[i];
			if (byteCode[i].type == Token::NUMBER) {
				printf("%d = %s (%g)\n", i, get_token_name(byteCode[i].type), byteCode[i].value);
			}
			else if (byteCode[i].type == Token::VARIABLE) {
				printf("%d = %s (%d)\n", i, get_token_name(byteCode[i].type), byteCode[i].id);
			}
			else {
				printf("%d = %s (%s)\n", i, get_token_name(byteCode[i].type), get_function_name(byteCode[i].id));
			}
		}
		printf("--------------------------------------\n");
	}

	void print_bytecode(const Context& ctx, const Expression& exp) {
		print_bytecode(ctx, exp.tokens, exp.num);
	}

	// ------------------------------------------------------------------
	// run
	// ------------------------------------------------------------------
	float run(Token* byteCode, uint16_t capacity, Context& ctx, const char* result_name) {
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
				case OP_SUB: a = stack.pop(); b = stack.pop(); stack.push(b - a); break;
				case OP_MUL: stack.push(stack.pop() * stack.pop()); break;
				case OP_DIV: a = stack.pop(); b = stack.pop(); stack.push(b / a); break;
				case OP_SIN: stack.push(sin(stack.pop())); break;
				case OP_COS: stack.push(cos(stack.pop())); break;
				case OP_ABS: stack.push(abs(stack.pop())); break;
				case OP_RAMP: a = stack.pop(); t = stack.pop(); stack.push(a >= t ? 1.0f : 0.0f); break;
				case OP_RANGE: t = stack.pop(); b = stack.pop(); a = stack.pop(); stack.push(t >= a && t <= b ? 1.0f : 0.0f); break;
				case OP_LERP: t = stack.pop(); a = stack.pop(); b = stack.pop(); stack.push((1.0f - t) * b + t * a); break;
				case OP_SATURATE: a = stack.pop(); if (a < 0.0f) stack.push(0.0f); else if (a > 1.0f) stack.push(1.0f); else stack.push(a); break;
				case OP_RANDOM: a = stack.pop(); b = stack.pop(); stack.push(random(b, a)); break;
				case OP_CHANNEL: a = stack.pop(); stack.push(a); break;
				}
			}
		}
		float r = 0.0f;
		if (stack.size > 0) {
			r = stack.pop();
		}
		if (result_name != 0) {
			uint16_t id = find_variable(result_name, strlen(result_name), ctx);
			if (id != UINT16_MAX) {
				ctx.variables[id].value = r;
			}
		}
		return r;
	}

	CharBuffer::CharBuffer() : data(nullptr), size(0), capacity(0), num(0) {}

	CharBuffer::~CharBuffer() {
		if (data != nullptr) {
			delete[] data;
		}
	}

	void* CharBuffer::alloc(uint16_t sz) {
		if (size + sz > capacity) {
			int d = capacity * 2 + 8;
			if (d < sz) {
				d = sz * 2 + 8;
			}
			resize(d);
		}
		auto res = data + size;
		size += sz;
		int d = sz / 4;
		if (d == 0) {
			d = 1;
		}
		num += d;
		return res;
	}

	void CharBuffer::resize(uint16_t newCap) {
		if (newCap > capacity) {
			char* tmp = new char[newCap];
			if (data != nullptr) {
				memcpy(tmp, data, size);
				delete[] data;
			}
			capacity = newCap;
			data = tmp;
		}
	}

	const char* CharBuffer::get(uint16_t index) const {
		return data + index;
	}

	uint16_t CharBuffer::append(const char* s, int len) {
		if (size + len + 1 > capacity) {
			resize(capacity + len + 1 + 8);
		}
		const char* t = s;
		uint16_t ret = size;
		for (int i = 0; i < len; ++i) {
			data[size++] = *t;
			++t;
		}
		data[size++] = '\0';
		return ret;
	}

	uint16_t CharBuffer::append(const char* s) {
		int len = strlen(s);
		return append(s, len);
	}

	uint16_t CharBuffer::append(char s) {
		if (size + 1 > capacity) {
			resize(capacity + 9);
		}
		uint16_t ret = size;
		data[size++] = s;
		data[size++] = '\0';
		return ret;
	}
}

#endif
