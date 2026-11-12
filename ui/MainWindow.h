#ifndef CODECLEAN_MAINWINDOW_H
#define CODECLEAN_MAINWINDOW_H

#include <QMainWindow>

namespace codeclean {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};

} // namespace codeclean

#endif // CODECLEAN_MAINWINDOW_H
