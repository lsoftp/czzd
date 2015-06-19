#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "datastruct.h"
#include <string>
#include <map>
#define COMMENT_CHAR '#'

class Param
{
public:
	DWORD id;
	string value;
	Param(DWORD i, string s);
	Param(){};
};

class Config
{
public:
	Config();


	
	map<string, Param> m_paramlist;
};
#endif

