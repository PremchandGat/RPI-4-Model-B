/*permission codes */
#define RDONLY 0x01
#define WRONLY 0X10
#define RDWR   0x11

struct device_data {
    int size;
    int perm;
    char *serial_name;
};