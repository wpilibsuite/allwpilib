/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) 2018 FIRST. All Rights Reserved.               */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
Inja - A Template Engine for Modern C++
version 1.1.0
https://github.com/pantor/inja

Licensed under the MIT License <https://opensource.org/licenses/MIT>.
Copyright (c) 2017-2018 Pantor <https://github.com/pantor>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "TestThrow.h"
#include "gtest/gtest.h"
#include "inja_renderer.h"
#include "wpi/SmallString.h"
#include "wpi/inja.h"

using namespace wpi;

TEST(InjaDotToPointer, Case) {
  SmallString<64> buf;
  EXPECT_EQ(inja::ConvertDotToJsonPointer("person.names.surname", buf),
            "/person/names/surname");
  EXPECT_EQ(inja::ConvertDotToJsonPointer("guests.2", buf), "/guests/2");
}

class InjaTypesTest : public ::testing::Test {
 public:
  inja::Environment env;
  json data;

  InjaTypesTest() {
    data["name"] = "Peter";
    data["city"] = "Brunswick";
    data["age"] = 29;
    data["names"] = {"Jeff", "Seb"};
    data["brother"]["name"] = "Chris";
    data["brother"]["daughters"] = {"Maria", "Helen"};
    data["brother"]["daughter0"] = { { "name", "Maria" } };
    data["is_happy"] = true;
    data["is_sad"] = false;
    data["relatives"]["mother"] = "Maria";
    data["relatives"]["brother"] = "Chris";
    data["relatives"]["sister"] = "Jenny";
    data["vars"] = {2, 3, 4, 0, -1, -2, -3};
  }
};

TEST_F(InjaTypesTest, Basic) {
  EXPECT_EQ(env.Render("", data), "");
  EXPECT_EQ(env.Render("Hello World!", data), "Hello World!");
}

TEST_F(InjaTypesTest, Variables) {
  EXPECT_EQ(env.Render("Hello {{ name }}!", data), "Hello Peter!");
  EXPECT_EQ(env.Render("{{ name }}", data), "Peter");
  EXPECT_EQ(env.Render("{{name}}", data), "Peter");
  EXPECT_EQ(env.Render("{{ name }} is {{ age }} years old.", data),
            "Peter is 29 years old.");
  EXPECT_EQ(env.Render("{{name}}{{age}}", data), "Peter29");
  EXPECT_EQ(env.Render("Hello {{ name }}! I come from {{ city }}.", data),
            "Hello Peter! I come from Brunswick.");
  EXPECT_EQ(env.Render("Hello {{ names/1 }}!", data), "Hello Seb!");
  EXPECT_EQ(env.Render("Hello {{ brother/name }}!", data), "Hello Chris!");
  EXPECT_EQ(env.Render("Hello {{ brother/daughter0/name }}!", data),
            "Hello Maria!");

  EXPECT_THROW_MSG(
      env.Render("{{unknown}}", data), std::runtime_error,
      "[inja.exception.render_error] variable 'unknown' not found");
}

TEST_F(InjaTypesTest, Comments) {
  EXPECT_EQ(env.Render("Hello{# This is a comment #}!", data), "Hello!");
  EXPECT_EQ(env.Render("{# --- #Todo --- #}", data), "");
}

TEST_F(InjaTypesTest, Loops) {
  EXPECT_EQ(env.Render("{% for name in names %}a{% endfor %}", data), "aa");
  EXPECT_EQ(
      env.Render("Hello {% for name in names %}{{ name }} {% endfor %}!", data),
      "Hello Jeff Seb !");
  EXPECT_EQ(env.Render("Hello {% for name in names %}{{ loop/index }}: {{ name "
                       "}}, {% endfor %}!",
                       data),
            "Hello 0: Jeff, 1: Seb, !");
  EXPECT_EQ(env.Render("{% for type, name in relatives %}{{ type }}: {{ name "
                       "}}, {% endfor %}",
                       data),
            "brother: Chris, mother: Maria, sister: Jenny, ");
  EXPECT_EQ(
      env.Render("{% for v in vars %}{% if v > 0 %}+{% endif %}{% endfor %}",
                 data),
      "+++");
  EXPECT_EQ(env.Render("{% for name in names %}{{ loop/index }}: {{ name }}{% "
                       "if not loop/is_last %}, {% endif %}{% endfor %}!",
                       data),
            "0: Jeff, 1: Seb!");
  EXPECT_EQ(env.Render("{% for name in names %}{{ loop/index }}: {{ name }}{% "
                       "if loop/is_last == false %}, {% endif %}{% endfor %}!",
                       data),
            "0: Jeff, 1: Seb!");

  data["empty_loop"] = {};
  EXPECT_EQ(env.Render("{% for name in empty_loop %}a{% endfor %}", data), "");
  EXPECT_EQ(env.Render("{% for name in {} %}a{% endfor %}", data), "");

  EXPECT_THROW_MSG(env.Render("{% for name ins names %}a{% endfor %}", data),
                   std::runtime_error,
                   "[inja.exception.parser_error] expected 'in', got 'ins'");
  // CHECK_THROWS_WITH( env.Render("{% for name in relatives %}{{ name }}{%
  // endfor %}", data), "[inja.exception.json_error]
  // [json.exception.type_error.302] type must be array, but is object" );
}

