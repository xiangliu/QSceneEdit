#ifndef BITMAP_H
#define BITMAP_H

/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/

//完全是写bitmap文件的函数

void WriteBitmap(unsigned char *Buff, int x, int y, char *filename);
void WriteBitmap8(unsigned char *sBuff, int x, int y, char *filename);
void WriteRAW(unsigned char *Buff, int x, int y, char *filename);

#endif