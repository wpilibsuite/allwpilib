#include "networktables2/client/NetworkTableClient.h"
#include "networktables2/type/NetworkTableEntryTypeManager.h"
#include "networktables2/stream/SocketStreamFactory.h"
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
#include "networktables2/type/StringArray.h"


NetworkTableClient* client;

int main(){
  NetworkTable::SetClientMode();
  NetworkTable::SetIPAddress("localhost");
  NetworkTable::Initialize();

  ITable* table = NetworkTable::GetTable("SmartDashboard");
  ITable* sTable = NetworkTable::GetTable("/server");
  class ClientListener : public ITableListener {
      virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew){
	fprintf(stdout, "Got key in server table: %s, = %f\n", key.c_str(), value.f);
	fflush(stdout);
	NetworkTable::GetTable("/client")->PutNumber(key, value.f);
      };
  };
  ClientListener* listener = new ClientListener();
  sTable->AddTableListener(listener, true);


  sleep(2);

  StringArray array;
  array.add("Hello");
  array.add("Another Hello");

  table->PutValue("myarrayjjj", array);

  table->GetNumber("mykey", 0);
  

  table->PutString("TEST", "hello");
  table->PutString("TEST", "hellofasd");
  table->PutString("TEST", "hellod");
  table->PutString("TEST", "helloddd");
  table->PutString("TEST", "hellodsasd");
  table->PutString("TEST", "helloffff");



  std::string tmp;
  std::cin >> tmp;
  sTable->RemoveTableListener(listener);
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
