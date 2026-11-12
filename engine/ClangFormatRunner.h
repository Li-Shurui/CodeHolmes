#ifndef CODECLEAN_CLANGFORMATRUNNER_H
#define CODECLEAN_CLANGFORMATRUNNER_H

#include "Issue.h"

#include <QList>
#include <QString>

namespace codeclean {

// clang-format 单次运行结果
struct FormatRunResult {
    bool started = false;
    int exitCode = -1;
    QString stdOut;
    QString stdErr;
    bool needsFormatting = false; // 是否存在需要重排的位置
    QList<Issue> issues;          // 由 replacement 转换而来（category = Format）
};

// 封装一次 clang-format 调用
// 关键点：
//  - 用 --output-replacements-xml 拿到具体替换位置，而不是只判断"是否需要格式化"
//  - replacement 的 offset 是「字节偏移」，转行列号时必须按字节读取原文件
class ClangFormatRunner
{
public:
    void setToolPath(const QString& path) { m_tool = path; }
    void setStyle(const QString& style) { m_style = style; }   // 风格名或 "file:<path>"
    void setTimeoutMs(int ms) { m_timeoutMs = ms; }

    FormatRunResult run(const QString& filePath) const;

    // 把字节偏移转换为 (行, 列)；行列从 1 开始
    // content 必须是文件的「字节」内容（QByteArray），否则含中文/CRLF 时会算错
    static void offsetToLineColumn(const QByteArray& content, int offset,
                                   int* line, int* column);

    // 解析 --output-replacements-xml 的输出
    static QList<FixIt> parseReplacementsXml(const QString& xml, bool* ok);

private:
    QString m_tool;
    QString m_style;
    int m_timeoutMs = 60000;
};

} // namespace codeclean

#endif // CODECLEAN_CLANGFORMATRUNNER_H
