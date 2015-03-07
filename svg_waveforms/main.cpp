//
//  main.cpp
//  svg_waveforms
//
//  Created by Piaras Hoban on 29/11/2014.
//  Copyright (c) 2014 Piaras Hoban. All rights reserved.
//

#include <iostream>

#include "essentia/essentia.h"
#include "essentia/essentiamath.h"
#include "essentia/algorithm.h"
#include "essentia/algorithmfactory.h"
#include "boost/format.hpp"


using namespace std;
using namespace essentia;
using namespace essentia::standard;
using boost::format;


std::vector<Real> getAudioSamples(string audiopath);
std::vector<Real> getPeaks(std::vector<Real> buffer, int length);
string soundcloudBars(std::vector<Real> peaks,int width, int height);
string classicWaveform(std::vector<Real> peaks,int width, int height);
string drawRect(int x, int y, int height,int rwidth);

int maxlength = 200;
int maxheight = 800;

int main(int argc, const char * argv[]) {

	

	
	std::vector<Real> samples = getAudioSamples(argv[1]);

	std::vector<Real> peaks = getPeaks(samples,maxlength);
	
	string svg_string = soundcloudBars(peaks,maxlength,maxheight);
	
	ofstream svg_output("waveform.svg");
	
	svg_output << svg_string;
	
	svg_output.close();
	
    return 0;
}

std::vector<Real> getAudioSamples(string audiopath)
{
	std::vector<Real> audioBuffer;

	essentia::init();
	
	//Algorithms
	AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
	Algorithm* audio = factory.create("MonoLoader","filename",audiopath);

	audio->output("audio").set(audioBuffer);

	audio->compute();

	delete audio;
	
	essentia::shutdown();
	
	return audioBuffer;
}

std::vector<Real> getPeaks(std::vector<Real> buffer, int length)
{
	int sampleSize = buffer.size() / length;
	int sampleStep = (sampleSize / 10) | 1;
	std::vector<Real> peaks(length);
	
	for (int i = 0; i < length; ++i) {
		int start = i * sampleSize;
		int end = start + sampleSize;
		Real max = 0;
		for (int j = start; j < end; j += sampleStep) {
			Real value = buffer[j];
			if (value > max) {
				max = value;
			} else if (-value > max) {
				max = -value;
			}
 		}
		peaks[i] = max;
	}
	return peaks;
}


//soundcloud style bars
string soundcloudBars(std::vector<Real> peaks,int width, int height)
{

	int rwidth = 2;

	string header = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	
	header += "\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
	
	header += (boost::format("\n<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\"\n\tviewBox = \"0 0 %s %s\">\n") % (rwidth * 1.25 * peaks.size()) % height).str();
	
	string path = "";
	
	for (int i = 0; i < peaks.size(); ++i) {
		path += drawRect((i + (rwidth * 1.25 * i)), height/2, (peaks[i]*height/2),rwidth);
	}
	
	string footer = "\n</svg>";
	
	string svg = header + path + footer;
	
	return svg;
}

string drawRect(int x, int y, int height,int rwidth)
{
	int x1,y1,x2,y2,x3,y3,x4,y4;
	
	x1 = x;
	y1 = y;
	
	x2 = x;
	y2 = y - height;
	
	x3 = x + rwidth;
	y3 = y2;
	
	x4 = x3;
	y4 = y1;
	
	string rect = (boost::format("<path d=\"M %s %s L %s %s L %s %s L %s %s\" stroke=\"black\"/>\n\t")
				   % x1 % y1 % x2 % y2 % x3 % y3 % x4 % y4).str();
	return rect;
}


//standard waveform

string classicWaveform(std::vector<Real> peaks,int width, int height)
{
	
	string header = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	
	header += "\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
	
	header += (boost::format("\n<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\"\n\tviewBox = \"0 0 %s %s\">\n") % width % height).str();
	
	string path = (boost::format("\n\t<path d=\"M 0 %s\n\t") % (height/2)).str();
	
	for (int i = 0; i < peaks.size(); ++i) {
			boost::format fmt = boost::format("L %s %s\n\t") % i % ((height/2) - (peaks[i] * height/2));
			path += fmt.str();
	}
	
	path += (boost::format("L %s %s\n\t") % (peaks.size() + 1) % (height/2)).str();
	
	path += (boost::format("M 0 %s\n\t") % (height/2)).str();
	
	for (int i = 0; i < peaks.size(); ++i) {
		boost::format fmt = boost::format("L %s %s\n\t") % i % ((height/2) + (peaks[i] * height/2));
		path += fmt.str();
	}

	path += (boost::format("L %s %s\n\t") % (peaks.size() + 1) % (height/2)).str();

	
	string footer = "\" stroke=\"black\" fill=\"black\"/>\n</svg>";

	string svg = header + path + footer;
	
	return svg;
}