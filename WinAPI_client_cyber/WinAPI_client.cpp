// Client.cpp: определяет точку входа для консольного приложения.
//

#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
    _wsetlocale(LC_ALL, L".866");

    wcout << L"Введите имя сервера:";
    TCHAR name_server[MAX_PATH] = { 0 };
    wcin.getline(name_server, MAX_PATH);

    // Формирование пути к серверу
    wstring path = wstring(L"\\") + wstring(name_server) + wstring(L"\\pipe\\MyPipeName");

        // Открытие сервера именованного канала
        //HANDLE hFile = CreateFile(path.c_str(),                 // Путь к серверу
        //    GENERIC_READ | GENERIC_WRITE,   // Режим открытия
        //    NULL,                           // Монопольный доступ
        //    NULL,                           // Защита по умочланию
        //    OPEN_EXISTING,                  // Открытие существующего канала
        //    NULL,                           // Запись-чтение синхронные
        //    NULL);                          // Не требуется

        HANDLE hFile = CreateNamedPipe(path.c_str(),                       // Имя сервера именованного канала
            PIPE_ACCESS_DUPLEX,             // Чтение и запись
            PIPE_TYPE_MESSAGE | PIPE_WAIT,  // Общение сообщениями в синхронном режиме
            PIPE_UNLIMITED_INSTANCES,       // Количество клиентов, которые могут подключиться к создаваемому экземпляру именованного канала
            NULL,                           // размер выходного буфера в байтах 
            NULL,                           // размер входного буфера в байтах 
            NULL,                           // время ожидания в миллисекундах 
            NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        wcout << L"Ошибка CreateFile = " << GetLastError() << endl;
        wcin.get();
        return 0;
    }


    wcout << L"Для завершение необходимо ввести команду exit" << endl;

    // До тех пор, пока пользователь не введет exit
    while (true)
    {

        wcout << L"Введите сообщение: ";
        TCHAR buf[MAX_PATH] = { 0 };
        wcin.getline(buf, MAX_PATH);

        // Если пользователь ввел exit
        if (!wcscmp(buf, L"exit")) // сравнение введеной строки с exit
            break;

        // Передача на сервер сообщения
        DWORD write = NULL;
        if (!WriteFile(hFile,                               // Дескриптор именованного канала
            buf,                                // Указатель на буфер
            wcslen(buf) * sizeof(TCHAR),    // Размер буфера в байтах
            &write,                             // Сколько фактически передано байт
            NULL))                          // Синхронная запись
        {
            wcout << L"Ошибка WriteFile = " << GetLastError() << endl;
            wcin.get();
            break;
        }

        // Прием данных от сервера
        DWORD read = NULL;
        memset(buf, 0, sizeof(buf));
        if (!ReadFile(hFile,                // Дескриптор файла
            buf,                // Буфер для данных
            MAX_PATH - 1,       // Размер буфера - количество символов Unicode
            &read,              // Фактический размер принятых данных
            NULL))
        {
            wcout << L"Ошибка ReadFile = " << GetLastError() << endl;
            wcin.get();
            break;
        }

        // Вывод на консоль полученных данных
        wcout << L"Сообщение сервера: ";
        buf[read] = '\0';
        wcout << buf << endl;

    }

    // Закрытие дескрипторов именованного канала
    CloseHandle(hFile);

    wcin.get();

    return 0;
}