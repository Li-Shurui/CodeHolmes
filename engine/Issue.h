#ifndef CODECLEAN_ISSUE_H
#define CODECLEAN_ISSUE_H

#include <QList>
#include <QMetaType>
#include <QString>

namespace codeclean {

// 一条可自动修复的替换建议（clang-tidy 的 fixit / clang-format 的 replacement）
struct FixIt {
    int offset = 0;      // 相对文件起始的字节偏移
    int length = 0;      // 被替换的长度（0 表示纯插入）
    QString replacement; // 替换成的文本
};

// 统一的问题模型：clang-tidy 与 clang-format 的产出都归一到这个结构
struct Issue {
    enum class Severity { Error, Warning, Suggestion, Note, Ignored };
    enum class Category { Security, Defect, Style, Format, Convention };

    Severity severity = Severity::Warning;
    Category category = Category::Convention;

    QString filePath;    // 绝对路径
    int line = 0;
    int column = 0;
    QString ruleId;      // 如 "cert-msc30-c"、"clang-format"
    QString message;

    bool autoFixable = false;
    QList<FixIt> fixIts;

    // 便于表格/树视图排序与过滤
    int severityRank() const
    {
        switch (severity) {
        case Severity::Error:     return 0;
        case Severity::Warning:   return 1;
        case Severity::Suggestion:return 2;
        case Severity::Note:      return 3;
        case Severity::Ignored:   return 4;
        }
        return 5;
    }

    // 严重性与分类的显示名。
    // 注意：UI 与 CLI 的可见字符串一律用英文，规避中文 Windows 下的字符集乱码；
    //      若将来需要中文界面，应改用 Qt 翻译机制（tr() + .qm），而非硬编码字面量。
    static QString severityName(Severity s)
    {
        switch (s) {
        case Severity::Error:      return QStringLiteral("Error");
        case Severity::Warning:    return QStringLiteral("Warning");
        case Severity::Suggestion: return QStringLiteral("Suggestion");
        case Severity::Note:       return QStringLiteral("Note");
        case Severity::Ignored:    return QStringLiteral("Ignored");
        }
        return QStringLiteral("Unknown");
    }

    static QString categoryName(Category c)
    {
        switch (c) {
        case Category::Security:   return QStringLiteral("Security");
        case Category::Defect:     return QStringLiteral("Defect");
        case Category::Style:      return QStringLiteral("Convention");
        case Category::Format:     return QStringLiteral("Format");
        case Category::Convention: return QStringLiteral("Convention");
        }
        return QStringLiteral("Convention");
    }
};

} // namespace codeclean

Q_DECLARE_METATYPE(codeclean::Issue)
Q_DECLARE_METATYPE(codeclean::Issue::Severity)

#endif // CODECLEAN_ISSUE_H
