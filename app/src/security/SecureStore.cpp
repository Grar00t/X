#include "SecureStore.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")
QByteArray SecureStore::protect(const QByteArray& plain){
    DATA_BLOB in{(DWORD)plain.size(), (BYTE*)plain.data()}, out{};
    if(CryptProtectData(&in, L"GrA", nullptr, nullptr, nullptr, 0, &out)){
        QByteArray b((char*)out.pbData, out.cbData);
        LocalFree(out.pbData);
        return b;
    }
    return {};
}
QByteArray SecureStore::unprotect(const QByteArray& blob){
    DATA_BLOB in{(DWORD)blob.size(), (BYTE*)blob.data()}, out{};
    LPWSTR desc = nullptr;
    if(CryptUnprotectData(&in, &desc, nullptr, nullptr, nullptr, 0, &out)){
        QByteArray b((char*)out.pbData, out.cbData);
        LocalFree(out.pbData);
        LocalFree(desc);
        return b;
    }
    return {};
}
#else
QByteArray SecureStore::protect(const QByteArray& plain){ return plain; }
QByteArray SecureStore::unprotect(const QByteArray& blob){ return blob; }
#endif
