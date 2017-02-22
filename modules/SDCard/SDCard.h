//*****************************************************************************
// SDCard.h
//
//*****************************************************************************

#ifndef SDCARD_H_
#define SDCARD_H_

extern static void sdCardInit();
extern static void setFile(char file[]);
extern static void setDirectoryPath(char directoryPath[]);
extern static void openDirectory();
extern static void ListDirectory();
extern static void mountFile();
extern static void openFile();
extern static void removeWavHeader();
extern static unsigned char readFile();
extern static void closeFile();
extern static void createFile(char fileName[], char fileText);
extern static UINT getSize();

#endif /* SDCARD_H_ */
