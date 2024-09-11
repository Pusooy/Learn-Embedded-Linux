#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd;
    char buffer;

    // 检查命令行参数数量
    if (argc != 3)
    {
        printf("Usage: %s <device_path> <1|0>\n", argv[0]);
        return -1;
    }

    // 获取设备路径和写入值
    char *device = argv[1];
    buffer = argv[2][0];

    // 打开设备
    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open device");
        return -1;
    }

    // 写入值到设备
    if (write(fd, &buffer, 1) < 0)
    {
        perror("Failed to write to device");
        close(fd);
        return -1;
    }

    // 从设备读取当前状态
    if (read(fd, &buffer, 1) < 0)
    {
        perror("Failed to read from device");
        close(fd);
        return -1;
    }

    // 打印当前LED状态
    printf("LED Status: %c\n", buffer);
    close(fd);
    return 0;
}
