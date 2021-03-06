//
//  xSpectrum.h
//  xSpectrum
//
//  Created by Rohan Jyoti on 04/20/13.
//  Copyright (c) 2013 Rohan Jyoti. All rights reserved.
//

#include "AUEffectBase.h"

#include "xSpectrumVersion.h"
#include "SpectralProcessor.h"
#include "xSpectrumSharedData.h"

#include "CARingBuffer.h"
#include "CABufferList.h"


#if AU_DEBUG_DISPATCHER
	#include "AUDebugDispatcher.h"
#endif


#ifndef __xSpectrum_h__
#define __xSpectrum_h__


#pragma mark ____xSpectrum Properties
enum
{
	kAudioUnitProperty_xSpectrumData = 65536,
	kAudioUnitProperty_SampleTimeStamp = 65537,
};


#pragma mark ____xSpectrum Parameters

static const float kDefaultValue_ParamOne = 0.5; //used for testing purposes
static const UInt64 kDefaultValue_BufferSize = kMaxNumAnalysisFrames * kMaxNumBins;

//static CFStringRef kParameterOneName = CFSTR("Parameter One"); //used for testing purposes

enum 
{
	kParam_One =0,
	kNumberOfParameters=1
}; //used for testing purposes


#pragma mark ____xSpectrum
class xSpectrumKernel : public AUKernelBase	// Kernel Extension: most real work happens here
{
	public:
		xSpectrumKernel(AUEffectBase *inAudioUnit ) : AUKernelBase(inAudioUnit) {}
	
		// *Required* overides for the process method for this effect
		// processes one channel of interleaved samples
		virtual void Process(	const Float32 	*inSourceP,
								Float32		 	*inDestP,
								UInt32 			inFramesToProcess,
								UInt32			inNumChannels,
								bool			&ioSilence);
	
		virtual void Reset();
};


class xSpectrum : public AUEffectBase
{
	public:
		xSpectrum(AudioUnit component); //default constructor
		virtual ~xSpectrum(); //virtual destructor 

	
		//The following are necessary CoreAudio/AudioUnit functions
		virtual AUKernelBase *NewKernel() { return new xSpectrumKernel(this); }
	
		virtual	OSStatus GetParameterValueStrings(AudioUnitScope		inScope,
												  AudioUnitParameterID	inParameterID,
												  CFArrayRef *			outStrings);
    
		virtual	OSStatus GetParameterInfo(AudioUnitScope			inScope,
										  AudioUnitParameterID		inParameterID,
										  AudioUnitParameterInfo	&outParameterInfo);
    
		virtual OSStatus GetPropertyInfo(AudioUnitPropertyID		inID,
										 AudioUnitScope				inScope,
										 AudioUnitElement			inElement,
										 UInt32 &					outDataSize,
										 Boolean &					outWritable );
	
		virtual OSStatus GetProperty(AudioUnitPropertyID			inID,
									 AudioUnitScope					inScope,
									 AudioUnitElement				inElement,
									 void *							outData);
	
		virtual Float64 GetTailTime(){return(0.0);}
		virtual	bool SupportsTail(){ return true;}
		virtual OSStatus Version() { return kxSpectrumVersion; }
	
		//Additional xSpectrum required functions
		virtual OSStatus init();
		void allocateBuffers();
		virtual	OSStatus changeStreamFormat(AudioUnitScope						inScope,
											AudioUnitElement					inElement,
											const CAStreamBasicDescription & 	inPrevFormat,
											const CAStreamBasicDescription &	inNewFormat);
		
		OSStatus Render(AudioUnitRenderActionFlags		&ioActionFlags,
						const AudioTimeStamp &			inTimeStamp,
						UInt32							inFramesToProcess );
	
		OSStatus getxSpectrumData(xSpectrumData* data);
	
	private:
		CARingBuffer* mSpectrumBuffer; //for spectral data
		CABufferList* mFetchingBufferList; //for fetching from ring
		CABufferList* mSpectralDataBufferList; //for fft computation
		AudioTimeStamp mRenderStamp;
	
		CAAutoDelete<SpectralProcessor> mSpectralProcessor;
		UInt32 mBlockSize;
		UInt32 mNumBins;
		Float32* mMinAmp;
		Float32* mMaxAmp;
};
#endif
