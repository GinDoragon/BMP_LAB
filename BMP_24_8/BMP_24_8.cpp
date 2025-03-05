#include <windows.h>
#include <iostream>
#include <string>
#include <cmath>

using namespace std;
const string PIC_FOLDER_PATH = "../pic/";
const string PIC_EXTENSION = ".bmp";
const string OUTPUT_PIC_NAME = "8bit";
int main() {
    string inputFileName;
    cout << "Enter input 24-bit BMP file name: ";
    cin >> inputFileName;

    // Открытие исходного файла
    HANDLE hInputFile = CreateFileA(
        (PIC_FOLDER_PATH + inputFileName + PIC_EXTENSION).c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hInputFile == INVALID_HANDLE_VALUE) {
        cerr << "Error opening input file!" << endl;
        system("pause");
        return 1;
    }

    // Чтение заголовков
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    DWORD bytesRead;

    // Чтение BITMAPFILEHEADER
    if (!ReadFile(hInputFile, &bmpFileHeader, sizeof(bmpFileHeader), &bytesRead, NULL) ||
        bytesRead != sizeof(bmpFileHeader)) {
        cerr << "Error reading file header!" << endl;
        CloseHandle(hInputFile);
        return 1;
    }

    // Проверка сигнатуры BMP
    if (bmpFileHeader.bfType != 0x4D42) {
        cerr << "Not a BMP file!" << endl;
        CloseHandle(hInputFile);
        return 1;
    }

    // Чтение BITMAPINFOHEADER
    if (!ReadFile(hInputFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &bytesRead, NULL) ||
        bytesRead != sizeof(bmpInfoHeader)) {
        cerr << "Error reading info header!" << endl;
        CloseHandle(hInputFile);
        return 1;
    }

    // Проверка формата
    if (bmpInfoHeader.biBitCount != 24 || bmpInfoHeader.biCompression != BI_RGB) {
        cerr << "Only uncompressed 24-bit BMP supported!" << endl;
        CloseHandle(hInputFile);
        return 1;
    }

    // Создание выходного файла
    HANDLE hOutputFile = CreateFileA(
        (PIC_FOLDER_PATH + OUTPUT_PIC_NAME + PIC_EXTENSION).c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hOutputFile == INVALID_HANDLE_VALUE) {
        cerr << "Error creating output file!" << endl;
        CloseHandle(hInputFile);
        return 1;
    }

    // Рассчет параметров
    const int width = bmpInfoHeader.biWidth;
    const int height = abs(bmpInfoHeader.biHeight);
    const int inputRowSize = ((width * 3 + 3) / 4) * 4; // Выравнивание для 24 бит
    const int outputRowSize = ((width + 3) / 4) * 4;     // Выравнивание для 8 бит

    // Подготовка новых заголовков
    BITMAPFILEHEADER newFileHeader = bmpFileHeader;
    BITMAPINFOHEADER newInfoHeader = bmpInfoHeader;

    // Обновление параметров
    newInfoHeader.biBitCount = 8;
    newInfoHeader.biClrUsed = 256;
    newInfoHeader.biClrImportant = 256;
    newInfoHeader.biSizeImage = outputRowSize * height;

    newFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    newFileHeader.bfSize = newFileHeader.bfOffBits + newInfoHeader.biSizeImage;

    // Создание серой палитры
    RGBQUAD palette[256];
    for (int i = 0; i < 256; i++) {
        palette[i].rgbBlue = i;
        palette[i].rgbGreen = i;
        palette[i].rgbRed = i;
        palette[i].rgbReserved = 0;
    }

    // Запись новых заголовков и палитры
    WriteFile(hOutputFile, &newFileHeader, sizeof(newFileHeader), &bytesRead, NULL);
    WriteFile(hOutputFile, &newInfoHeader, sizeof(newInfoHeader), &bytesRead, NULL);
    WriteFile(hOutputFile, palette, sizeof(palette), &bytesRead, NULL);

    // Выделение памяти для буферов
    BYTE* inputRow = new BYTE[inputRowSize];
    BYTE* outputRow = new BYTE[outputRowSize]{ 0 };

    // Переход к данным изображения
    SetFilePointer(hInputFile, bmpFileHeader.bfOffBits, NULL, FILE_BEGIN);

    // Обработка строк
    for (int y = 0; y < height; y++) {
        ReadFile(hInputFile, inputRow, inputRowSize, &bytesRead, NULL);

        // Преобразование в градации серого
        for (int x = 0; x < width; x++) {
            BYTE b = inputRow[x * 3];
            BYTE g = inputRow[x * 3 + 1];
            BYTE r = inputRow[x * 3 + 2];

            // Формула преобразования RGB в яркость
            outputRow[x] = static_cast<BYTE>(0.299f * r + 0.587f * g + 0.114f * b);
        }

        // Запись строки с выравниванием
        WriteFile(hOutputFile, outputRow, outputRowSize, &bytesRead, NULL);
    }

    // Очистка
    delete[] inputRow;
    delete[] outputRow;
    CloseHandle(hInputFile);
    CloseHandle(hOutputFile);

    cout << "Conversion completed successfully!" << endl;
    system("pause");
    return 0;
}