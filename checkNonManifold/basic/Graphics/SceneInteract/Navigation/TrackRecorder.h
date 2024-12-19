#ifndef VCGLIB_TRACKRECORDER
#define VCGLIB_TRACKRECORDER

#include <wrap/gui/trackball.h>
#include <stdio.h>
#include <time.h>

namespace sn3DCore
{
	struct TrackRecorder{

		TrackRecorder(){Stop();}
	
		FILE * trackfile;
	
		enum { PLAY,REC,OFF } mode;
		int nextTime,   
				startTime;  
 
		void StartPlaying(char * namefile)
		{
			if(trackfile != nullptr) return;

			trackfile = fopen(namefile,"rb");
			startTime = clock();
			mode = PLAY;
			fread(&nextTime,4,1,trackfile);
		} 

		void UpdateTrackball(Trackball & t)
		{
			while( ( clock()-startTime > nextTime)&& !feof(trackfile)){
					fread(&t.track,sizeof(float)*4 + sizeof(float)*5,1,trackfile);
					fread(&nextTime,4,1,trackfile);
			}
			if(feof(trackfile))
				Stop();
		}

		void  StartRecording(char * namefile)
		{
			if(trackfile != nullptr) return;
			trackfile = fopen(namefile,"wb");	
			startTime = clock();
			mode = REC;
		} 

		void  RecordTrackball(Trackball & t)
		{
			nextTime = clock()-startTime;
			fwrite(&nextTime,4,1,trackfile);
			fwrite(&t.track,sizeof(float)*4 + sizeof(float)*5,1,trackfile);
		}

		void  Stop(){mode = OFF; trackfile = nullptr;};
	};
}
#endif