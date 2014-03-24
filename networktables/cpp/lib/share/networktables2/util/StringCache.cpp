/*
 * StringCache.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Mitchell Wills
 */

#include <map>
#include "networktables2/util/StringCache.h"

using namespace std;

StringCache::StringCache(){
}
StringCache::~StringCache(){
}

std::string& StringCache::Get(const std::string& input){
	map<std::string, std::string>::iterator itr = cache.find(input);
	if(itr != cache.end()){
	   return itr->second;
	}
	else{
		cache[input] = Calc(input);
		return cache[input];
	}
}
