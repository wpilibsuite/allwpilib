#include "SystemServer.h"
#include "mrc/NtNetComm.h"

namespace hal {

static nt::NetworkTableInstance ServerInstance;

void InitializeSystemServer() {
    if (ServerInstance) {
        return;
    }
    ServerInstance = nt::NetworkTableInstance::Create();
    ServerInstance.SetServer("localhost", ROBOT_SYSTEM_SERVER_NT_PORT);
    ServerInstance.StartClient4("RobotProgram");
}

void ShutdownSystemServer() {
    if (!ServerInstance) {
        return;
    }

    ServerInstance.StopClient();
    nt::NetworkTableInstance::Destroy(ServerInstance);
}

nt::NetworkTableInstance GetSystemServer() {
    return ServerInstance;
}

}
