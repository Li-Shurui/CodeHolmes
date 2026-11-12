#ifndef CODECLEAN_CONFIG_H
#define CODECLEAN_CONFIG_H

#include "Issue.h"

#include <QHash>
#include <QString>
#include <QStringList>

namespace codeclean {

// 一个路径 → clang-format 风格的指派
struct StyleOverride {
    QString glob;        // 如 "src/**/*.cpp"
    QString style;       // 风格名，或 "file:<路径>" 指向自定义 .clang-format
    bool reformat = true; // false 表示该路径不做格式检查（如 third_party）
};

// 单条规则的严重性覆盖
struct RuleOverride {
    QString ruleId;
    Issue::Severity severity = Issue::Severity::Warning;
    // 注意：severity == Ignored 表示忽略该规则
};

// 一套完整的扫描方案（profile）
struct Profile {
    QString name;

    // ---- clang-tidy ----
    QStringList bundles;                  // 规范集，如 ["cert", "readability"]
    QStringList extraChecks;              // 额外单条规则
    QList<RuleOverride> overrides;        // 单条规则覆盖

    // ---- clang-format ----
    QString defaultStyle = QStringLiteral("Google");
    QList<StyleOverride> styleOverrides;

    // ---- 质量门禁 ----
    int maxErrors = 0;
    int maxWarnings = 50;
};

// 全局配置：.codeclean.yaml 的内存表示
class Config
{
public:
    QHash<QString, Profile> profiles;   // key = profile 名
    QString activeProfile = QStringLiteral("default");

    // 扫描根目录与工具路径（留空则从 PATH 查找）
    QString clangTidyBin;
    QString clangFormatBin;
    int parallelJobs = 0;               // 0 = 自动（CPU 核数）

    // 排除的目录/文件 glob
    QStringList excludes;

    static Config defaultConfig();

    const Profile* activeProfilePtr() const;
    // 把规范集展开成 clang-tidy 的 Checks 字符串，如 "-*,cert-*,readability-*"
    QString buildChecksString(const Profile& p) const;
    // 判断某文件最终使用哪个 clang-format 风格
    QString resolveStyle(const Profile& p, const QString& filePath) const;
};

} // namespace codeclean

#endif // CODECLEAN_CONFIG_H
