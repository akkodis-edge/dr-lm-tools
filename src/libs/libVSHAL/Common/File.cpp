#include "File.h"
#include <HALException.h>

//#define DEBUG_OUTPUT
#ifdef DEBUG_OUTPUT
#include <iostream>
#endif

using namespace LM_VSHAL;
using namespace std;

pthread_mutex_t global_lcd_mutex = PTHREAD_MUTEX_INITIALIZER;

File::File(std::string a_Prefix) :
    m_Prefix(a_Prefix + "/"), m_Lock(global_lcd_mutex)
{
    if(!File::exists(a_Prefix))
    {
#ifdef DEBUG_OUTPUT
        std::cout << "File: " << path << "does not exists\n";
#endif
        throw HALException( "File: Path does not exist: " + a_Prefix );
    }
}

FILE * File::open(std::string a_Function, bool a_ReadOnly)
{
    string path = m_Prefix + a_Function;

    FILE *file = fopen(path.c_str(), a_ReadOnly ? "r" :"w");
    if( file == NULL )
    {
#ifdef DEBUG_OUTPUT
        std::cout << "File: " << path << "does not exists\n";
#endif
        throw HALException(path + " does not exist");
    }

    return file;
}

int File::get(std::string a_Function)
{
    FILE *file = open(a_Function);
    int data;

    m_Lock.lock();
    fscanf( file, "%d\n", &data );
#ifdef DEBUG_OUTPUT
    std::cout << "From file: " << a_Function << " - Read: " << data << "\n";
#endif
    m_Lock.unlock();

    fclose(file);

    return data;
}

std::string File::getString(std::string a_Function)
{
    FILE *file = open(a_Function);
    char text[512];

    m_Lock.lock();
    fscanf(file, "%511[^\n]\n", text);
#ifdef DEBUG_OUTPUT
    std::cout << "From file: " << a_Function << " - Read: " << text << "\n";
#endif
    m_Lock.unlock();
    fclose(file);
    return std::string(text);
}

void File::set(std::string a_Function, int a_Value)
{
    FILE *file = open(a_Function, false);

    m_Lock.lock();
    fprintf( file, "%d\n", a_Value );
#ifdef DEBUG_OUTPUT
    std::cout << "To file: " << a_Function << " - Set: " << a_Value << "\n";
#endif
    m_Lock.unlock();

    fclose(file);
}

void File::setString(std::string a_Function, std::string a_Value)
{
    FILE *file = open(a_Function, false);

    m_Lock.lock();
    fprintf( file, "%s\n", a_Value.c_str() );
#ifdef DEBUG_OUTPUT
    std::cout << "To file: " << a_Function << " - Set: " << a_Value << "\n";
#endif
    m_Lock.unlock();

    fclose(file);
}

bool File::exists(std::string a_Path)
{
    struct stat stFileInfo;
#ifdef DEBUG_OUTPUT
    std::cout << "File exists: " << a_Path << " - Set: " << (stat(a_Path.c_str(), &stFileInfo) == 0 ? "true" : "false") << "\n";
#endif
    return stat(a_Path.c_str(), &stFileInfo) == 0;
}
