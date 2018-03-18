#include "CSVFile.h"
#include <fstream>
#include <assert.h>

// ------------------------------------------------------
// CSVLine
// ------------------------------------------------------
TextLine::TextLine(const std::string& str) : _delimiter(',') {
	set(str);
}

void TextLine::set(const std::string& str,const char delimiter) {
	_content = str;
	_num_delimiters = 0;
	_delimiter = delimiter;
	for ( int i = 0; i < _content.length();++i ) {
		if ( _content[i] == delimiter ) {
			++_num_delimiters;
		}
	}	
}

int TextLine::num_tokens() const {
	return _num_delimiters + 1;
}

void TextLine::print() const {
	printf("%s\n", _content.c_str());
}
// ------------------------------------------------------
// find pos in string for field index
// ------------------------------------------------------
int TextLine::find_pos(int field_index) const {
	if ( field_index == 0 ) {
		return 0;
	}
	int cnt = 0;
	for ( int i = 0; i < _content.length();++i ) {
		if ( _content[i] == _delimiter ) {
			++cnt;
		}
		if ( cnt == field_index ) {
			return i + 1;
		}
	}
	return -1;
}

// ------------------------------------------------------
// get int
// ------------------------------------------------------
bool TextLine::get(int index,int* ret) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		int npos = find_pos(index+1);
		if ( npos != -1 ) {
			std::string str = _content.substr(idx,npos-idx-1);		
			sscanf(str.c_str(),"%d",ret);
		}
		else {
			std::string str = _content.substr(idx);		
			sscanf(str.c_str(),"%d",ret);
		}
		return true;
	}
	return false;
}

bool TextLine::get(int index, ds::vec4* ret) const {
	for (int i = 0; i < 4; ++i) {
		if (!get(index + i, &ret->data[i])) {
			return false;
		}
		return true;
	}
	return false;
}

bool TextLine::get(int index, ds::vec3* ret) const {
	for (int i = 0; i < 3; ++i) {
		if (!get(index + i, &ret->data[i])) {
			return false;
		}
		return true;
	}
	return false;
}

bool TextLine::get(int index, ds::vec2* ret) const {
	for (int i = 0; i < 2; ++i) {
		if (!get(index + i, &ret->data[i])) {
			return false;
		}
		return true;
	}
	return false;
}
// ------------------------------------------------------
// get float
// ------------------------------------------------------
bool TextLine::get(int index, float* ret) const {
	int idx = find_pos(index);
	if (idx != -1) {
		int npos = find_pos(index + 1);
		if (npos != -1) {
			std::string str = _content.substr(idx, npos - idx - 1);
			sscanf(str.c_str(), "%g", ret);
		}
		else {
			std::string str = _content.substr(idx);
			sscanf(str.c_str(), "%g", ret);
		}
		return true;
	}
	return false;
}

// ------------------------------------------------------
// get string
// ------------------------------------------------------
bool TextLine::get(int index,char* dest,size_t* length) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		int npos = find_pos(index+1);
		if ( npos != -1 ) {			
			std::string str = _content.substr(idx,npos-idx-1);		
			strcpy(dest,str.c_str());
			*length = str.length();
			return true;
		}
		else {
			std::string str = _content.substr(idx);		
			strcpy(dest,str.c_str());
			*length = str.length();
			return true;
		}
	}
	return false;
}

// ------------------------------------------------------
// get bool
// ------------------------------------------------------
bool TextLine::get(int index, bool* ret) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		char c = _content[idx];
		if ( c == 'N' || c == 'n' ) {
			*ret = false;
		}
		else {
			*ret = true;
		}
		return true;		
	}
	return false;
}


// ------------------------------------------------------
// CSVFile
// ------------------------------------------------------
CSVFile::CSVFile() {}


CSVFile::~CSVFile() {}

// ------------------------------------------------------
// load
// ------------------------------------------------------
bool CSVFile::load(const char* fileName,const char* directory) {
	char buffer[256];
	sprintf(buffer,"%s\\%s",directory,fileName);
	std::string line;
	std::ifstream myfile(buffer);
	_lines.clear();
	if (myfile.is_open()) {
		while (std::getline(myfile, line)) {
			if (line.find("#") == std::string::npos && line.find(",") != std::string::npos) {
				_lines.push_back(TextLine(line));					
			}
		}
		myfile.close();
		return true;
	}
	return false;
}

// ------------------------------------------------------
// get line
// ------------------------------------------------------
const TextLine& CSVFile::get(int index) const {
	assert(index >= 0 && index < _lines.size());
	return _lines[index];
}

// ------------------------------------------------------
// size
// ------------------------------------------------------
const size_t CSVFile::size() const {
	return _lines.size();
}