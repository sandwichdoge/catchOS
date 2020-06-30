/* Segmentation - Address A:B
 * Physical address = (A * 0x10) + B
 * In protected mode A is not an absolute value for the segment. In protected mode A is a selector.
 * A selector represents an offset into a system table called the Global Descriptor Table (GDT).
 * The GDT contains a list of descriptors. Each of these descriptors contains information that describes
 * the characteristics of a segment.
 */

#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H
void kboot(void);
#endif