TEST_F(InjaTypesTest, Conditionals) {
  EXPECT_EQ(env.Render("{% if is_happy %}Yeah!{% endif %}", data), "Yeah!");
  EXPECT_EQ(env.Render("{% if is_sad %}Yeah!{% endif %}", data), "");
  EXPECT_EQ(
      env.Render("{% if is_sad %}Yeah!{% else %}Nooo...{% endif %}", data),
      "Nooo...");
  EXPECT_EQ(
      env.Render("{% if age == 29 %}Right{% else %}Wrong{% endif %}", data),
      "Right");
  EXPECT_EQ(
      env.Render("{% if age > 29 %}Right{% else %}Wrong{% endif %}", data),
      "Wrong");
  EXPECT_EQ(
      env.Render("{% if age <= 29 %}Right{% else %}Wrong{% endif %}", data),
      "Right");
  EXPECT_EQ(
      env.Render("{% if age != 28 %}Right{% else %}Wrong{% endif %}", data),
      "Right");
  EXPECT_EQ(
      env.Render("{% if age >= 30 %}Right{% else %}Wrong{% endif %}", data),
      "Wrong");
  EXPECT_EQ(env.Render("{% if age in [28, 29, 30] %}True{% endif %}", data),
            "True");
  EXPECT_EQ(
      env.Render("{% if age == 28 %}28{% else if age == 29 %}29{% endif %}",
                 data),
      "29");
  EXPECT_EQ(env.Render("{% if age == 26 %}26{% else if age == 27 %}27{% else "
                       "if age == 28 %}28{% else %}29{% endif %}",
                       data),
            "29");
  EXPECT_EQ(env.Render("{% if age == 25 %}+{% endif %}{% if age == 29 %}+{% "
                       "else %}-{% endif %}",
                       data),
            "+");

  EXPECT_THROW_MSG(
      env.Render("{% if is_happy %}{% if is_happy %}{% endif %}", data),
      std::runtime_error, "[inja.exception.parser_error] unmatched if");
  //EXPECT_THROW_MSG(
  //    env.Render("{% if is_happy %}{% else if is_happy %}{% end if %}", data),
  //    std::runtime_error,
  //    "[inja.exception.parser_error] misordered if statement");
}

TEST_F(InjaTypesTest, LineStatements) {
  EXPECT_EQ(env.Render("## if is_happy\nYeah!\n## endif", data), "Yeah!\n");

  EXPECT_EQ(
      env.Render("## if is_happy\n## if is_happy\nYeah!\n## endif\n## endif",
                 data),
      "Yeah!\n");
}

class InjaFunctionsTest : public ::testing::Test {
 public:
  inja::Environment env;
  json data;

  InjaFunctionsTest() {
    data["name"] = "Peter";
    data["city"] = "New York";
    data["names"] = {"Jeff", "Seb", "Peter", "Tom"};
    data["temperature"] = 25.6789;
    data["brother"]["name"] = "Chris";
    data["brother"]["daughters"] = {"Maria", "Helen"};
    data["property"] = "name";
    data["age"] = 29;
    data["is_happy"] = true;
    data["is_sad"] = false;
    data["vars"] = {2, 3, 4, 0, -1, -2, -3};
  }
};

TEST_F(InjaFunctionsTest, Upper) {
  EXPECT_EQ(env.Render("{{ upper(name) }}", data), "PETER");
  EXPECT_EQ(env.Render("{{ upper(  name  ) }}", data), "PETER");
  EXPECT_EQ(env.Render("{{ upper(city) }}", data), "NEW YORK");
  EXPECT_EQ(env.Render("{{ upper(upper(name)) }}", data), "PETER");
  // CHECK_THROWS_WITH( env.Render("{{ upper(5) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // string, but is number" ); CHECK_THROWS_WITH( env.Render("{{ upper(true)
  // }}", data), "[inja.exception.json_error] [json.exception.type_error.302]
  // type must be string, but is boolean" );
}

