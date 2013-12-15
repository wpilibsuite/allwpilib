#ifndef _STRINGCACHE_H_
#define _STRINGCACHE_H_

#include <map>
#include <string>

using namespace std;

/**
 * A simple cache that allows for caching the mapping of one string to another calculated one
 * 
 * @author Mitchell
 *
 */
class StringCache {
private:
	map<std::string, std::string> cache;

	
	/**
	 * @param input
	 * @return the value for a given input
	 */
public:
	StringCache();
	virtual ~StringCache();
	
	
	std::string& Get(const std::string& input);
	
	/**
	 * Will only be called if a value has not already been calculated
	 * @param input
	 * @return the calculated value for a given input
	 */
	virtual std::string Calc(const std::string& input) = 0;
};

#endif
