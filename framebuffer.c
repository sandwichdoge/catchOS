volatile char *fb = (volatile char*)0x000B8000;

void write_cell(unsigned int i, unsigned char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = ((fg & 0b00001111) << 4) | (bg & 0b00001111);
}