TEST_F(InjaFunctionsTest, Lower) {
  EXPECT_EQ(env.Render("{{ lower(name) }}", data), "peter");
  EXPECT_EQ(env.Render("{{ lower(city) }}", data), "new york");
  // CHECK_THROWS_WITH( env.Render("{{ lower(5.45) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // string, but is number" );
}

TEST_F(InjaFunctionsTest, Range) {
  EXPECT_EQ(env.Render("{{ range(2) }}", data), "[0,1]");
  EXPECT_EQ(env.Render("{{ range(4) }}", data), "[0,1,2,3]");
  // CHECK_THROWS_WITH( env.Render("{{ range(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // number, but is string" );
}

TEST_F(InjaFunctionsTest, Length) {
  EXPECT_EQ(env.Render("{{ length(names) }}", data), "4");
  // CHECK_THROWS_WITH( env.Render("{{ length(5) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is number" );
}

TEST_F(InjaFunctionsTest, Sort) {
  EXPECT_EQ(env.Render("{{ sort([3, 2, 1]) }}", data), "[1,2,3]");
  EXPECT_EQ(env.Render("{{ sort([\"bob\", \"charlie\", \"alice\"]) }}", data),
            "[\"alice\",\"bob\",\"charlie\"]");
  // CHECK_THROWS_WITH( env.Render("{{ sort(5) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is number" );
}

TEST_F(InjaFunctionsTest, First) {
  EXPECT_EQ(env.Render("{{ first(names) }}", data), "Jeff");
  // CHECK_THROWS_WITH( env.Render("{{ first(5) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is number" );
}

TEST_F(InjaFunctionsTest, Last) {
  EXPECT_EQ(env.Render("{{ last(names) }}", data), "Tom");
  // CHECK_THROWS_WITH( env.Render("{{ last(5) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is number" );
}

TEST_F(InjaFunctionsTest, Round) {
  EXPECT_EQ(env.Render("{{ round(4, 0) }}", data), "4.0");
  EXPECT_EQ(env.Render("{{ round(temperature, 2) }}", data), "25.68");
  // CHECK_THROWS_WITH( env.Render("{{ round(name, 2) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // number, but is string" );
}

TEST_F(InjaFunctionsTest, DivisibleBy) {
  EXPECT_EQ(env.Render("{{ divisibleBy(50, 5) }}", data), "true");
  EXPECT_EQ(env.Render("{{ divisibleBy(12, 3) }}", data), "true");
  EXPECT_EQ(env.Render("{{ divisibleBy(11, 3) }}", data), "false");
  // CHECK_THROWS_WITH( env.Render("{{ divisibleBy(name, 2) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // number, but is string" );
}

TEST_F(InjaFunctionsTest, Odd) {
  EXPECT_EQ(env.Render("{{ odd(11) }}", data), "true");
  EXPECT_EQ(env.Render("{{ odd(12) }}", data), "false");
  // CHECK_THROWS_WITH( env.Render("{{ odd(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // number, but is string" );
}

TEST_F(InjaFunctionsTest, Even) {
  EXPECT_EQ(env.Render("{{ even(11) }}", data), "false");
  EXPECT_EQ(env.Render("{{ even(12) }}", data), "true");
  // CHECK_THROWS_WITH( env.Render("{{ even(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // number, but is string" );
}

TEST_F(InjaFunctionsTest, Max) {
  EXPECT_EQ(env.Render("{{ max([1, 2, 3]) }}", data), "3");
  EXPECT_EQ(env.Render("{{ max([-5.2, 100.2, 2.4]) }}", data), "100.2");
  // CHECK_THROWS_WITH( env.Render("{{ max(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is string" );
}

TEST_F(InjaFunctionsTest, Min) {
  EXPECT_EQ(env.Render("{{ min([1, 2, 3]) }}", data), "1");
  EXPECT_EQ(env.Render("{{ min([-5.2, 100.2, 2.4]) }}", data), "-5.2");
  // CHECK_THROWS_WITH( env.Render("{{ min(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is string" );
}

TEST_F(InjaFunctionsTest, Float) {
  EXPECT_EQ(env.Render("{{ float(\"2.2\") == 2.2 }}", data), "true");
  EXPECT_EQ(env.Render("{{ float(\"-1.25\") == -1.25 }}", data), "true");
  // CHECK_THROWS_WITH( env.Render("{{ max(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is string" );
}

