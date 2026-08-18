#include "mrc/AnsiDisplayState.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mrc {
namespace {
static constexpr size_t MaxDisplayRows = 2000;
static constexpr size_t MaxDisplayColumns = 4096;
static constexpr size_t MaxDisplayRowIndex = MaxDisplayRows - 1;
static constexpr size_t MaxDisplayColumnIndex = MaxDisplayColumns - 1;
static constexpr size_t DisplayTabWidth = 8;
static constexpr std::string_view SgrResetAnsi = "\x1b[0m";

AnsiDisplayColor MakeBasicColor(int Index, bool Bright) {
    AnsiDisplayColor Color;
    Color.ColorKind = AnsiDisplayColor::Kind::Basic;
    Color.Index = Index;
    Color.Bright = Bright;
    return Color;
}

AnsiDisplayColor MakeIndexed256Color(int Index) {
    AnsiDisplayColor Color;
    Color.ColorKind = AnsiDisplayColor::Kind::Indexed256;
    Color.Index = Index;
    return Color;
}

AnsiDisplayColor MakeRgbColor(int Red, int Green, int Blue) {
    AnsiDisplayColor Color;
    Color.ColorKind = AnsiDisplayColor::Kind::Rgb;
    Color.Red = Red;
    Color.Green = Green;
    Color.Blue = Blue;
    return Color;
}

bool IsDefault(const AnsiDisplayColor& Color) {
    return Color.ColorKind == AnsiDisplayColor::Kind::Default;
}

bool IsDefault(const AnsiDisplayStyle& Style) {
    return IsDefault(Style.Foreground) && IsDefault(Style.Background) &&
           !Style.Bold && !Style.Italic && !Style.Underline && !Style.Inverse;
}

std::string ToAnsiParameter(const AnsiDisplayColor& Color, bool Foreground) {
    switch (Color.ColorKind) {
        case AnsiDisplayColor::Kind::Default:
            return {};
        case AnsiDisplayColor::Kind::Basic:
            return std::to_string((Foreground ? (Color.Bright ? 90 : 30)
                                              : (Color.Bright ? 100 : 40)) +
                                  Color.Index);
        case AnsiDisplayColor::Kind::Indexed256:
            return std::string{Foreground ? "38;5;" : "48;5;"} +
                   std::to_string(Color.Index);
        case AnsiDisplayColor::Kind::Rgb:
            return std::string{Foreground ? "38;2;" : "48;2;"} +
                   std::to_string(Color.Red) + ";" +
                   std::to_string(Color.Green) + ";" +
                   std::to_string(Color.Blue);
    }

    return {};
}

std::string ToAnsi(const AnsiDisplayStyle& Style) {
    std::string Output;
    auto AppendParam = [&Output](std::string_view Param) {
        if (Param.empty()) {
            return;
        }
        if (Output.empty()) {
            Output = "\x1b[";
        } else {
            Output += ';';
        }
        Output += Param;
    };

    if (Style.Bold) {
        AppendParam("1");
    }
    if (Style.Italic) {
        AppendParam("3");
    }
    if (Style.Underline) {
        AppendParam("4");
    }
    if (Style.Inverse) {
        AppendParam("7");
    }
    AppendParam(ToAnsiParameter(Style.Foreground, true));
    AppendParam(ToAnsiParameter(Style.Background, false));
    if (Output.empty()) {
        return {};
    }

    Output += 'm';
    return Output;
}

struct DisplayCell {
    std::string Text{" "};
    AnsiDisplayStyle Style;

