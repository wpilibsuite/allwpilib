#include "networktables2/server/NetworkTableServer.h"
#include "networktables2/type/NetworkTableEntryTypeManager.h"
#include "networktables2/stream/SocketServerStreamProvider.h"
#include "networktables2/thread/DefaultThreadManager.h"
#include "tables/ITableListener.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableProvider.h"
#include "tables/ITable.h"
#include "networktables2/type/NumberArray.h"


NetworkTableServer* server;

int main(){
  NetworkTable::SetServerMode();
  NetworkTable::Initialize();
  
  /*class ServerListener : public ITableListener {
      virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew){
	std::string clientString("/client/");
	if(key.startsWith(clientString)) {
	    std::string name = key.substring(std::string("/client/").length());
	    std::string newName("/server/");
	    newName += name;
	    server->PutDouble(newName, value.f);
          }
      };
      };

  ServerListener* listener = new ServerListener();
  server->AddTableListener(listener, true);*/

  ITable* table = NetworkTable::GetTable("SmartDashboard");
  ITable* cTable = NetworkTable::GetTable("/client");
  class ServerListener : public ITableListener {
      virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew){
	fprintf(stdout, "Got key in client table: %s, = %f\n", key.c_str(), value.f);
	fflush(stdout);
	NetworkTable::GetTable("/server")->PutNumber(key, value.f);
      };
  };
  ServerListener* listener = new ServerListener();
  cTable->AddTableListener(listener, true);
  

  table->PutString("TEST", "hello");
  table->PutString("TEST", "hellofasd");
  table->PutString("TEST", "hellod");
  table->PutString("TEST", "helloddd");
  table->PutString("TEST", "hellodsasd");
  for(int i = 0; i<20; ++i)
	  table->PutString("TEST", "helloffff");

  NumberArray array;
  for(int i = 0; i<5; ++i){
    array.add(i);
    table->PutValue("Hi", array);
    array.set(0, i);
    table->PutValue("Hi3", array);
    for(int j = 0; j<4; ++j){
	    table->PutValue("Hi", array);
	    table->PutNumber("i", i);
	    table->PutValue("Hi2", array);
	    table->PutNumber("j", j);
	    sleep(1);
	    printf("Testing %d %d \n", i, j);
    }
  }

  printf("Waiting for keypress\n");

  std::string tmp;
  std::cin >> tmp;
  printf("Shutting down\n");
  cTable->RemoveTableListener(listener);
  NetworkTable::Shutdown();
  delete listener;

  /*ITable* table = NetworkTable::GetTable("SmartDashboard");
  ITable* cTable = NetworkTable::GetTable("client");
  class ServerListener : public ITableListener {
      virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew){
	fprintf(stdout, "Got key in client table: %s, = %f\n", key.c_str(), value.f);
	fflush(stdout);
	NetworkTable::GetTable("server")->PutNumber(key, value.f);
      };
  };
  ServerListener* listener = new ServerListener();
  cTable->AddTableListener("mykey", listener, true);
  table->PutString("TEST", "hello");
  sleep(1);*/
  pthread_exit(NULL);
}
