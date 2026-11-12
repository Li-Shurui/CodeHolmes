#include "Toolchain.h"

#include <QCoreApplication>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

namespace codeclean {

QString Toolchain::findInPath(const QString& exeName)
{
    // 1) 当前应用程序目录
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString inAppDir = QDir::toNativeSeparators(appDir + "/" + exeName);
    if (QFileInfo::exists(inAppDir)) {
        return inAppDir;
    }

    // 2) PATH 环境变量
    const QString pathEnv = QString::fromLocal8Bit(qgetenv("PATH"));
    const QStringList pathDirs = pathEnv.split(QDir::listSeparator(), Qt::SkipEmptyParts);
    for (const QString& dir : pathDirs) {
        const QString candidate = QDir::toNativeSeparators(QDir(dir).absoluteFilePath(exeName));
        if (QFileInfo::exists(candidate)) {
            return candidate;
        }
    }
    return QString();
}

// 运行 "<exe> --version" 并提取版本号
// 注意：clang-tidy 的首行是 "LLVM (http://llvm.org/):"，真正的版本号在后续行；
//       clang-format 则首行就带版本号。故不能简单取第一行。
static QString queryVersion(const QString& exe)
{
    if (exe.isEmpty()) {
        return QString();
    }
    QProcess p;
    p.start(exe, QStringList{QStringLiteral("--version")});
    if (!p.waitForStarted(5000)) {
        return QString();
    }
    if (!p.waitForFinished(15000)) {
        p.kill();
        p.waitForFinished(3000);
        return QString();
    }
    // clang 工具把版本信息打到 stderr 或 stdout 不一，两边都取
    QString text = QString::fromLocal8Bit(p.readAllStandardOutput());
    const QString errText = QString::fromLocal8Bit(p.readAllStandardError());
    if (text.trimmed().isEmpty()) {
        text = errText;
    }

    // 1) 优先匹配 "version X.Y.Z" 形式的行
    static const QRegularExpression reVersion(
        QStringLiteral("(?:version|LLVM version)\\s+([0-9]+\\.[0-9]+\\.[0-9]+)"));
    const QRegularExpressionMatch m = reVersion.match(text);
    if (m.hasMatch()) {
        return m.captured(1);
    }

    // 2) 退化：取第一个含数字的圆点串的行
    const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\r\n]+")),
                                        Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        if (QRegularExpression(QStringLiteral("\\d+\\.\\d+")).match(line).hasMatch()) {
            return line.trimmed();
        }
    }
    return lines.isEmpty() ? QString() : lines.first().trimmed();
}

Toolchain::Status Toolchain::probe(const QString& configuredTidy,
                                  const QString& configuredFormat)
{
    Status st;

    st.tidyPath = configuredTidy.isEmpty() ? findInPath(QStringLiteral("clang-tidy.exe"))
                                           : configuredTidy;
    st.formatPath = configuredFormat.isEmpty() ? findInPath(QStringLiteral("clang-format.exe"))
                                               : configuredFormat;

    if (!st.tidyPath.isEmpty() && QFileInfo::exists(st.tidyPath)) {
        st.tidyVersion = queryVersion(st.tidyPath);
        st.tidyOk = !st.tidyVersion.isEmpty();
    }
    if (!st.formatPath.isEmpty() && QFileInfo::exists(st.formatPath)) {
        st.formatVersion = queryVersion(st.formatPath);
        st.formatOk = !st.formatVersion.isEmpty();
    }
    return st;
}

} // namespace codeclean
