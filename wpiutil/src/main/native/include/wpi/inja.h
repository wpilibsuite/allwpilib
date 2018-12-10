/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_INJA_H_
#define WPIUTIL_WPI_INJA_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "wpi/ArrayRef.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"
#include "wpi/json.h"

namespace wpi {

class raw_ostream;

namespace inja {

class Bytecode;
class Parser;
class Renderer;

using CallbackFunction =
    std::function<json(ArrayRef<const json*> args, const json& data)>;

class Template {
  friend class Parser;
  friend class Renderer;

 public:
  // These must be out of line because Bytecode is forward declared
  Template();
  ~Template();
  Template(const Template&);
  Template(Template&& oth);
  Template& operator=(const Template&);
  Template& operator=(Template&&);

 private:
  std::vector<Bytecode> bytecodes;
  std::string contents;
};

enum class ElementNotation {
  Dot,
  Pointer
};

class Environment {
 public:
  Environment();
  explicit Environment(const Twine& path);
  ~Environment();

  void SetStatement(const Twine& open, const Twine& close);

  void SetLineStatement(const Twine& open);

  void SetExpression(const Twine& open, const Twine& close);

  void SetComment(const Twine& open, const Twine& close);

  void SetElementNotation(ElementNotation notation);

  void SetLoadFile(std::function<std::string(StringRef)> loadFile);

  Template Parse(StringRef input);

  std::string Render(StringRef input, const json& data);

  std::string Render(const Template& tmpl, const json& data);

  raw_ostream& RenderTo(raw_ostream& os, const Template& tmpl,
                        const json& data);

  void AddCallback(StringRef name, unsigned int numArgs,
                   const CallbackFunction& callback);

  void IncludeTemplate(const Twine& name, const Template& tmpl);

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

/*!
@brief render with default settings
*/
inline std::string Render(StringRef input, const json& data) {
  return Environment().Render(input, data);
}

}  // namespace inja
}  // namespace wpi

#endif  // WPIUTIL_WPI_INJA_H_
