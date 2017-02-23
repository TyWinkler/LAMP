//*****************************************************************************
// SDCard.h
//
//*****************************************************************************

#ifndef SDCARD_H_
#define SDCARD_H_

extern void sdCardInit();
extern void setFile(char file[]);
extern void setDirectoryPath(char directoryPath[]);
extern void openDirectory();
extern void ListDirectory();
extern void mountFile();
extern void openFile();
extern void removeWavHeader();
extern unsigned char readFile();
extern void closeFile();
extern void createFile(char fileName[], char fileText);
extern UINT getSize();

#endif /* SDCARD_H_ */
