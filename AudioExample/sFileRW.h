/*
 * sFileRW.h
 *
 *  Created on: Feb 6, 2017
 *      Author: Ty
 */

#ifndef SFILERW_H_
#define SFILERW_H_

extern const unsigned char SineWave[16];
extern unsigned char gaucCmpBuf[2048];
extern long WriteFileToDevice(unsigned long *ulToken, long *lFileHandle);
extern long ReadFileFromDevice(unsigned long ulToken, long lFileHandle);



#endif /* SFILERW_H_ */
