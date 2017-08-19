/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockData/NotifyListenerVector.h"

using namespace hal;

NotifyListenerVector::NotifyListenerVector(const NotifyListenerVector* copyFrom,
                                           const private_init&)
    : m_vector(copyFrom->m_vector), m_free(copyFrom->m_free) {}

NotifyListenerVector::NotifyListenerVector(void* param,
                                           HAL_NotifyCallback callback,
                                           unsigned int* newUid) {
  *newUid = emplace_back_impl(param, callback);
}

std::shared_ptr<NotifyListenerVector> NotifyListenerVector::emplace_back(
    void* param, HAL_NotifyCallback callback, unsigned int* newUid) {
  auto newVector = std::make_shared<NotifyListenerVector>(this, private_init());
  newVector->m_vector = m_vector;
  newVector->m_free = m_free;
  *newUid = newVector->emplace_back_impl(param, callback);
  return newVector;
}

std::shared_ptr<NotifyListenerVector> NotifyListenerVector::erase(
    unsigned int uid) {
  auto newVector = std::make_shared<NotifyListenerVector>(this, private_init());
  newVector->m_vector = m_vector;
  newVector->m_free = m_free;
  newVector->erase_impl(uid);
  return newVector;
}

unsigned int NotifyListenerVector::emplace_back_impl(
    void* param, HAL_NotifyCallback callback) {
  unsigned int uid;
  if (m_free.empty()) {
    uid = m_vector.size();
    m_vector.emplace_back(param, callback);
  } else {
    uid = m_free.back();
    m_free.pop_back();
    m_vector[uid] = NotifyListener(param, callback);
  }
  return uid + 1;
}

void NotifyListenerVector::erase_impl(unsigned int uid) {
  --uid;
  if (uid >= m_vector.size() || !m_vector[uid]) return;
  m_free.push_back(uid);
  m_vector[uid] = NotifyListener();
}
