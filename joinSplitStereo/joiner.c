/*
	joiner.c
	by John Allsup
	Copyright (C) 2011 John Allsup
	This file is in the Public Domain
*/
/*
	This serves as a simple illustration of usage of the C api for libsndfile
*/
/* compile with 
	gcc joiner.c -o joiner `pkg-config --cflags --libs sndfile`
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

int main(int argc, char **argv)
{
	char *leftFileName, *rightFileName, *outFileName;
	SNDFILE *leftSndFile, *rightSndFile, *outputSndFile;
	SF_INFO leftSndFileInfo, rightSndFileInfo, outputSndFileInfo;

	if( argc < 4 ) {
		fprintf(stderr, "Joiner: Joins two mono audio files to a stereo audio file.\n");
		fprintf(stderr, "Must have three arguments: LEFT RIGHT OUTPUT\n");
		exit(1);
	}

	leftFileName = argv[1];
	rightFileName = argv[2];
	outFileName = argv[3];

	leftSndFile = sf_open(leftFileName,SFM_READ,&leftSndFileInfo);
	if( leftSndFile == NULL ) {
		fprintf(stderr,"Failed to open left input file: %s (errno %d)\n",leftFileName,sf_error(NULL)); 
		exit(0);
	}
	
	rightSndFile = sf_open(rightFileName,SFM_READ,&rightSndFileInfo);
	if( rightSndFile == NULL ) {
		fprintf(stderr,"Failed to open right input file: %s (errno %d)\n",rightFileName,sf_error(NULL)); 
		exit(0);
	}

	if( memcmp(&leftSndFileInfo,&rightSndFileInfo,sizeof(SF_INFO)) != 0 ) {
		fprintf(stderr,"Files are not compatible -- must be same format and length.\n");
		exit(0);
	}
	if( leftSndFileInfo.channels != 1 ) {
		fprintf(stderr,"Input files must be mono.\n");
		exit(0);
	}
	memcpy(&outputSndFileInfo,&leftSndFileInfo,sizeof(SF_INFO));
	outputSndFileInfo.channels = 2;

	outputSndFile = sf_open(outFileName,SFM_WRITE,&outputSndFileInfo);
	if( outputSndFile == NULL ) {
		fprintf(stderr,"Failed to create output file.\n");
		exit(0);
	}
	
	fprintf(stdout,"%d frames to process.\n",(int)leftSndFileInfo.frames);	
	fprintf(stdout,"format %4x\n",leftSndFileInfo.format & 0xffff); 

	if ( (leftSndFileInfo.format & 0xff) > 0 && (leftSndFileInfo.format & 0xff) <= 4 ) {
		int i,ii,j;
		int leftBuffer[1024],rightBuffer[1024];
		int outputBuffer[2048];

		for( i=leftSndFileInfo.frames; i > 0; i -= 1024 ) {
			ii = (i < 1024) ? i : 1024;

			sf_read_int(leftSndFile,leftBuffer,ii);				
			sf_read_int(rightSndFile,rightBuffer,ii);
			
			for( j=0; j<ii; j++ )
			{
				outputBuffer[j*2] = leftBuffer[j];
				outputBuffer[j*2+1] = rightBuffer[j];
			}

			sf_write_int(outputSndFile,outputBuffer,ii*2); 
		}
	} else if ( leftSndFileInfo.format & 0xff == 6 ) {
		int i,ii,j;
		float leftBuffer[1024],rightBuffer[1024];
		float outputBuffer[2048];

		for( i=leftSndFileInfo.frames; i > 0; i -= 1024 ) {
			ii = (i < 1024) ? i : 1024;

			sf_read_float(leftSndFile,leftBuffer,ii);				
			sf_read_float(rightSndFile,rightBuffer,ii);
			
			for( j=0; j<ii; j++ )
			{
				outputBuffer[j*2] = leftBuffer[j];
				outputBuffer[j*2+1] = rightBuffer[j];
			}

			sf_write_float(outputSndFile,outputBuffer,ii*2); 
		}
	}

	sf_close(leftSndFile);
	sf_close(rightSndFile);
	sf_close(outputSndFile);
	
	return 0;
}
