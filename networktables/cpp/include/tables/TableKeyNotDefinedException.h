/*
 * TableKeyNotDefinedException.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef TABLEKEYNOTDEFINEDEXCEPTION_H_
#define TABLEKEYNOTDEFINEDEXCEPTION_H_


class TableKeyNotDefinedException;


#include <exception>
#include <string>


/**
 * An exception throw when the lookup a a key-value fails in a {@link ITable}
 * 
 * @author Mitchell
 *
 */
class TableKeyNotDefinedException : public std::exception {
public:
	/**
	 * @param key the key that was not defined in the table
	 */
	TableKeyNotDefinedException(const std::string key);
	~TableKeyNotDefinedException() throw();
	const char* what();
private:
	const std::string msg;
};




#endif /* TABLEKEYNOTDEFINEDEXCEPTION_H_ */
