#ifndef CODECLEAN_TOOLCHAIN_H
#define CODECLEAN_TOOLCHAIN_H

#include <QString>

namespace codeclean {

// 外部工具链探测：clang-tidy / clang-format
// 优先用配置里显式指定的路径，否则按 PATH 查找
class Toolchain
{
public:
    struct Status {
        QString tidyPath;
        QString formatPath;
        QString tidyVersion;
        QString formatVersion;
        bool tidyOk = false;
        bool formatOk = false;
    };

    // 探测一次；configuredTidy/configuredFormat 为空则走 PATH
    static Status probe(const QString& configuredTidy = QString(),
                        const QString& configuredFormat = QString());

    // 在 PATH 中按名字查找可执行文件；找不到返回空串
    static QString findInPath(const QString& exeName);
};

} // namespace codeclean

#endif // CODECLEAN_TOOLCHAIN_H