TEST_F(InjaFunctionsTest, Int) {
  EXPECT_EQ(env.Render("{{ int(\"2\") == 2 }}", data), "true");
  EXPECT_EQ(env.Render("{{ int(\"-1.25\") == -1 }}", data), "true");
  // CHECK_THROWS_WITH( env.Render("{{ max(name) }}", data),
  // "[inja.exception.json_error] [json.exception.type_error.302] type must be
  // array, but is string" );
}

TEST_F(InjaFunctionsTest, Default) {
  EXPECT_EQ(env.Render("{{ default(11, 0) }}", data), "11");
  EXPECT_EQ(env.Render("{{ default(nothing, 0) }}", data), "0");
  EXPECT_EQ(env.Render("{{ default(name, \"nobody\") }}", data), "Peter");
  EXPECT_EQ(env.Render("{{ default(surname, \"nobody\") }}", data), "nobody");
  EXPECT_THROW_MSG(
      env.Render("{{ default(surname, lastname) }}", data), std::runtime_error,
      "[inja.exception.render_error] variable 'lastname' not found");
}

TEST_F(InjaFunctionsTest, Exists) {
  EXPECT_EQ(env.Render("{{ exists(\"name\") }}", data), "true");
  EXPECT_EQ(env.Render("{{ exists(\"zipcode\") }}", data), "false");
  EXPECT_EQ(env.Render("{{ exists(name) }}", data), "false");
  EXPECT_EQ(env.Render("{{ exists(property) }}", data), "true");
}

TEST_F(InjaFunctionsTest, ExistsIn) {
  EXPECT_EQ(env.Render("{{ existsIn(brother, \"name\") }}", data), "true");
  EXPECT_EQ(env.Render("{{ existsIn(brother, \"parents\") }}", data), "false");
  EXPECT_EQ(env.Render("{{ existsIn(brother, property) }}", data), "true");
  EXPECT_EQ(env.Render("{{ existsIn(brother, name) }}", data), "false");
  EXPECT_THROW_MSG(
      env.Render("{{ existsIn(sister, \"lastname\") }}", data),
      std::runtime_error,
      "[inja.exception.render_error] variable 'sister' not found");
  EXPECT_THROW_MSG(
      env.Render("{{ existsIn(brother, sister) }}", data), std::runtime_error,
      "[inja.exception.render_error] variable 'sister' not found");
}

TEST_F(InjaFunctionsTest, IsType) {
  EXPECT_EQ(env.Render("{{ isBoolean(is_happy) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isBoolean(vars) }}", data), "false");
  EXPECT_EQ(env.Render("{{ isNumber(age) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isNumber(name) }}", data), "false");
  EXPECT_EQ(env.Render("{{ isInteger(age) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isInteger(is_happy) }}", data), "false");
  EXPECT_EQ(env.Render("{{ isFloat(temperature) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isFloat(age) }}", data), "false");
  EXPECT_EQ(env.Render("{{ isObject(brother) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isObject(vars) }}", data), "false");
  EXPECT_EQ(env.Render("{{ isArray(vars) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isArray(name) }}", data), "false");
  EXPECT_EQ(env.Render("{{ isString(name) }}", data), "true");
  EXPECT_EQ(env.Render("{{ isString(names) }}", data), "false");
}

TEST(InjaCallbacksTest, Case) {
  inja::Environment env;
  json data;
  data["age"] = 28;

  env.AddCallback("double", 1,
                  [](ArrayRef<const json*> args, const json& data) {
                    int number = args[0]->get<double>();
                    return 2 * number;
                  });

  env.AddCallback("half", 1, [](ArrayRef<const json*> args, const json& data) {
    int number = args[0]->get<double>();
    return number / 2;
  });

  std::string greet = "Hello";
  env.AddCallback("double-greetings", 0, [greet](auto, const json&) {
    return greet + " " + greet + "!";
  });

  env.AddCallback("multiply", 2,
                  [](ArrayRef<const json*> args, const json& data) {
                    auto number1 = args[0]->get<double>();
                    auto number2 = args[1]->get<double>();
                    return number1 * number2;
                  });

  env.AddCallback("multiply", 3,
                  [](ArrayRef<const json*> args, const json& data) {
                    double number1 = args[0]->get<double>();
                    double number2 = args[1]->get<double>();
                    double number3 = args[2]->get<double>();
                    return number1 * number2 * number3;
                  });

  env.AddCallback("multiply", 0, [](auto, const json&) { return 1.0; });

  EXPECT_EQ( env.Render("{{ double(age) }}", data), "56" );
  EXPECT_EQ( env.Render("{{ half(age) }}", data), "14" );
  EXPECT_EQ( env.Render("{{ double-greetings }}", data), "Hello Hello!" );
  EXPECT_EQ( env.Render("{{ double-greetings() }}", data), "Hello Hello!" );
  EXPECT_EQ( env.Render("{{ multiply(4, 5) }}", data), "20.0" );
  EXPECT_EQ( env.Render("{{ multiply(3, 4, 5) }}", data), "60.0" );
  EXPECT_EQ( env.Render("{{ multiply }}", data), "1.0" );
}

