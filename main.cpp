#include <iostream>
#include <fstream>
#include <string>
#include <sstream>	//For 'stringstream' used in conversions int <---> string
#include <cmath>	//For 'round()'

using namespace std;

void displayHowTo(){
	cout <<endl <<"How to use it:"
		<<endl <<"\t$./sub_shifter <option> -f sub_file.srt"
		<<endl <<endl <<"The option to use:"
		<<endl <<"\t-s\tFor positive or negative shift. The number of seconds to add or substract."
		<<endl <<"(Must be a decimal value)." <<endl <<"e.g.:"
		<<endl <<"\tTo forward:\n\t\t$./sub_shifter -s 10 -f sub_shifter.srt"
		<<endl <<"\tTo delay:\n\t\t$./sub_shifter -s -3 -f sub_shifter.srt"
		<<endl <<endl;
}

string int2str(const int n){
	stringstream ss;
	ss <<n;

	string str("");
	ss >>str;

	return str;	
}

int str2int(string const& str){
	stringstream ss;
	ss <<str;

	int n = 0;
	ss >>n;

	return n;
}

unsigned formattedTime2sec(string const& formattedTime){
	//Format must be something like HH:MM:SS

	unsigned short HH, MM, SS;

	HH = str2int(formattedTime.substr(0, 2));
	MM = str2int(formattedTime.substr(3, 2));
	SS = str2int(formattedTime.substr(6, 2));

	return (HH*3600 + MM*60 + SS);
}

string sec2formattedTime(const unsigned sec){
	double floatSec = sec;
	unsigned HH = 0;
	unsigned short MM = 0, SS = 0;

	floatSec /= 3600;
	HH = floatSec;
	floatSec = (floatSec - HH) * 60;
	MM = floatSec;
	floatSec = (floatSec - MM) * 60;
	SS = round(floatSec);

	string zeroPadding_HH = (HH < 10)? "0" : "";
	string zeroPadding_MM = (MM < 10)? "0" : "";
	string zeroPadding_SS = (SS < 10)? "0" : "";

	return string(zeroPadding_HH + int2str(HH) + ":"
		+ zeroPadding_MM + int2str(MM) + ":"
		+ zeroPadding_SS + int2str(SS));
}


int main(int argc, char* argv[]){

	string format = "HH:MM:SS";	//The true format is HH:MM:SS,MILLISEC
	string separation = " --> ";
	string shiftedFilename = "shifted_subtitles.srt";


	if(argc != 5){
		displayHowTo();
		return -1;
	}

	//Getting subtitles filename and shift duration
	unsigned fileIndex, shiftIndex;
	for(unsigned i = 1 ; i < argc-1 ; ++i){
		if(string(argv[i]) == "-f"){
			if(i+1 < argc){
				fileIndex = i + 1;
			}
			else{
				cout <<"Error: Cannot find subtitles file." <<endl;
				return -1;
			}
	
		}
		else if(string(argv[i]) == "-s"){
			if(i+1 < argc){
				shiftIndex = i + 1;
			}
			else{
				cout <<"Error: No shift duration found." <<endl;
				return -1;
			}
		}
	}
	if(fileIndex == 0 || shiftIndex == 0){	//Nothing found
		displayHowTo();
		return -1;
	}

	//The shift duration
	int shift = str2int(argv[shiftIndex]);

	//The subtitles file
	ifstream subFile(argv[fileIndex]);
	if(! subFile.is_open()){
		cout <<"Error: Cannot open the subtitles file." <<endl;
		return -1;
	}

	//The output file with shifted subtitles
	ofstream shiftedFile(shiftedFilename.c_str());
	if(! shiftedFile.is_open()){
		cout <<"Error: Cannot open destination file for shifted subtitles." <<endl;
		return -1;
	}

	//Reading line by line the .srt file, operating the shift and writing to 'shiftedFile'
	string line;
	unsigned subCount = 0;
	while(getline(subFile, line)){
		size_t found = line.find(separation);	
		if(found != string::npos){	//The line containing the timings is found
			subCount++;

			//Getting the formatted times
			string strStart = line.substr(0 , format.size());
			string strEnd = line.substr(found + separation.size() , format.size());

			//Operating the shift
			unsigned start = ((int) formattedTime2sec(strStart) + shift > 0) ? (formattedTime2sec(strStart) + shift) : 0;
			unsigned end = ((int) formattedTime2sec(strEnd) + shift > 0) ? (formattedTime2sec(strEnd) + shift) : 0;

			line.replace(0, format.size(), sec2formattedTime(start));
			line.replace(found + separation.size() , format.size() , sec2formattedTime(end));

		}

		//Writing to new file
		//shiftedFile <<line;
		shiftedFile <<line <<endl;
	}	
	
	cout <<endl <<endl <<"Shift operation done successfully with " <<subCount <<" subtitles shifted." <<endl;

	return 0;
}
