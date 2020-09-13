//
//  Portable File Dialogs
//
//  Copyright © 2018—2020 Sam Hocevar <sam@hocevar.net>
//
//  This library is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the **** You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

#include <string>   // std::string
#include <memory>   // std::shared_ptr
#include <map>      // std::map
#include <vector>

namespace pfd
{

enum class button
{
    cancel = -1,
    ok,
    yes,
    no,
    abort,
    retry,
    ignore,
};

enum class choice
{
    ok = 0,
    ok_cancel,
    yes_no,
    yes_no_cancel,
    retry_cancel,
    abort_retry_ignore,
};

enum class icon
{
    info = 0,
    warning,
    error,
    question,
};

// Additional option flags for various dialog constructors
enum class opt : uint8_t
{
    none = 0,
    // For file open, allow multiselect.
    multiselect     = 0x1,
    // For file save, force overwrite and disable the confirmation dialog.
    force_overwrite = 0x2,
    // For folder select, force path to be the provided argument instead
    // of the last opened directory, which is the Microsoft-recommended,
    // user-friendly behaviour.
    force_path      = 0x4,
};

inline opt operator |(opt a, opt b) { return opt(uint8_t(a) | uint8_t(b)); }
inline bool operator &(opt a, opt b) { return bool(uint8_t(a) & uint8_t(b)); }

// The settings class, only exposing to the user a way to set verbose mode
// and to force a rescan of installed desktop helpers (zenity, kdialog…).
class settings
{
public:
    static bool available();

    static void verbose(bool value);
    static void rescan();

protected:
    explicit settings(bool resync = false);

    bool check_program(std::string const &program);

    constexpr bool is_osascript() const {
#if __APPLE__
        return true;
#else
        return false;
#endif
    }
    bool is_zenity() const {
        return flags(flag::has_zenity) ||
               flags(flag::has_matedialog) ||
               flags(flag::has_qarma);
    }
    bool is_kdialog() const { return flags(flag::has_kdialog); }

    enum class flag
    {
        is_scanned = 0,
        is_verbose,

        has_zenity,
        has_matedialog,
        has_qarma,
        has_kdialog,
        is_vista,

        max_flag,
    };

    // Static array of flags for internal state
    bool const &flags(flag in_flag) const;

    // Non-const getter for the static array of flags
    bool &flags(flag in_flag);
};

// Internal classes, not to be used by client applications
namespace internal
{

// Process wait timeout, in milliseconds
constexpr int default_wait_timeout = 20;

class executor;

class dialog : protected settings
{
public:
    bool ready(int timeout = default_wait_timeout) const;
    bool kill() const;

protected:
    explicit dialog();

    std::vector<std::string> desktop_helper() const;
    static std::string buttons_to_name(choice _choice);
    static std::string get_icon_name(icon _icon);

    std::string powershell_quote(std::string const &str) const;
    std::string osascript_quote(std::string const &str) const;
    std::string shell_quote(std::string const &str) const;

    // Keep handle to executing command
    std::shared_ptr<executor> m_async;
};

class file_dialog : public dialog
{
protected:
    enum type
    {
        open,
        save,
        folder,
    };

    file_dialog(type in_type,
                std::string const &title,
                std::string const &default_path = "",
                std::vector<std::string> const &filters = {},
                opt options = opt::none);

protected:
    std::string string_result();
    std::vector<std::string> vector_result();

    class Impl;
    std::shared_ptr<Impl> m_impl;
};

} // namespace internal

//
// The notify widget
//

class notify : public internal::dialog
{
public:
    notify(std::string const &title,
           std::string const &message,
           icon _icon = icon::info);
};

//
// The message widget
//

class message : public internal::dialog
{
public:
    message(std::string const &title,
            std::string const &text,
            choice _choice = choice::ok_cancel,
            icon _icon = icon::info);

    button result();

private:
    // Some extra logic to map the exit code to button number
    std::map<int, button> m_mappings;
};

//
// The open_file, save_file, and open_folder widgets
//

class open_file : public internal::file_dialog
{
public:
    open_file(std::string const &title,
              std::string const &default_path = "",
              std::vector<std::string> const &filters = { "All Files", "*" },
              opt options = opt::none)
      : file_dialog(type::open, title, default_path, filters, options)
    {}

    std::vector<std::string> result() { return vector_result(); }
};

class save_file : public internal::file_dialog
{
public:
    save_file(std::string const &title,
              std::string const &default_path = "",
              std::vector<std::string> const &filters = { "All Files", "*" },
              opt options = opt::none)
      : file_dialog(type::save, title, default_path, filters, options)
    {}

    std::string result() { return string_result(); }
};

class select_folder : public internal::file_dialog
{
public:
    select_folder(std::string const &title,
                  std::string const &default_path = "",
                  opt options = opt::none)
      : file_dialog(type::folder, title, default_path, {}, options)
    {}

    std::string result() { return string_result(); }
};

} // namespace pfd

