Network Tables 2.0 README

Important Classes:

Network Table Transaction - Handles reading/writing bytes to a connection
Client Connection Adapter - Handles behavior specific to a client and interacts with the entry store
Server Connection Adapter - Handles behavior specific to a connection to a server and interacts with the server's entry store
Null Transaction Receiver - Returns a transaction and all entries inside of it to the transaction pool
Write Manager - Buffers transaction and writes them out periodically on another thread
Transaction Dirtier - Marks all entries mentioned in a transaction as dirty
Entry Store - The local copy of entries, worries about applying transactions locally
Connection List - Keeps a list of connections and forwards a transaction to all of them