    bool IsDefaultBlank() const { return Text == " " && IsDefault(Style); }
};

struct DisplayLine {
    std::map<size_t, DisplayCell> Cells;
};

static void AppendCursorMove(std::string& Output, size_t Row, size_t Column) {
    Output += "\x1b[";
    Output += std::to_string(Row + 1);
    Output += ';';
    Output += std::to_string(Column + 1);
    Output += 'H';
}

static bool IsUtf8ContinuationByte(unsigned char Ch) {
    return (Ch & 0xc0) == 0x80;
}

static bool IsCsiParameterOrIntermediateByte(char Ch) {
    return Ch >= 0x20 && Ch <= 0x3f;
}

static size_t GetUtf8ExpectedLength(unsigned char Lead) {
    if (Lead >= 0xc2 && Lead <= 0xdf) {
        return 2;
    } else if (Lead >= 0xe0 && Lead <= 0xef) {
        return 3;
    } else if (Lead >= 0xf0 && Lead <= 0xf4) {
        return 4;
    }

    return 0;
}

struct Utf8SequenceInfo {
    size_t Length = 0;
    bool Incomplete = false;
};

static Utf8SequenceInfo GetUtf8SequenceInfo(std::string_view Text,
                                            size_t Index) {
    unsigned char Lead = static_cast<unsigned char>(Text[Index]);
    size_t Length = GetUtf8ExpectedLength(Lead);
    if (Length == 0) {
        return {};
    }

    for (size_t Offset = 1; Offset < Length; Offset++) {
        if (Index + Offset >= Text.size()) {
            return {.Incomplete = true};
        }
        if (!IsUtf8ContinuationByte(
                static_cast<unsigned char>(Text[Index + Offset]))) {
            return {};
        }
    }

    return {.Length = Length};
}
}  // namespace

struct AnsiDisplayState::Impl {
    enum class EscapeState {
        Normal,
        Escape,
        Csi,
        Osc,
        OscEscape,
    };

    void Apply(std::string_view Text) {
        size_t I = ConsumePendingUtf8(Text);
        for (; I < Text.size();) {
            if (State == EscapeState::Normal) {
                unsigned char Ch = static_cast<unsigned char>(Text[I]);
                if (Ch >= 0x80) {
                    auto Sequence = GetUtf8SequenceInfo(Text, I);
                    if (Sequence.Length != 0) {
                        WriteCharacter(Text.substr(I, Sequence.Length));
                        I += Sequence.Length;
                        continue;
                    }
                    if (Sequence.Incomplete) {
                        PendingUtf8.assign(Text.data() + I, Text.size() - I);
                        break;
                    }
                }
            }

            ProcessCharacter(Text[I]);
            I++;
        }
    }

    void ClearScreenAndHome() {
        ClearScreen();
        CursorRow = 0;
        CursorColumn = 0;
    }

    void ClearScreen() {
        Lines.clear();
        Lines.resize(1);
    }

    std::string BuildSnapshot() const {
        std::string Output{AnsiDisplayState::ClearScreenAnsi};
        Output += SgrResetAnsi;
        AnsiDisplayStyle EmittedStyle;

        for (size_t Row = 0; Row < Lines.size(); Row++) {
            const auto& Cells = Lines[Row].Cells;
            auto It = Cells.begin();
            while (It != Cells.end()) {
                const size_t StartColumn = It->first;
                const auto& Style = It->second.Style;
                AppendCursorMove(Output, Row, StartColumn);
                if (Style != EmittedStyle) {
                    Output += SgrResetAnsi;
                    Output += ToAnsi(Style);
                    EmittedStyle = Style;
                }

                size_t NextColumn = StartColumn;
                while (It != Cells.end() && It->first == NextColumn &&
                       It->second.Style == Style) {
                    Output += It->second.Text;
                    ++It;
                    ++NextColumn;
                }
            }
        }

        Output += SgrResetAnsi;
        AppendCursorMove(Output, SavedCursorRow, SavedCursorColumn);
        Output += "\x1b[s";
        AppendCursorMove(Output, CursorRow, CursorColumn);
        Output += ToAnsi(CurrentStyle);
        return Output;
    }

