#pragma once

#include <memory>
#include <string>

#include <pybind11/pybind11.h>

namespace wpi::tunables::python {

class PyTunable;

class PyMutationList {
 public:
  PyMutationList(std::shared_ptr<PyTunable> owner, pybind11::list data);
  size_t Size() const;
  pybind11::iterator Iter() const;
  pybind11::object GetItem(pybind11::object key) const;
  void SetItem(pybind11::object key, pybind11::object value);
  void DelItem(pybind11::object key);
  bool Contains(pybind11::object value) const;
  bool Equal(pybind11::object value) const;
  std::string Repr() const;
  pybind11::list Copy() const;
  void Append(pybind11::object value);
  void Extend(pybind11::object value);
  void Insert(pybind11::ssize_t index, pybind11::object value);
  pybind11::object Pop(pybind11::args args);
  void Remove(pybind11::object value);
  void Clear();
  void Reverse();
  void Sort(pybind11::args args, pybind11::kwargs kwargs);
  PyMutationList& IAdd(pybind11::object value);

 private:
  void Sync();
  std::shared_ptr<PyTunable> m_owner;
  pybind11::list m_data;
};

pybind11::object MakeMutationList(std::shared_ptr<PyTunable> owner,
                                  pybind11::list data);

}  // namespace wpi::tunables::python
