#pragma once
#include <string>
#include <vector>
#include <diesel.h>
// ------------------------------------------------------
// TextLine
// ------------------------------------------------------
class TextLine {

public:
	TextLine() {}
	TextLine(const std::string& str);
	~TextLine() {}
	void set(const std::string& str,const char delimiter = ',');
	int find_pos(int field_index) const;
	bool get(int index, int* ret) const;
	bool get(int index, float* ret) const;
	bool get(int index, ds::vec2* ret) const;
	bool get(int index, ds::vec3* ret) const;
	bool get(int index, ds::vec4* ret) const;
	bool get(int index,bool* ret) const;
	bool get(int index,char* dest, size_t* length) const;
	int num_tokens() const;
	void print() const;
private:
	int _num_delimiters;
	std::string _content;
	char _delimiter;
};

// ------------------------------------------------------
// CSVFile
// ------------------------------------------------------
class CSVFile {

typedef std::vector<TextLine> Lines;

public:
	CSVFile();
	~CSVFile();
	bool load(const char* fileName,const char* directory);
	const TextLine& get(int index) const;
	const size_t size() const;
private:
	Lines _lines;
};

