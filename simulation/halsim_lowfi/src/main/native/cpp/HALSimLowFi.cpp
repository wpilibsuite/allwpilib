/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <HALSimLowFi.h>

void HALSimLowFi::Initialize() {
    table = nt::NetworkTableInstance::GetDefault().GetTable("sim");
}

void HALSimNTProvider::Inject(std::shared_ptr<HALSimLowFi> parentArg, std::string tableName) {
    parent = parentArg;
    table = parent->table->GetSubTable(tableName);
    this->Initialize();
}