TEST(InjaCombinationsTest, Case) {
  inja::Environment env;
  json data;
  data["name"] = "Peter";
  data["city"] = "Brunswick";
  data["age"] = 29;
  data["names"] = {"Jeff", "Seb"};
  data["brother"]["name"] = "Chris";
  data["brother"]["daughters"] = {"Maria", "Helen"};
  data["brother"]["daughter0"] = { { "name", "Maria" } };
  data["is_happy"] = true;

  EXPECT_EQ( env.Render("{% if upper(\"Peter\") == \"PETER\" %}TRUE{% endif %}", data), "TRUE" );
  EXPECT_EQ( env.Render("{% if lower(upper(name)) == \"peter\" %}TRUE{% endif %}", data), "TRUE" );
  EXPECT_EQ( env.Render("{% for i in range(4) %}{{ loop/index1 }}{% endfor %}", data), "1234" );
}

class InjaTemplatesTest : public ::testing::Test {
 public:
  inja::Environment env;
  json data;

  InjaTemplatesTest() {
    data["name"] = "Peter";
    data["city"] = "Brunswick";
    data["is_happy"] = true;
  }
};

TEST_F(InjaTemplatesTest, Reuse) {
  inja::Template temp =
      env.Parse("{% if is_happy %}{{ name }}{% else %}{{ city }}{% endif %}");

  EXPECT_EQ(env.Render(temp, data), "Peter");

  data["is_happy"] = false;

  EXPECT_EQ(env.Render(temp, data), "Brunswick");
}

TEST_F(InjaTemplatesTest, Include) {
  inja::Template t1 = env.Parse("Hello {{ name }}");
  env.IncludeTemplate("greeting", t1);

  inja::Template t2 = env.Parse("{% include \"greeting\" %}!");
  EXPECT_EQ(env.Render(t2, data), "Hello Peter!");
}

class InjaOtherSyntaxTest : public ::testing::Test {
 public:
  inja::Environment env;
  json data;

  InjaOtherSyntaxTest() {
    data["name"] = "Peter";
    data["city"] = "Brunswick";
    data["age"] = 29;
    data["names"] = {"Jeff", "Seb"};
    data["brother"]["name"] = "Chris";
    data["brother"]["daughters"] = {"Maria", "Helen"};
    data["brother"]["daughter0"] = { { "name", "Maria" } };
    data["is_happy"] = true;
  }
};

TEST_F(InjaOtherSyntaxTest, Variables) {
  env.SetElementNotation(inja::ElementNotation::Dot);

  EXPECT_EQ(env.Render("{{ name }}", data), "Peter");
  EXPECT_EQ(env.Render("Hello {{ names.1 }}!", data), "Hello Seb!");
  EXPECT_EQ(env.Render("Hello {{ brother.name }}!", data), "Hello Chris!");
  EXPECT_EQ(env.Render("Hello {{ brother.daughter0.name }}!", data),
            "Hello Maria!");

  EXPECT_THROW_MSG(
      env.Render("{{unknown.name}}", data), std::runtime_error,
      "[inja.exception.render_error] variable 'unknown.name' not found");
}

TEST_F(InjaOtherSyntaxTest, OtherExpressionSyntax) {
  EXPECT_EQ(env.Render("Hello {{ name }}!", data), "Hello Peter!");

  env.SetExpression("(&", "&)");

  EXPECT_EQ(env.Render("Hello {{ name }}!", data), "Hello {{ name }}!");
  EXPECT_EQ(env.Render("Hello (& name &)!", data), "Hello Peter!");
}

TEST_F(InjaOtherSyntaxTest, OtherCommentSyntax) {
  env.SetComment("(&", "&)");

  EXPECT_EQ(env.Render("Hello {# Test #}", data), "Hello {# Test #}");
  EXPECT_EQ(env.Render("Hello (& Test &)", data), "Hello ");
}
