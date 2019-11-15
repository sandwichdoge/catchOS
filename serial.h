#define SERIAL_COM1_BASE 0x3f8

void serial_configure(unsigned short com, unsigned short divisor);
void serial_configure_baud_rate(unsigned short com, unsigned short divisor);
void serial_configure_line(unsigned short com);
void serial_configure_buffer(unsigned short com);
void serial_configure_modem(unsigned short com);
void serial_write(unsigned short com, char *data, unsigned int len);