    void VisitRuns(AnsiDisplayRunVisitor& Visitor) const {
        for (size_t Row = 0; Row < Lines.size(); Row++) {
            const auto& Cells = Lines[Row].Cells;
            auto It = Cells.begin();
            while (It != Cells.end()) {
                const size_t StartColumn = It->first;
                const auto& Style = It->second.Style;
                auto EndIt = It;

                size_t Columns = 0;
                size_t NextColumn = StartColumn;
                while (EndIt != Cells.end() && EndIt->first == NextColumn &&
                       EndIt->second.Style == Style) {
                    ++Columns;
                    ++EndIt;
                    ++NextColumn;
                }

                Visitor.StartRun(Row, StartColumn, Columns, Style);
                while (It != EndIt) {
                    Visitor.AppendRunText(It->second.Text);
                    ++It;
                }
            }
        }
    }

   private:
    size_t ConsumePendingUtf8(std::string_view Text) {
        if (PendingUtf8.empty()) {
            return 0;
        }

        size_t I = 0;
        size_t Length = GetUtf8ExpectedLength(
            static_cast<unsigned char>(PendingUtf8.front()));
        while (I < Text.size()) {
            unsigned char Ch = static_cast<unsigned char>(Text[I]);
            if (!IsUtf8ContinuationByte(Ch)) {
                FlushPendingUtf8();
                return I;
            }

            PendingUtf8 += Text[I];
            I++;
            if (PendingUtf8.size() == Length) {
                WriteCharacter(PendingUtf8);
                PendingUtf8.clear();
                return I;
            }
        }

        return I;
    }

    void FlushPendingUtf8() {
        for (char Ch : PendingUtf8) {
            WriteCharacter(std::string_view{&Ch, 1});
        }
        PendingUtf8.clear();
    }

    void ProcessCharacter(char Ch) {
        switch (State) {
            case EscapeState::Normal:
                ProcessNormalCharacter(Ch);
                break;
            case EscapeState::Escape:
                ProcessEscapeCharacter(Ch);
                break;
            case EscapeState::Csi:
                ProcessCsiCharacter(Ch);
                break;
            case EscapeState::Osc:
                ProcessOscCharacter(Ch);
                break;
            case EscapeState::OscEscape:
                State = EscapeState::Normal;
                break;
        }
    }

    void ProcessNormalCharacter(char Ch) {
        switch (Ch) {
            case '\x1b':
                State = EscapeState::Escape;
                EscapeBuilder.clear();
                break;
            case '\r':
                SetCursorColumn(0);
                break;
            case '\n':
                MoveToNextLine();
                break;
            case '\b':
                MoveCursorColumns(-1);
                break;
            case '\t':
                WriteTab();
                break;
            default:
                if (static_cast<unsigned char>(Ch) >= 0x20 &&
                    static_cast<unsigned char>(Ch) != 0x7f) {
                    WriteCharacter(std::string_view{&Ch, 1});
                }
                break;
        }
    }

    void ProcessEscapeCharacter(char Ch) {
        switch (Ch) {
            case '[':
                State = EscapeState::Csi;
                EscapeBuilder.clear();
                break;
            case ']':
                State = EscapeState::Osc;
                EscapeBuilder.clear();
                break;
            case '7':
                SaveCursor();
                State = EscapeState::Normal;
                break;
            case '8':
                RestoreCursor();
                State = EscapeState::Normal;
                break;
            case 'c':
                ClearScreenAndHome();
                CurrentStyle = {};
                State = EscapeState::Normal;
                break;
            default:
                State = EscapeState::Normal;
                break;
        }
    }

    void ProcessCsiCharacter(char Ch) {
        if (Ch >= 0x40 && Ch <= 0x7e) {
            ProcessCsi(Ch, EscapeBuilder);
            ResetEscapeSequence();
            return;
        }

        if (!IsCsiParameterOrIntermediateByte(Ch) ||
            EscapeBuilder.size() >= MaxCsiLength) {
            ResetEscapeSequence();
            ProcessNormalCharacter(Ch);
            return;
        }

        EscapeBuilder += Ch;
    }

