#include "platform/win/win_lnk_resolver.h"

#include <QFileInfo>
#include <QString>

#ifdef Q_OS_WIN
#include <shlobj.h>
#include <shobjidl.h>
#include <windows.h>
#endif

namespace quickdeck {

Result<QString> resolve_lnk_target(const QString &lnk_path)
{
    if (!lnk_path.endsWith(QStringLiteral(".lnk"), Qt::CaseInsensitive)) {
        return Result<QString>::ok(lnk_path);
    }

#ifdef Q_OS_WIN
    IShellLinkW *shell_link = nullptr;
    IPersistFile *persist_file = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IShellLinkW, reinterpret_cast<void **>(&shell_link));
    if (FAILED(hr) || shell_link == nullptr) {
        return Result<QString>::fail(QStringLiteral("Failed to create shell link instance"));
    }

    hr = shell_link->QueryInterface(IID_IPersistFile, reinterpret_cast<void **>(&persist_file));
    if (FAILED(hr) || persist_file == nullptr) {
        shell_link->Release();
        return Result<QString>::fail(QStringLiteral("Failed to query persist file"));
    }

    hr = persist_file->Load(reinterpret_cast<LPCWSTR>(lnk_path.utf16()), STGM_READ);
    if (FAILED(hr)) {
        persist_file->Release();
        shell_link->Release();
        return Result<QString>::fail(QStringLiteral("Failed to load shortcut: %1").arg(lnk_path));
    }

    wchar_t target_path[MAX_PATH] = {};
    hr = shell_link->GetPath(target_path, MAX_PATH, nullptr, SLGP_RAWPATH);
    persist_file->Release();
    shell_link->Release();

    if (FAILED(hr) || target_path[0] == L'\0') {
        return Result<QString>::fail(QStringLiteral("Shortcut has no target: %1").arg(lnk_path));
    }

    return Result<QString>::ok(QString::fromWCharArray(target_path));
#else
    Q_UNUSED(lnk_path)
    return Result<QString>::fail(QStringLiteral("LNK resolution supported on Windows only"));
#endif
}

} // namespace quickdeck
