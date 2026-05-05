#pragma once

#include <QObject>
#include <QTimer>
#include <QList>

// ─────────────────────────────────────────────────────────────────────────────
// ResourceMonitor — cross-platform process resource sampler.
//
// Polls RAM, CPU, thread count and per-GPU VRAM at a configurable interval
// and exposes them as Q_PROPERTYs for QML consumption.
//
// GPU detection strategy (no vendor SDK required):
//   NVIDIA : QProcess → nvidia-smi
//   AMD    : QProcess → rocm-smi  (Linux) / not supported on Windows yet
//   Apple  : IOKit   → unified memory (RAM == VRAM on Apple Silicon)
//   Fallback: empty gpus list, gracefully shows "N/A" in UI
// ─────────────────────────────────────────────────────────────────────────────

struct GpuInfo {
    Q_GADGET
    Q_PROPERTY(QString name        MEMBER name)
    Q_PROPERTY(quint64 usedBytes   MEMBER usedBytes)
    Q_PROPERTY(quint64 totalBytes  MEMBER totalBytes)
    Q_PROPERTY(QString usedString  MEMBER usedString)
    Q_PROPERTY(QString totalString MEMBER totalString)
public:
    QString name;
    quint64 usedBytes   = 0;
    quint64 totalBytes  = 0;
    QString usedString;   // e.g. "3.2 GB"
    QString totalString;  // e.g. "8.0 GB"
};
Q_DECLARE_METATYPE(GpuInfo)

class ResourceMonitor : public QObject {
    Q_OBJECT

    Q_PROPERTY(quint64     ramUsedBytes    READ ramUsedBytes    NOTIFY statsUpdated)
    Q_PROPERTY(quint64     ramTotalBytes   READ ramTotalBytes   NOTIFY statsUpdated)
    Q_PROPERTY(QString     ramUsedString   READ ramUsedString   NOTIFY statsUpdated)
    Q_PROPERTY(QString     ramTotalString  READ ramTotalString  NOTIFY statsUpdated)
    Q_PROPERTY(double      cpuPercent      READ cpuPercent      NOTIFY statsUpdated)
    Q_PROPERTY(int         threadCount     READ threadCount     NOTIFY statsUpdated)
    Q_PROPERTY(QList<GpuInfo> gpus         READ gpus            NOTIFY statsUpdated)
    Q_PROPERTY(bool        isRunning       READ isRunning       NOTIFY isRunningChanged)

public:
    explicit ResourceMonitor(QObject* parent = nullptr);
    ~ResourceMonitor() override = default;

    Q_INVOKABLE void start(int intervalMs = 2000);
    Q_INVOKABLE void stop();

    quint64        ramUsedBytes()  const { return mRamUsedBytes; }
    quint64        ramTotalBytes() const { return mRamTotalBytes; }
    QString        ramUsedString() const { return formatBytes(mRamUsedBytes); }
    QString        ramTotalString()const { return formatBytes(mRamTotalBytes); }
    double         cpuPercent()    const { return mCpuPercent; }
    int            threadCount()   const { return mThreadCount; }
    QList<GpuInfo> gpus()          const { return mGpus; }
    bool           isRunning()     const { return mTimer.isActive(); }

signals:
    void statsUpdated();
    void isRunningChanged();

private:
    void sample();

    // Per-platform implementations — each returns the metric or 0/empty on failure
    quint64        readRamUsedBytes()  const;
    quint64        readRamTotalBytes() const;
    double         readCpuPercent();          // stateful — needs previous sample
    int            readThreadCount()   const;
    QList<GpuInfo> readGpuInfo()       const;

    // GPU backend helpers
    QList<GpuInfo> readNvidiaSmi()  const;
    QList<GpuInfo> readRocmSmi()    const;
    QList<GpuInfo> readAppleIOKit() const;

    static QString formatBytes(quint64 bytes);

    QTimer         mTimer;
    quint64        mRamUsedBytes  = 0;
    quint64        mRamTotalBytes = 0;
    double         mCpuPercent    = 0.0;
    int            mThreadCount   = 0;
    QList<GpuInfo> mGpus;

    // CPU delta state — platform-specific previous-sample bookkeeping
#if defined(Q_OS_WIN)
    quint64 mPrevProcessKernel = 0;
    quint64 mPrevProcessUser   = 0;
    quint64 mPrevSystemKernel  = 0;
    quint64 mPrevSystemUser    = 0;
    quint64 mPrevSystemIdle    = 0;
#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
    quint64 mPrevProcessTicks  = 0;
    quint64 mPrevTotalTicks    = 0;
#elif defined(Q_OS_MACOS)
    quint64 mPrevProcessTicks  = 0;
    quint64 mPrevTotalTicks    = 0;
#endif
};