    void ResetEscapeSequence() {
        State = EscapeState::Normal;
        EscapeBuilder.clear();
    }

    void ProcessOscCharacter(char Ch) {
        if (Ch == '\a') {
            State = EscapeState::Normal;
        } else if (Ch == '\x1b') {
            State = EscapeState::OscEscape;
        }
    }

    void ProcessCsi(char Command, std::string_view Parameters) {
        auto Values = ParseCsiParameters(Parameters);
        switch (Command) {
            case 'm':
                ApplySgr(Values);
                break;
            case 'J':
                EraseDisplay(GetValue(Values, 0, 0));
                break;
            case 'K':
                EraseLine(GetValue(Values, 0, 0));
                break;
            case 'A':
                MoveCursorRows(-GetValue(Values, 0, 1));
                break;
            case 'B':
                MoveCursorRows(GetValue(Values, 0, 1));
                break;
            case 'C':
                MoveCursorColumns(GetValue(Values, 0, 1));
                break;
            case 'D':
                MoveCursorColumns(-GetValue(Values, 0, 1));
                break;
            case 'E':
                MoveCursorRows(GetValue(Values, 0, 1));
                SetCursorColumn(0);
                break;
            case 'F':
                MoveCursorRows(-GetValue(Values, 0, 1));
                SetCursorColumn(0);
                break;
            case 'G':
                SetCursorColumn(GetValue(Values, 0, 1) - 1);
                break;
            case 'H':
            case 'f':
                SetCursorPosition(GetValue(Values, 0, 1) - 1,
                                  GetValue(Values, 1, 1) - 1);
                break;
            case 'd':
                SetCursorRow(GetValue(Values, 0, 1) - 1);
                break;
            case 's':
                SaveCursor();
                break;
            case 'u':
                RestoreCursor();
                break;
            default:
                break;
        }
    }

    void ApplySgr(const std::vector<int>& Values) {
        if (Values.empty()) {
            CurrentStyle = {};
            return;
        }

        for (size_t I = 0; I < Values.size(); I++) {
            int Value = Values[I];
            switch (Value) {
                case 0:
                    CurrentStyle = {};
                    break;
                case 1:
                    CurrentStyle.Bold = true;
                    break;
                case 3:
                    CurrentStyle.Italic = true;
                    break;
                case 4:
                    CurrentStyle.Underline = true;
                    break;
                case 7:
                    CurrentStyle.Inverse = true;
                    break;
                case 22:
                    CurrentStyle.Bold = false;
                    break;
                case 23:
                    CurrentStyle.Italic = false;
                    break;
                case 24:
                    CurrentStyle.Underline = false;
                    break;
                case 27:
                    CurrentStyle.Inverse = false;
                    break;
                case 39:
                    CurrentStyle.Foreground = {};
                    break;
                case 49:
                    CurrentStyle.Background = {};
                    break;
                default:
                    ApplyColorSgr(Values, &I);
                    break;
            }
        }
    }

    void ApplyColorSgr(const std::vector<int>& Values, size_t* Index) {
        int Value = Values[*Index];
        if (Value >= 30 && Value <= 37) {
            CurrentStyle.Foreground = MakeBasicColor(Value - 30, false);
        } else if (Value >= 40 && Value <= 47) {
            CurrentStyle.Background = MakeBasicColor(Value - 40, false);
        } else if (Value >= 90 && Value <= 97) {
            CurrentStyle.Foreground = MakeBasicColor(Value - 90, true);
        } else if (Value >= 100 && Value <= 107) {
            CurrentStyle.Background = MakeBasicColor(Value - 100, true);
        } else if (Value == 38 || Value == 48) {
            AnsiDisplayColor Color;
            if (TryReadExtendedColor(Values, Index, &Color)) {
                if (Value == 38) {
                    CurrentStyle.Foreground = Color;
                } else {
                    CurrentStyle.Background = Color;
                }
            }
        }
    }

