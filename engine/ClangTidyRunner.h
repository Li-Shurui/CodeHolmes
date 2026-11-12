#ifndef CODECLEAN_CLANGTIDYRUNNER_H
#define CODECLEAN_CLANGTIDYRUNNER_H

#include "Issue.h"

#include <QList>
#include <QString>
#include <QStringList>

namespace codeclean {

// clang-tidy 单次运行的结果
struct TidyRunResult {
    bool started = false;      // 进程是否成功启动
    int exitCode = -1;
    QString stdOut;
    QString stdErr;
    QList<Issue> issues;       // 解析后的诊断
};

// 封装一次 clang-tidy 调用
// 典型用法：
//   ClangTidyRunner r;
//   r.setToolPath("D:/program/clang/bin/clang-tidy.exe");
//   r.setChecks("-*,cert-*");
//   r.setCompileArgs({"-std=c++17", "-Iinclude"});
//   r.setExportFixes(tmpPath);            // 需要修复能力时
//   TidyRunResult res = r.run("src/main.cpp");
class ClangTidyRunner
{
public:
    void setToolPath(const QString& path) { m_tool = path; }
    void setChecks(const QString& checks) { m_checks = checks; }
    void setCompileArgs(const QStringList& args) { m_compileArgs = args; }
    void setExportFixes(const QString& yamlPath) { m_exportFixes = yamlPath; }
    void setTimeoutMs(int ms) { m_timeoutMs = ms; }

    // 同步执行（阻塞）。UI 线程请放到工作线程里调用。
    TidyRunResult run(const QString& filePath) const;

    // 把 clang-tidy 的文本输出解析成 Issue 列表
    // 说明：clang-tidy 默认 stdout 是 YAML（Diagnostics: 列表），
    //       但不同版本字段有差异，故用容错的行状态机解析而非强依赖 YAML 库。
    static QList<Issue> parseOutput(const QString& filePath, const QString& output);

private:
    QString m_tool;
    QString m_checks;
    QStringList m_compileArgs;
    QString m_exportFixes;
    int m_timeoutMs = 120000;
};

} // namespace codeclean

#endif // CODECLEAN_CLANGTIDYRUNNER_H
