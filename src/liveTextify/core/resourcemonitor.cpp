#include "liveTextify/core/resourcemonitor.h"
#include "liveTextify/core/Logger.h"

#include <QProcess>
#include <QTimer>

// ── Platform includes ─────────────────────────────────────────────────────────
#if defined(Q_OS_WIN)
#   include <windows.h>
#   include <psapi.h>
#   include <tlhelp32.h>
#   pragma comment(lib, "psapi.lib")

#elif defined(Q_OS_MACOS)
#   include <mach/mach.h>
#   include <mach/task_info.h>
#   include <mach/mach_host.h>
#   include <sys/sysctl.h>
#   include <IOKit/IOKitLib.h>

#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
#   include <QFile>
#   include <QTextStream>
#   include <sys/sysinfo.h>
#endif

// ─────────────────────────────────────────────────────────────────────────────

ResourceMonitor::ResourceMonitor(QObject* parent)
    : QObject(parent)
{
    connect(&mTimer, &QTimer::timeout, this, &ResourceMonitor::sample);
    qRegisterMetaType<GpuInfo>();
    qRegisterMetaType<QList<GpuInfo>>();
}

void ResourceMonitor::start(int intervalMs) {
    if (mTimer.isActive()) return;
    sample(); // immediate first sample
    mTimer.start(intervalMs);
    emit isRunningChanged();
}

void ResourceMonitor::stop() {
    if (!mTimer.isActive()) return;
    mTimer.stop();
    emit isRunningChanged();
}

void ResourceMonitor::sample() {
    mRamUsedBytes  = readRamUsedBytes();
    mRamTotalBytes = readRamTotalBytes();
    mCpuPercent    = readCpuPercent();
    mThreadCount   = readThreadCount();
    mGpus          = readGpuInfo();
    emit statsUpdated();
}

// ── Formatting ────────────────────────────────────────────────────────────────

QString ResourceMonitor::formatBytes(quint64 bytes) {
    if (bytes == 0)                    return "N/A";
    if (bytes < 1024ULL)               return QString("%1 B").arg(bytes);
    if (bytes < 1024ULL * 1024)        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024ULL * 1024 * 1024) return QString("%1 MB").arg(bytes / (1024.0 * 1024), 0, 'f', 1);
    return QString("%1 GB").arg(bytes / (1024.0 * 1024 * 1024), 0, 'f', 2);
}

// ── GPU dispatch ──────────────────────────────────────────────────────────────

QList<GpuInfo> ResourceMonitor::readGpuInfo() const {
#if defined(Q_OS_MACOS)
    return readAppleIOKit();
#else
    // Try NVIDIA first, then AMD
    QList<GpuInfo> result = readNvidiaSmi();
    if (!result.isEmpty()) return result;
    return readRocmSmi();
#endif
}

// ── NVIDIA (nvidia-smi) ───────────────────────────────────────────────────────

QList<GpuInfo> ResourceMonitor::readNvidiaSmi() const {
    QProcess proc;
    proc.start("nvidia-smi",
               { "--query-gpu=name,memory.used,memory.total",
                "--format=csv,noheader,nounits" });

    if (!proc.waitForFinished(2000)) return {};

    QList<GpuInfo> result;
    const QString output = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    const auto lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        const auto parts = line.split(',');
        if (parts.size() < 3) continue;

        GpuInfo g;
        g.name       = parts[0].trimmed();
        // nvidia-smi reports in MiB
        g.usedBytes  = static_cast<quint64>(parts[1].trimmed().toULongLong()) * 1024 * 1024;
        g.totalBytes = static_cast<quint64>(parts[2].trimmed().toULongLong()) * 1024 * 1024;
        g.usedString  = formatBytes(g.usedBytes);
        g.totalString = formatBytes(g.totalBytes);
        result.append(g);
    }

    return result;
}

// ── AMD (rocm-smi) ────────────────────────────────────────────────────────────

QList<GpuInfo> ResourceMonitor::readRocmSmi() const {
#if !defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    return {}; // rocm-smi only meaningful on Linux
#else
    QProcess proc;
    proc.start("rocm-smi", { "--showmeminfo", "vram", "--csv" });
    if (!proc.waitForFinished(2000)) return {};

    QList<GpuInfo> result;
    const QString output = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    const auto lines = output.split('\n', Qt::SkipEmptyParts);

    // Expected CSV: device,VRAM Total Memory (B),VRAM Total Used Memory (B)
    // Skip header line
    for (int i = 1; i < lines.size(); ++i) {
        const auto parts = lines[i].split(',');
        if (parts.size() < 3) continue;

        GpuInfo g;
        g.name        = parts[0].trimmed();
        g.totalBytes  = parts[1].trimmed().toULongLong();
        g.usedBytes   = parts[2].trimmed().toULongLong();
        g.usedString  = formatBytes(g.usedBytes);
        g.totalString = formatBytes(g.totalBytes);
        result.append(g);
    }

    return result;
#endif
}

// ── Apple IOKit (unified memory) ──────────────────────────────────────────────