    static bool TryReadExtendedColor(const std::vector<int>& Values,
                                     size_t* Index, AnsiDisplayColor* Output) {
        if (*Index + 2 >= Values.size()) {
            return false;
        }

        int Mode = Values[*Index + 1];
        if (Mode == 5) {
            *Output = MakeIndexed256Color(Values[*Index + 2]);
            *Index += 2;
            return true;
        }

        if (Mode == 2 && *Index + 4 < Values.size()) {
            *Output = MakeRgbColor(Values[*Index + 2], Values[*Index + 3],
                                   Values[*Index + 4]);
            *Index += 4;
            return true;
        }

        return false;
    }

    void EraseDisplay(int Mode) {
        ClampCursor();
        switch (Mode) {
            case 0:
                EraseLine(0);
                for (size_t Row = CursorRow + 1; Row < Lines.size(); Row++) {
                    Lines[Row].Cells.clear();
                }
                break;
            case 1:
                for (size_t Row = 0; Row < CursorRow && Row < Lines.size();
                     Row++) {
                    Lines[Row].Cells.clear();
                }
                EraseLine(1);
                break;
            case 2:
            case 3:
                ClearScreen();
                break;
            default:
                break;
        }
    }

    void EraseLine(int Mode) {
        ClampCursor();
        EnsureLine(CursorRow);
        auto& Cells = Lines[CursorRow].Cells;
        switch (Mode) {
            case 0:
                Cells.erase(Cells.lower_bound(CursorColumn), Cells.end());
                break;
            case 1:
                for (size_t Column = 0; Column <= CursorColumn; Column++) {
                    SetCell(CursorRow, Column, {" ", CurrentStyle});
                }
                break;
            case 2:
                Cells.clear();
                break;
            default:
                break;
        }
    }

    void WriteCharacter(std::string_view Text) {
        ClampCursor();
        EnsureLine(CursorRow);
        SetCell(CursorRow, CursorColumn,
                {std::string{Text.data(), Text.size()}, CurrentStyle});
        SetCursorColumn(CursorColumn + 1);
    }

    void WriteTab() {
        size_t TargetColumn =
            ((CursorColumn / DisplayTabWidth) + 1) * DisplayTabWidth;
        TargetColumn = std::min(TargetColumn, MaxDisplayColumnIndex);
        while (CursorColumn < TargetColumn) {
            WriteCharacter(" ");
        }
    }

    void SetCell(size_t Row, size_t Column, DisplayCell Cell) {
        if (Row > MaxDisplayRowIndex || Column > MaxDisplayColumnIndex) {
            return;
        }

        EnsureLine(Row);
        auto& Cells = Lines[Row].Cells;
        if (Cell.IsDefaultBlank()) {
            Cells.erase(Column);
        } else {
            Cells[Column] = std::move(Cell);
        }
    }

    void MoveToNextLine() {
        if (CursorRow >= MaxDisplayRowIndex) {
            if (!Lines.empty()) {
                Lines.erase(Lines.begin());
            }
            Lines.emplace_back();
            CursorRow = MaxDisplayRowIndex;
            SavedCursorRow = SavedCursorRow == 0 ? 0 : SavedCursorRow - 1;
        } else {
            SetCursorRow(CursorRow + 1);
        }
        SetCursorColumn(0);
    }

    void EnsureLine(size_t Row) {
        Row = std::min(Row, MaxDisplayRowIndex);
        while (Lines.size() <= Row) {
            Lines.emplace_back();
        }
    }

    void SaveCursor() {
        ClampCursor();
        SavedCursorRow = CursorRow;
        SavedCursorColumn = CursorColumn;
    }

    void RestoreCursor() {
        SetCursorPosition(SavedCursorRow, SavedCursorColumn);
    }

