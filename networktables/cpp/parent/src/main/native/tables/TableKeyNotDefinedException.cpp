/*
 * TableKeyNotDefinedException.cpp
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#include "tables/TableKeyNotDefinedException.h"
#include <stdlib.h>
#include <cstring>


#define MESSAGE_PREFIX "Unkown Table Key: "
TableKeyNotDefinedException::TableKeyNotDefinedException(const std::string key):
msg(MESSAGE_PREFIX + key){

}

const char* TableKeyNotDefinedException::what(){
	return msg.c_str();
}

TableKeyNotDefinedException::~TableKeyNotDefinedException() throw(){
}
