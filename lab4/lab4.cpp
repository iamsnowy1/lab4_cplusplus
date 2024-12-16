#include <windows.h>
#include <iostream>
#include <iomanip>
#include <aclapi.h>
#include <tchar.h>

void printFileTime(const FILETIME& fileTime, const char* label) {
    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&fileTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
    std::cout << label << ": "
        << std::setfill('0') << std::setw(2) << stLocal.wDay << "/"
        << std::setw(2) << stLocal.wMonth << "/"
        << stLocal.wYear << " "
        << std::setw(2) << stLocal.wHour << ":"
        << std::setw(2) << stLocal.wMinute << ":"
        << std::setw(2) << stLocal.wSecond << "\n";
}

void getFileAttributes(const char* filePath) {

    HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file.\n";
        return;
    }


    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        std::cerr << "Error getting file size.\n";
    }
    else {
        std::cout << "File size: " << fileSize.QuadPart << " bytes\n";
    }


    FILETIME creationTime, accessTime, modificationTime;
    if (GetFileTime(hFile, &creationTime, &accessTime, &modificationTime)) {
        printFileTime(creationTime, "Creation time");
        printFileTime(accessTime, "Last access time");
        printFileTime(modificationTime, "Last modification time");
    }
    else {
        std::cerr << "Error getting file times.\n";
    }


    DWORD attributes = GetFileAttributesA(filePath);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Error getting file attributes.\n";
    }
    else {
        std::cout << "File attributes:\n";
        if (attributes & FILE_ATTRIBUTE_HIDDEN) std::cout << "  Hidden\n";
        if (attributes & FILE_ATTRIBUTE_SYSTEM) std::cout << "  System\n";
        if (attributes & FILE_ATTRIBUTE_READONLY) std::cout << "  Read-only\n";
    }


    PSECURITY_DESCRIPTOR pSD = NULL;
    PSID ownerSID = NULL;
    PACL dacl = NULL;
    DWORD result = GetSecurityInfo(hFile, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
        &ownerSID, NULL, &dacl, NULL, &pSD);
    if (result == ERROR_SUCCESS) {
        char ownerName[256];
        DWORD ownerNameLen = 256;
        char domainName[256];
        DWORD domainNameLen = 256;
        SID_NAME_USE sidType;
        if (LookupAccountSidA(NULL, ownerSID, ownerName, &ownerNameLen, domainName, &domainNameLen, &sidType)) {
            std::cout << "Owner: " << domainName << "\\" << ownerName << "\n";
        }
        else {
            std::cerr << "Error looking up account SID.\n";
        }
        LocalFree(pSD);
    }
    else {
        std::cerr << "Error getting security info.\n";
    }

    CloseHandle(hFile);
}

int main() {
    const char* filePath = "c:\\debug\\code\\lab4\\test.zip ";
    getFileAttributes(filePath);
    return 0;
}