#include <Windows.h>
#include <tchar.h>
#include <iostream>
using namespace std;

// Имя сервера именованного канала
LPCTSTR NAME_PIPE = _T("\\\\.\\pipe\\MyPipeName");

DWORD WINAPI Thread(LPVOID);

int _tmain(int argc, _TCHAR* argv[])
{
    _wsetlocale(LC_ALL, L".866");
    while (true)
    {

        // Создание нового экземпляра сервера именованного канала
        HANDLE hPipe = CreateNamedPipe(NAME_PIPE,                       // Имя сервера именованного канала
            PIPE_ACCESS_DUPLEX,             // Чтение и запись
            PIPE_TYPE_MESSAGE | PIPE_WAIT,  // Общение сообщениями в синхронном режиме
            PIPE_UNLIMITED_INSTANCES,       // Количество клиентов, которые могут подключиться к создаваемому экземпляру именованного канала
            NULL,                           // размер выходного буфера в байтах 
            NULL,                           // размер входного буфера в байтах 
            NULL,                           // время ожидания в миллисекундах 
            NULL);                           // Атрибуты безопасноти именованного канала
        
        // Проверка на создание канала
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            wcout << L"Ошибка CreateNamedPipe = " << GetLastError() << endl;
            wcin.get();
            break;
        }

        // Ожидание подключения клиента
        if (ConnectNamedPipe(hPipe,     // Дескриптор именованного канала 
            NULL))  // Синхронно
        {
            // Запуск отдельного потока, для обслуживания подключенного клиента
            HANDLE hThread = CreateThread(NULL,     // Атрибут безопасности
                NULL,       // Размер стека по умолчанию
                Thread,     // Указатель на функцию потока
                hPipe,      // Дескриптор именованного канала с подключенным клиентом
                NULL,       // Запуск сразу
                NULL);      // ID не требуется
           
                            // Проверка на создание потока
            if (!hThread)
            {
                wcout << L"Ошибка CreateThread = " << GetLastError() << endl;
                wcin.get();
                break;
            }

            // Закрытие дескриптора потока
            CloseHandle(hThread);

        }
        else
            wcout << L"Error ConnectNamedPipe = " << GetLastError() << endl;
    }

    wcin.get();
    return 0;
}

// Функция - поток для обслуживания клиентов
DWORD WINAPI Thread(LPVOID param)
{
    // Дескриптор клиента
    HANDLE hPipe = (HANDLE)param;

    // Вечный цикл
    while (true)
    {
        // Прием данных от клиента
        DWORD read = NULL;
        TCHAR buf[MAX_PATH] = { 0 };
        if (!ReadFile(hPipe,            // Дескриптор именованного канала
            buf,            // Указатель на буфер
            MAX_PATH - 1,   // Размер буфера в символах Unicode
            &read,          // Сколько байт полученного
            NULL))
        {
            wcout << L"Ошибка ReadFile = " << GetLastError() << endl;
            break;
        }

        wcout << L"Сообщение клиента: ";
        buf[read / sizeof(TCHAR)] = '\0';
        wcout << buf << endl;

        // Передача ответа клиенту
        wcout << L"Ваш ответ: ";
        wcin.getline(buf, MAX_PATH);
        DWORD write = NULL;
        if (!WriteFile(hPipe,                               // Дескриптор именованного канала
            buf,                                // Указатель на буфер передаваемых данных
            wcslen(buf) * sizeof(TCHAR),    // Размер данных в байтах
            &write,                             // Сколько байт передано
            NULL))
        {
            wcout << L"Ошибка WriteFile = " << GetLastError() << endl;
            break;
        }
    }

    // Закрытие дескриптора именованного канала
    CloseHandle(hPipe);

    return 0;

}