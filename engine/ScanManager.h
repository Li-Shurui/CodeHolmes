#ifndef CODECLEAN_SCANMANAGER_H
#define CODECLEAN_SCANMANAGER_H

#include "Config.h"
#include "Issue.h"

#include <QAtomicInt>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

class QThreadPool;

namespace codeclean {

// 扫描选项：一次扫描的输入
struct ScanRequest {
    QString rootDir;                     // 仓库根目录
    QString databasePath;                // compile_commands.json，可空
    QStringList files;                   // 显式指定文件（空则用数据库/全量）
    bool runTidy = true;
    bool runFormat = true;
};

// 扫描进度
struct ScanProgress {
    int total = 0;
    int done = 0;
    QString currentFile;
};

// 调度器：线程池 + 取消 + 结果汇总
// 说明：
//  - 每个任务在工作线程内各自创建 QProcess（QProcess 不可跨线程共享）
//  - 结果通过队列化信号回主线程，UI 直接连这些信号即可
class ScanManager : public QObject
{
    Q_OBJECT
public:
    explicit ScanManager(QObject* parent = nullptr);
    ~ScanManager() override;

    void setConfig(const Config& cfg) { m_config = cfg; }
    const Config& config() const { return m_config; }

    // 异步开始扫描（立即返回）
    void start(const ScanRequest& req);
    // 请求取消；在工作线程检查到取消标志后尽快停止
    void cancel();
    bool isRunning() const { return m_running.loadAcquire() != 0; }

    static QStringList collectSourceFiles(const QString& rootDir, const QStringList& excludes);

Q_SIGNALS:
    void started();
    void progressChanged(const codeclean::ScanProgress& progress);
    void issueFound(const codeclean::Issue& issue);
    void fileFinished(const QString& filePath, int issueCount);
    void finished(const QList<codeclean::Issue>& allIssues);
    void failed(const QString& message);

private:
    void runScan(const ScanRequest& req);

    Config m_config;
    QThreadPool* m_pool = nullptr;
    QAtomicInt m_cancelFlag{0};
    QAtomicInt m_running{0};
    QList<Issue> m_results;
    int m_total = 0;
    int m_done = 0;
};

} // namespace codeclean

#endif // CODECLEAN_SCANMANAGER_H
