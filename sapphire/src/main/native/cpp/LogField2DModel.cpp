#include "LogField2DModel.h"


#include <wpi/StringExtras.h>


using namespace sapphire;

void LogField2DModel::ObjectModel::UpdatePoses(const wpi::log::DataLogRecord& value){
    std::vector<double> arr;
    if(!value.GetDoubleArray(&arr)){
        return;
    }
    auto size = arr.size();
    if ((size % 3) != 0) {
        return;
    }
    m_poses.resize(size / 3);
    for (size_t i = 0; i < size / 3; ++i) {
        m_poses[i] = frc::Pose2d{
            units::meter_t{arr[i * 3 + 0]}, units::meter_t{arr[i * 3 + 1]},
            frc::Rotation2d{units::degree_t{arr[i * 3 + 2]}}};
    }
}
void LogField2DModel::ObjectModel::SetPoses(std::span<const frc::Pose2d> poses) {
  m_poses.assign(poses.begin(), poses.end());
}

void LogField2DModel::ObjectModel::SetPose(size_t i, frc::Pose2d pose) {
  if (i < m_poses.size()) {
    m_poses[i] = pose;
  }
}

void LogField2DModel::ObjectModel::SetPosition(size_t i,
                                              frc::Translation2d pos) {
  if (i < m_poses.size()) {
    m_poses[i] = frc::Pose2d{pos, m_poses[i].Rotation()};
  }
}

void LogField2DModel::ObjectModel::SetRotation(size_t i, frc::Rotation2d rot) {
  if (i < m_poses.size()) {
    m_poses[i] = frc::Pose2d{m_poses[i].Translation(), rot};
  }
}


LogField2DModel::LogField2DModel(DataLogModel& model, std::string_view path, float& nowRef) :  m_model{model},
                                                    m_path{path},
                                                    m_node{model.GetEntryNode(std::string(path))},
                                                    m_nowRef{nowRef},
                                                    m_now{nowRef} {
  for(auto& obj : m_node->m_children){
    if(obj.m_entry && obj.m_entry->m_type == "double[]"){
      AddFieldObject(obj.m_name);
    }
  }
}

LogField2DModel::~LogField2DModel() = default;

void LogField2DModel::Update() {
    if(m_now != m_nowRef + m_model.m_offset){ 
        m_now = m_nowRef + m_model.m_offset;
        for(auto& object : m_objects){
            object->Update();
        }
    }
}



glass::FieldObjectModel* LogField2DModel::AddFieldObject(std::string_view name) {
  auto fullName = fmt::format("{}{}", m_path, name);
  auto [it, match] = Find(fullName);
  if (!match) {
    auto dataNode = m_model.GetEntryNode(fullName);
    if(dataNode == nullptr){ return nullptr;}
    it = m_objects.emplace(
        it, std::make_unique<ObjectModel>(fullName, dataNode->m_entry, m_nowRef));
  }
  return it->get();

}

void  LogField2DModel::RemoveFieldObject(std::string_view name) {
  auto [it, match] = Find(fmt::format("{}{}", m_path, name));
  if (match) {
    m_objects.erase(it);
  }
}

void LogField2DModel::ForEachFieldObject(
    wpi::function_ref<void(glass::FieldObjectModel& model, std::string_view name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      func(*obj, wpi::drop_front(obj->GetName(), m_path.size()));
    }
  }
}


std::pair<LogField2DModel::Objects::iterator, bool> LogField2DModel::Find(
    std::string_view fullName) {
  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), fullName,
      [](const auto& e, std::string_view name) { return e->GetName() < name; });
  return {it, it != m_objects.end() && (*it)->GetName() == fullName};
}