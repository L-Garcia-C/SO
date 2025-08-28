#include <unistd.h>

int main() {
    const char *msg = "Hello, World!\n";
    write(1, msg, 14);  // 1 é o descritor do stdout
    return 0;
}
