// codeholmes-cli —— 引擎的命令行入口
//
// 设计目的：让扫描逻辑可以在没有界面的情况下调试。
// UI 出问题时，先用本程序复现，即可分清是引擎错还是界面错。
//
// 已实现（阶段 0）：
//   codeholmes-cli --help
//   codeholmes-cli doctor                 工具链自检
//   codeholmes-cli scan <文件或目录> ...   （阶段 1 起填充）
//
// 注意：所有面向用户的输出字符串一律使用英文。
//      中文 Windows + MinGW 下，UTF-8 字面量与控制台/Qt Creator 输出面板的
//      默认字符集不一致会显示乱码；用英文可彻底规避，源码注释仍保持中文。

#include "Config.h"
#include "Issue.h"
#include "Toolchain.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>
#include <QtGlobal>

#include <cstdio>

using namespace codeclean;

namespace {

QTextStream& out()
{
    static QTextStream s(stdout);
    return s;
}

QTextStream& err()
{
    static QTextStream s(stderr);
    return s;
}

void printHelp()
{
    out() << "CodeHolmes CLI - repository-wide C/C++ code quality scanner\n"
          << "\n"
          << "Usage:\n"
          << "  codeholmes-cli doctor                          Check clang-tidy / clang-format availability\n"
          << "  codeholmes-cli scan <path> [options]           Scan a file or a directory\n"
          << "  codeholmes-cli version                         Show version\n"
          << "\n"
          << "scan options:\n"
          << "  --checks <list>        clang-tidy Checks, e.g. '-*,cert-*'\n"
          << "  --style <style>        clang-format style, e.g. Google or file:team.clang-format\n"
          << "  -- <compile args>      Compiler args for clang-tidy, e.g. -- -std=c++17 -Iinclude\n"
          << "  --tidy-only            Run clang-tidy only\n"
          << "  --format-only          Run clang-format only\n"
          << "\n"
          << "Examples:\n"
          << "  codeholmes-cli scan src/main.cpp -- -std=c++17 -Iinclude\n"
          << "  codeholmes-cli scan . --checks '-*,readability-*'\n";
}

int cmdDoctor()
{
    const Toolchain::Status st = Toolchain::probe();
    out() << "Toolchain check\n";
    out() << "  clang-tidy   : " << (st.tidyOk ? "OK" : "NOT FOUND") << "  " << st.tidyPath << "\n";
    if (st.tidyOk) {
        out() << "                version " << st.tidyVersion << "\n";
    }
    out() << "  clang-format : " << (st.formatOk ? "OK" : "NOT FOUND") << "  " << st.formatPath << "\n";
    if (st.formatOk) {
        out() << "                version " << st.formatVersion << "\n";
    }

    if (!st.tidyOk || !st.formatOk) {
        err() << "\nHint: make sure D:\\program\\clang\\bin is on PATH, "
              << "or set the tool paths explicitly in the config.\n";
        return 2;
    }
    out() << "\nAll tools ready.\n";
    return 0;
}

int cmdVersion()
{
    out() << "CodeHolmes " << QCoreApplication::applicationVersion() << "\n";
    return 0;
}

// 阶段 1 将实现：调用 ClangTidyRunner / ClangFormatRunner
int cmdScan(const QStringList& /*args*/)
{
    err() << "scan is not implemented yet (to be filled in from phase 1).\n"
          << "Try: codeholmes-cli doctor\n";
    return 3;
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("codeholmes-cli"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.0.1"));

    const QStringList args = QCoreApplication::arguments();
    if (args.size() < 2) {
        printHelp();
        return 1;
    }

    const QString cmd = args.at(1);
    if (cmd == QStringLiteral("--help") || cmd == QStringLiteral("-h")
        || cmd == QStringLiteral("help")) {
        printHelp();
        return 0;
    }
    if (cmd == QStringLiteral("version") || cmd == QStringLiteral("--version")) {
        return cmdVersion();
    }
    if (cmd == QStringLiteral("doctor")) {
        return cmdDoctor();
    }
    if (cmd == QStringLiteral("scan")) {
        return cmdScan(args.mid(2));
    }

    err() << "Unknown command: " << cmd << "\n\n";
    printHelp();
    return 1;
}
