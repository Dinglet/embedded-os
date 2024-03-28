#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return -1;
    }

    int fd = open("/dev/etx_device", O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }

    char const *strNumber = argv[1];
    const int length = strlen(strNumber);
    char const *strZero = "0";
    // wrtie a char to the device every second
    for (int i = 0; i < length; i++)
    {
        // write a number to the device
        printf("Writing %c to the device\n", strNumber[i]);
        if (write(fd, &strNumber[i], 1) < 0)
        {
            perror("Failed to write the message to the device...");
            return errno;
        }
        usleep(750000);

        // write a zero to the device
        // printf("Writing 0 to the device\n");
        if (write(fd, strZero, 1) < 0)
        {
            perror("Failed to write the message to the device...");
            return errno;
        }
        usleep(250000);
    }

    close(fd);
    
    return 0;
}
