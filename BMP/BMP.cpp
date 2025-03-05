#include <windows.h>
#include <iostream>
#include <string>

using namespace std;
const string PIC_FOLDER_PATH = "../pic/";
const string PIC_EXTENSION = ".bmp";
int main() {
    string sFileName;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;

    cout << "Enter BMP file name: ";
    cin >> sFileName;

    // Открытие файла
    HANDLE hFile = CreateFileA(
        (PIC_FOLDER_PATH + sFileName + PIC_EXTENSION).c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Error opening file!" << endl;
        system("pause");
        return 1;
    }

    // Чтение заголовков
    DWORD bytesRead;
    if (!ReadFile(hFile, &bmpFileHeader, sizeof(bmpFileHeader), &bytesRead, NULL) ||
        bytesRead != sizeof(bmpFileHeader)) {
        cerr << "Error reading file header!" << endl;
        CloseHandle(hFile);
        system("pause");
        return 1;
    }

    // Проверка сигнатуры BMP
    if (bmpFileHeader.bfType != 0x4D42) {
        cerr << "Not a BMP file!" << endl;
        CloseHandle(hFile);
        system("pause");
        return 1;
    }

    if (!ReadFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &bytesRead, NULL) ||
        bytesRead != sizeof(bmpInfoHeader)) {
        cerr << "Error reading info header!" << endl;
        CloseHandle(hFile);
        system("pause");
        return 1;
    }

    // Вывод информации
    cout << "\nFile analysis results:" << endl;
    cout << "----------------------" << endl;
    cout << "Width: " << bmpInfoHeader.biWidth << " pixels" << endl;
    cout << "Height: " << abs(bmpInfoHeader.biHeight) << " pixels" << endl;
    cout << "Bits per pixel: " << bmpInfoHeader.biBitCount << endl;

    CloseHandle(hFile);
    system("pause");
    return 0;
}