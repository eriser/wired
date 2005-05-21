#ifndef _WIREDSAMPLERATE_H_
#define _WIREDSAMPLERATE_H_

#include <stdio.h>
#include <sndfile.h>
#include <samplerate.h>

using namespace std;

typedef struct s_samplerate_info
{
	string		WorkingDirectory;
	int			SampleRate;
	int			Format;
} t_samplerate_info;

class WiredSampleRate
{
public:
	WiredSampleRate(){}
	~WiredSampleRate(){}
	WiredSampleRate(const WiredSampleRate& copy);
	WiredSampleRate		operator=(const WiredSampleRate& right);

	void		Init(t_samplerate_info *Info);
	bool		OpenFile(string& Path);					//return false if not modified (or invalid), else return true 
														// and set Path to the new FilePath
private:
	t_samplerate_info	_ApplicationSettings
};

#endif //_WIREDSAMPLERATE_H_
