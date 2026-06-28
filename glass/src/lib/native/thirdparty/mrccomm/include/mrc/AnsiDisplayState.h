#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

namespace mrc {

struct AnsiDisplayColor {
    enum class Kind {
        Default,
        Basic,
        Indexed256,
        Rgb,
    };

    Kind ColorKind = Kind::Default;
    int Index = 0;
    bool Bright = false;
    int Red = 0;
    int Green = 0;
    int Blue = 0;

    bool operator==(const AnsiDisplayColor& Other) const = default;
};

struct AnsiDisplayStyle {
    AnsiDisplayColor Foreground;
    AnsiDisplayColor Background;
    bool Bold = false;
    bool Italic = false;
    bool Underline = false;
    bool Inverse = false;

    bool operator==(const AnsiDisplayStyle& Other) const = default;
};

/**
 * Visitor for renderer-neutral ANSI display runs.
 */
class AnsiDisplayRunVisitor {
   public:
    /**
     * Destructor.
     */
    virtual ~AnsiDisplayRunVisitor() = default;

    /**
     * Starts a contiguous run of same-styled display cells.
     *
     * @param Row Row index
     * @param Column Starting column index
     * @param Columns Number of display columns
     * @param Style Run style
     */
    virtual void StartRun(size_t Row, size_t Column, size_t Columns,
                          const AnsiDisplayStyle& Style) = 0;

    /**
     * Appends one display cell's text to the current run.
     *
     * @param Text Cell text
     */
    virtual void AppendRunText(std::string_view Text) = 0;
};

class AnsiDisplayState {
   public:
    static constexpr std::string_view ClearScreenAnsi = "\x1b[2J\x1b[H";

    AnsiDisplayState();
    ~AnsiDisplayState();

    AnsiDisplayState(const AnsiDisplayState&) = delete;
    AnsiDisplayState& operator=(const AnsiDisplayState&) = delete;

    AnsiDisplayState(AnsiDisplayState&&) noexcept;
    AnsiDisplayState& operator=(AnsiDisplayState&&) noexcept;

    void Apply(std::string_view Text);

    [[nodiscard]] std::string BuildSnapshot() const;

    /**
     * Visits renderer-neutral styled runs.
     *
     * @param Visitor Run visitor
     */
    void VisitRuns(AnsiDisplayRunVisitor& Visitor) const;

   private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

}  // namespace mrc