    void MoveCursorRows(int Delta) {
        SetCursorRow(AddClamped(CursorRow, Delta, MaxDisplayRowIndex));
    }

    void MoveCursorColumns(int Delta) {
        SetCursorColumn(AddClamped(CursorColumn, Delta, MaxDisplayColumnIndex));
    }

    void SetCursorPosition(size_t Row, size_t Column) {
        CursorRow = std::min(Row, MaxDisplayRowIndex);
        CursorColumn = std::min(Column, MaxDisplayColumnIndex);
        EnsureLine(CursorRow);
    }

    void SetCursorRow(size_t Row) {
        CursorRow = std::min(Row, MaxDisplayRowIndex);
        EnsureLine(CursorRow);
    }

    void SetCursorColumn(size_t Column) {
        CursorColumn = std::min(Column, MaxDisplayColumnIndex);
    }

    void ClampCursor() {
        CursorRow = std::min(CursorRow, MaxDisplayRowIndex);
        CursorColumn = std::min(CursorColumn, MaxDisplayColumnIndex);
    }

    static size_t AddClamped(size_t Value, int Delta, size_t Max) {
        if (Delta < 0) {
            size_t Amount = static_cast<size_t>(-Delta);
            return Amount > Value ? 0 : Value - Amount;
        }

        size_t Amount = static_cast<size_t>(Delta);
        if (Value > Max - std::min(Amount, Max)) {
            return Max;
        }
        return std::min(Value + Amount, Max);
    }

    static std::vector<int> ParseCsiParameters(std::string_view Parameters) {
        while (!Parameters.empty() &&
               (Parameters.front() == '?' || Parameters.front() == '>' ||
                Parameters.front() == '!')) {
            Parameters.remove_prefix(1);
        }

        std::vector<int> Values;
        Values.reserve(std::min<size_t>((Parameters.size() / 2) + 1, 8));
        int CurrentValue = 0;
        bool HasDigits = false;
        for (char Ch : Parameters) {
            if (Ch >= '0' && Ch <= '9') {
                HasDigits = true;
                if (CurrentValue < 1000000) {
                    CurrentValue = (CurrentValue * 10) + (Ch - '0');
                }
            } else {
                Values.emplace_back(HasDigits ? CurrentValue : 0);
                CurrentValue = 0;
                HasDigits = false;
            }
        }

        if (HasDigits || !Parameters.empty()) {
            Values.emplace_back(HasDigits ? CurrentValue : 0);
        }

        return Values;
    }

    static int GetValue(const std::vector<int>& Values, size_t Index,
                        int Fallback) {
        if (Index >= Values.size() || Values[Index] == 0) {
            return Fallback;
        }
        return Values[Index];
    }

    static constexpr size_t MaxCsiLength = 128;

    std::vector<DisplayLine> Lines{1};
    AnsiDisplayStyle CurrentStyle;
    size_t CursorRow = 0;
    size_t CursorColumn = 0;
    size_t SavedCursorRow = 0;
    size_t SavedCursorColumn = 0;
    EscapeState State = EscapeState::Normal;
    std::string EscapeBuilder;
    std::string PendingUtf8;
};

AnsiDisplayState::AnsiDisplayState() : m_impl{std::make_unique<Impl>()} {}

AnsiDisplayState::~AnsiDisplayState() = default;

AnsiDisplayState::AnsiDisplayState(AnsiDisplayState&&) noexcept = default;

AnsiDisplayState& AnsiDisplayState::operator=(AnsiDisplayState&&) noexcept =
    default;

void AnsiDisplayState::Apply(std::string_view Text) {
    m_impl->Apply(Text);
}

std::string AnsiDisplayState::BuildSnapshot() const {
    return m_impl->BuildSnapshot();
}

void AnsiDisplayState::VisitRuns(AnsiDisplayRunVisitor& Visitor) const {
    m_impl->VisitRuns(Visitor);
}

}  // namespace mrc
