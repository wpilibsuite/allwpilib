#ifndef _NETWORKTABLE_H_
#define _NETWORKTABLE_H_

#include <vector>

#include "tables/ITable.h"

class NetworkTable : public ITable {
 private:
  struct private_init {};

  std::string m_path;
  typedef std::pair<ITableListener*, unsigned int> Listener;
  std::vector<Listener> m_listeners;

  static std::string s_ip_address;
  static bool s_client;
  static bool s_running;

 public:
  NetworkTable(llvm::StringRef path, const private_init&);
  virtual ~NetworkTable();

  /**
   * The path separator for sub-tables and keys
   *
   */
  static const char PATH_SEPARATOR_CHAR;

  /**
   * @throws IOException
   */
  static void Initialize();
  static void Shutdown();

  /**
   * set that network tables should be a client
   * This must be called before initalize or GetTable
   */
  static void SetClientMode();

  /**
   * set that network tables should be a server
   * This must be called before initalize or GetTable
   */
  static void SetServerMode();

  /**
   * set the team the robot is configured for (this will set the ip address that
   * network tables will connect to in client mode)
   * This must be called before initalize or GetTable
   * @param team the team number
   */
  static void SetTeam(int team);
  /**
   * @param address the adress that network tables will connect to in client
   * mode
   */
  static void SetIPAddress(llvm::StringRef address);

  /**
   * Gets the table with the specified key. If the table does not exist, a new
   *table will be created.<br>
   * This will automatically initialize network tables if it has not been
   *already
   *
   * @param key
   *            the key name
   * @return the network table requested
   */
  static std::shared_ptr<NetworkTable> GetTable(llvm::StringRef key);

  void AddTableListener(ITableListener* listener);
  void AddTableListener(ITableListener* listener, bool immediateNotify);
  void AddTableListener(llvm::StringRef key, ITableListener* listener,
                        bool immediateNotify);
  void RemoveTableListener(ITableListener* listener);

  /**
   * Returns the table at the specified key. If there is no table at the
   * specified key, it will create a new table
   *
   * @param key
   *            the key name
   * @return the networktable to be returned
   */
  std::shared_ptr<ITable> GetSubTable(llvm::StringRef key) const;

  /**
   * Checks the table and tells if it contains the specified key
   *
   * @param key
   *            the key to be checked
   */
  bool ContainsKey(llvm::StringRef key) const;

  bool ContainsSubTable(llvm::StringRef key) const;

  /**
   * Makes a key's value persistent through program restarts.
   *
   * @param key the key to make persistent
   */
  void Persist(llvm::StringRef key);

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *
   * @param key
   *            the key
   * @param value
   *            the value
   */
  void PutNumber(llvm::StringRef key, double value);

  /**
   * Returns the key that the name maps to. If the key is null, it will return
   * the default value
   *
   * @param key
   *            the key name
   * @param defaultValue
   *            the default value if the key is null
   * @return the key
   */
  double GetNumber(llvm::StringRef key, double defaultValue) const;

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *
   * @param key
   *            the key
   * @param value
   *            the value
   */
  void PutString(llvm::StringRef key, llvm::StringRef value);

  /**
   * Returns the key that the name maps to. If the key is null, it will return
   * the default value
   *
   * @param key
   *            the key name
   * @param defaultValue
   *            the default value if the key is null
   * @return the key
   */
  std::string GetString(llvm::StringRef key,
                        llvm::StringRef defaultValue) const;

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *
   * @param key
   *            the key
   * @param value
   *            the value
   */
  void PutBoolean(llvm::StringRef key, bool value);

  /**
   * Returns the key that the name maps to. If the key is null, it will return
   * the default value
   *
   * @param key
   *            the key name
   * @param defaultValue
   *            the default value if the key is null
   * @return the key
   */
  bool GetBoolean(llvm::StringRef key, bool defaultValue) const;

  /**
   * Maps the specified key to the specified value in this table. The key can
   * not be null. The value can be retrieved by calling the get method with a
   * key that is equal to the original key.
   *
   * @param key the key name
   * @param value the value to be put
   */
  void PutValue(llvm::StringRef key, std::shared_ptr<nt::Value> value);

  /**
   * Returns the key that the name maps to.
   * NOTE: If the value is a double, it will return a Double object,
   * not a primitive.  To get the primitive, use GetDouble
   *
   * @param key
   *            the key name
   * @return the key, or nullptr if the key is not defined
   */
  std::shared_ptr<nt::Value> GetValue(llvm::StringRef key) const;
};

#endif
