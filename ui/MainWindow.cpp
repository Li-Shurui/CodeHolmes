#include "MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>

namespace codeclean {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("CodeHolmes - repository-wide C/C++ code quality scanner"));
    resize(1280, 800);

    // 阶段 0：仅占位。后续阶段在此逐步加入
    //   阶段 4  工具栏 + 文件树 + 问题表格
    //   阶段 5  代码预览
    //   阶段 7  检查项配置对话框
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    auto* hint = new QLabel(
        QStringLiteral("CodeHolmes - phase 0 skeleton\n\n"
                       "The engine and CLI are ready. Use codeholmes-cli to debug the scan logic.\n"
                       "UI features will be added from phase 4 onward."),
        central);
    hint->setAlignment(Qt::AlignCenter);
    layout->addWidget(hint);
    setCentralWidget(central);
}

MainWindow::~MainWindow() = default;

} // namespace codeclean
