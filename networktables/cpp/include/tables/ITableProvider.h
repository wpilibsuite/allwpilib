/*
 * ITableProvider.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef ITABLEPROVIDER_H_
#define ITABLEPROVIDER_H_


class ITableProvider;

#include "tables/ITable.h"


/**
 * A simple interface to provide tables
 * 
 * @author Mitchell
 *
 */
class ITableProvider {
public:
	/**
	 * Get a table by name
	 * @param name the name of the table
	 * @return a Table with the given name
	 */
	virtual ITable* GetTable(std::string name) = 0;
};

#endif /* ITABLEPROVIDER_H_ */
