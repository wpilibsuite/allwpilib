#include <map>
#include <string>

#include "tables/ITableProvider.h"
#include "networktables2/NetworkTableNode.h"
#include "networktables/NetworkTable.h"


#include "networktables/NetworkTableProvider.h"



using namespace std;

		

NetworkTableProvider::NetworkTableProvider(NetworkTableNode& _node) : node(_node){}
NetworkTableProvider::~NetworkTableProvider(){
  while(tables.size()>0){
    map<std::string, NetworkTable*>::iterator it = tables.begin();
    delete it->second;
    tables.erase(it);
  }
}

ITable* NetworkTableProvider::GetRootTable(){
	return GetTable("");
}

ITable* NetworkTableProvider::GetTable(std::string key){
	if(tables.find(key) != tables.end())
	{
		return tables[key];
	}
	else
	{
		NetworkTable* table = new NetworkTable(key, *this);
		tables[key] = table;
		return table;
	}
}

