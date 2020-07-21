unsigned short *graphics_memory = (unsigned short *)(0x0 + 0xA0000);

void putpixel()
{
    graphics_memory[0] = 0xff;
    graphics_memory[1] = 0xff;
    graphics_memory[2] = 0xff;
}