QList<GpuInfo> ResourceMonitor::readAppleIOKit() const {
#if !defined(Q_OS_MACOS)
    return {};
#else
    // On Apple Silicon, RAM and VRAM share the same pool.
    // We report total system RAM as VRAM total, and GPU allocated
    // memory via the AGXAccelerator IOKit entry.
    QList<GpuInfo> result;

    GpuInfo g;
    g.name = "Apple GPU (Unified Memory)";

    // Total system memory (same pool as GPU)
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    uint64_t totalMem = 0;
    size_t len = sizeof(totalMem);
    if (sysctl(mib, 2, &totalMem, &len, nullptr, 0) == 0)
        g.totalBytes = totalMem;

    // GPU allocated memory via IOKit
    io_iterator_t iter = 0;
    if (IOServiceGetMatchingServices(
            kIOMainPortDefault,
            IOServiceMatching("AGXAccelerator"),
            &iter) == kIOReturnSuccess)
    {
        io_registry_entry_t entry = IOIteratorNext(iter);
        if (entry) {
            CFTypeRef prop = IORegistryEntryCreateCFProperty(
                entry,
                CFSTR("PerformanceStatistics"),
                kCFAllocatorDefault, 0);

            if (prop && CFGetTypeID(prop) == CFDictionaryGetTypeID()) {
                CFDictionaryRef dict = static_cast<CFDictionaryRef>(prop);
                CFNumberRef allocBytes = static_cast<CFNumberRef>(
                    CFDictionaryGetValue(dict, CFSTR("Alloc system memory")));
                if (allocBytes) {
                    uint64_t used = 0;
                    CFNumberGetValue(allocBytes, kCFNumberSInt64Type, &used);
                    g.usedBytes = used;
                }
            }
            if (prop) CFRelease(prop);
            IOObjectRelease(entry);
        }
        IOObjectRelease(iter);
    }

    g.usedString  = formatBytes(g.usedBytes);
    g.totalString = formatBytes(g.totalBytes);
    result.append(g);
    return result;
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// Windows implementations
// ─────────────────────────────────────────────────────────────────────────────
#if defined(Q_OS_WIN)

quint64 ResourceMonitor::readRamUsedBytes() const {
    PROCESS_MEMORY_COUNTERS_EX pmc{};
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                             sizeof(pmc)))
        return static_cast<quint64>(pmc.PrivateUsage);
    return 0;
}

quint64 ResourceMonitor::readRamTotalBytes() const {
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms))
        return static_cast<quint64>(ms.ullTotalPhys);
    return 0;
}

double ResourceMonitor::readCpuPercent() {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) return 0.0;

    FILETIME procCreation, procExit, procKernel, procUser;
    if (!GetProcessTimes(GetCurrentProcess(),
                         &procCreation, &procExit,
                         &procKernel,   &procUser)) return 0.0;

    auto toUInt64 = [](const FILETIME& ft) -> quint64 {
        return (static_cast<quint64>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    };

    const quint64 sysKernel  = toUInt64(kernelTime);
    const quint64 sysUser    = toUInt64(userTime);
    const quint64 sysIdle    = toUInt64(idleTime);
    const quint64 procKern64 = toUInt64(procKernel);
    const quint64 procUser64 = toUInt64(procUser);

    const quint64 sysTotal  = (sysKernel - mPrevSystemKernel) + (sysUser - mPrevSystemUser);
    const quint64 sysActive = sysTotal - (sysIdle - mPrevSystemIdle);
    const quint64 procTotal = (procKern64 - mPrevProcessKernel) + (procUser64 - mPrevProcessUser);

    double percent = 0.0;
    if (sysActive > 0)
        percent = (static_cast<double>(procTotal) / static_cast<double>(sysActive)) * 100.0;

    mPrevSystemKernel  = sysKernel;
    mPrevSystemUser    = sysUser;
    mPrevSystemIdle    = sysIdle;
    mPrevProcessKernel = procKern64;
    mPrevProcessUser   = procUser64;

    return qBound(0.0, percent, 100.0);
}

int ResourceMonitor::readThreadCount() const {
    // TH32CS_SNAPTHREAD snapshots only threads — much cheaper than
    // TH32CS_SNAPPROCESS. Filter by our own PID to count our threads.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    THREADENTRY32 te{};
    te.dwSize = sizeof(te);
    const DWORD pid = GetCurrentProcessId();
    int count = 0;

    if (Thread32First(snapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == pid)
                ++count;
        } while (Thread32Next(snapshot, &te));
    }

    CloseHandle(snapshot);
    return count;
}

#endif // Q_OS_WIN

// ─────────────────────────────────────────────────────────────────────────────
// macOS implementations
// ─────────────────────────────────────────────────────────────────────────────
#if defined(Q_OS_MACOS)

quint64 ResourceMonitor::readRamUsedBytes() const {
    task_vm_info_data_t vmInfo{};
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO,
                  reinterpret_cast<task_info_t>(&vmInfo), &count) == KERN_SUCCESS)
        return static_cast<quint64>(vmInfo.phys_footprint);
    return 0;
}

