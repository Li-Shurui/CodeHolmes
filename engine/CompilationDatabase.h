#ifndef CODECLEAN_COMPILATIONDATABASE_H
#define CODECLEAN_COMPILATIONDATABASE_H

#include <QHash>
#include <QString>
#include <QStringList>

namespace codeclean {

// compile_commands.json 中单个文件的编译信息
struct CompileCommand {
    QString directory;      // 工作目录
    QString file;           // 源文件（相对 directory 或绝对路径）
    QStringList arguments;  // 编译参数（不含编译器本身）
    QString output;         // 可选
};

// compile_commands.json 解析器
// 说明：
//  - 支持 "arguments" 数组与 "command" 字符串两种格式
//  - 会展开 @xxx.rsp 响应文件（MSVC 常见）
class CompilationDatabase
{
public:
    CompilationDatabase() = default;

    // 从 JSON 文件加载；errorText 非空表示失败原因
    bool load(const QString& jsonPath, QString* errorText = nullptr);

    // 按文件绝对路径查询；找不到返回 false
    bool commandFor(const QString& filePath, CompileCommand* out) const;

    // 数据库中出现的所有源文件（绝对路径，已排序）
    QStringList allFiles() const;

    int size() const { return m_entries.size(); }

private:
    // 把 "command" 字符串按 shell 规则拆成参数表（Windows 引号语义）
    static QStringList splitCommandString(const QString& command);
    // 展开 @file.rsp 响应文件
    static QStringList expandResponseFiles(const QStringList& args, const QString& baseDir);

    QHash<QString, CompileCommand> m_entries; // key = 规范化后的绝对路径
};

} // namespace codeclean

#endif // CODECLEAN_COMPILATIONDATABASE_H
