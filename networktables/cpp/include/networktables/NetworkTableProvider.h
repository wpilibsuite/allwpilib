#ifndef _NETWORKTABLEPROVIDER_H_
#define _NETWORKTABLEPROVIDER_H_


class NetworkTableProvider;

#include <string>

#include "tables/ITableProvider.h"
#include "networktables2/NetworkTableNode.h"
#include "networktables/NetworkTable.h"

class NetworkTableProvider : public ITableProvider
{
private:
	NetworkTableNode& node;
	std::map<std::string, NetworkTable*> tables;

	/**
	 * Create a new NetworkTableProvider for a given NetworkTableNode
	 * @param node the node that handles the actual network table
	 */
public:
	NetworkTableProvider(NetworkTableNode& node);
	virtual ~NetworkTableProvider();

	ITable* GetRootTable();

	ITable* GetTable(std::string key);

	/**
	 * @return the Network Table node that backs the Tables returned by this provider
	 */
	NetworkTableNode& GetNode() {
		return node;
	};

	/**
	 * close the backing network table node
	 */
	void Close() {
		node.Close();
	};
};

#endif