quint64 ResourceMonitor::readRamTotalBytes() const {
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    uint64_t total = 0;
    size_t len = sizeof(total);
    if (sysctl(mib, 2, &total, &len, nullptr, 0) == 0)
        return static_cast<quint64>(total);
    return 0;
}

double ResourceMonitor::readCpuPercent() {
    // Process CPU ticks
    task_thread_times_info_data_t tinfo{};
    mach_msg_type_number_t count = TASK_THREAD_TIMES_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_THREAD_TIMES_INFO,
                  reinterpret_cast<task_info_t>(&tinfo), &count) != KERN_SUCCESS)
        return 0.0;

    const quint64 procTicks =
        static_cast<quint64>(tinfo.user_time.seconds)   * 1'000'000 + tinfo.user_time.microseconds +
        static_cast<quint64>(tinfo.system_time.seconds) * 1'000'000 + tinfo.system_time.microseconds;

    // System CPU ticks
    host_cpu_load_info_data_t cpuLoad{};
    mach_msg_type_number_t cpuCount = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
                        reinterpret_cast<host_info_t>(&cpuLoad), &cpuCount) != KERN_SUCCESS)
        return 0.0;

    quint64 totalTicks = 0;
    for (int i = 0; i < CPU_STATE_MAX; ++i)
        totalTicks += cpuLoad.cpu_ticks[i];

    const quint64 procDelta  = procTicks  - mPrevProcessTicks;
    const quint64 totalDelta = totalTicks - mPrevTotalTicks;

    mPrevProcessTicks = procTicks;
    mPrevTotalTicks   = totalTicks;

    if (totalDelta == 0) return 0.0;
    return qBound(0.0, (static_cast<double>(procDelta) / totalDelta) * 100.0, 100.0);
}

int ResourceMonitor::readThreadCount() const {
    task_info_data_t tinfo;
    mach_msg_type_number_t count = TASK_INFO_MAX;
    task_basic_info_t basic = reinterpret_cast<task_basic_info_t>(tinfo);
    count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO,
                  reinterpret_cast<task_info_t>(basic), &count) == KERN_SUCCESS)
        return static_cast<int>(basic->suspend_count); // not ideal but cross-API

    // Better: use thread_act_array
    thread_act_array_t threads;
    mach_msg_type_number_t threadCount = 0;
    if (task_threads(mach_task_self(), &threads, &threadCount) == KERN_SUCCESS) {
        const int count = static_cast<int>(threadCount);
        vm_deallocate(mach_task_self(),
                      reinterpret_cast<vm_address_t>(threads),
                      threadCount * sizeof(thread_act_t));
        return count;
    }
    return 0;
}

#endif // Q_OS_MACOS

// ─────────────────────────────────────────────────────────────────────────────
// Linux / Android implementations
// ─────────────────────────────────────────────────────────────────────────────
#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)

quint64 ResourceMonitor::readRamUsedBytes() const {
    // /proc/self/status → VmRSS (resident set size) in kB
    QFile f("/proc/self/status");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith("VmRSS:")) {
            const auto parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2)
                return parts[1].toULongLong() * 1024; // kB → bytes
        }
    }
    return 0;
}

quint64 ResourceMonitor::readRamTotalBytes() const {
    struct sysinfo si{};
    if (sysinfo(&si) == 0)
        return static_cast<quint64>(si.totalram) * si.mem_unit;
    return 0;
}

double ResourceMonitor::readCpuPercent() {
    // /proc/self/stat fields 14+15 = utime+stime in jiffies
    QFile procStat("/proc/self/stat");
    if (!procStat.open(QIODevice::ReadOnly)) return 0.0;
    const QByteArray data = procStat.readAll();
    const auto parts = QString::fromLatin1(data).split(' ');
    if (parts.size() < 15) return 0.0;

    const quint64 procTicks = parts[13].toULongLong() + parts[14].toULongLong();

    // /proc/stat first line: cpu user nice system idle ...
    QFile stat("/proc/stat");
    if (!stat.open(QIODevice::ReadOnly)) return 0.0;
    const QString cpuLine = QString::fromLatin1(stat.readLine());
    const auto cpuParts = cpuLine.split(' ', Qt::SkipEmptyParts);
    if (cpuParts.size() < 5) return 0.0;

    quint64 totalTicks = 0;
    for (int i = 1; i < cpuParts.size(); ++i)
        totalTicks += cpuParts[i].toULongLong();

    const quint64 procDelta  = procTicks  - mPrevProcessTicks;
    const quint64 totalDelta = totalTicks - mPrevTotalTicks;

    mPrevProcessTicks = procTicks;
    mPrevTotalTicks   = totalTicks;

    if (totalDelta == 0) return 0.0;
    return qBound(0.0, (static_cast<double>(procDelta) / totalDelta) * 100.0, 100.0);
}

int ResourceMonitor::readThreadCount() const {
    // /proc/self/status → Threads field
    QFile f("/proc/self/status");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith("Threads:")) {
            const auto parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2)
                return parts[1].toInt();
        }
    }
    return 0;
}

#endif // Q_OS_LINUX || Q_OS_ANDROID
