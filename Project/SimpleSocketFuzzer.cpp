#include "stdafx.h"

#include "cFuzzClass.h"
#include "Network.h"

#include "support_fcn.h"

#if (_MSC_VER < 1700)
#error You must use Visual Studio 2012 to compile this program
#endif

using namespace std;

const char* ErrorFile = "CU_ERR.txt";

namespace ScriptLabels {
	const string Verbose="[VERBOSE]";
	const string Protocol="[PROTOCOL]";
	const string Test="[TEST]";
	const string Mutable="[M]";
	const string Constant="[C]";
	const string Repeats="[R]";
	const string Pause="[P]";
	const string Delay="[D]";
};

void LogCOMMUError( const char* Error )
{
	FILE *_f=NULL;
	errno_t res= fopen_s( &_f, ErrorFile, "a+" );

	if( !res)
		return;

	fprintf( _f, Error );

	fclose( _f );
}

void print_Help() {
	cout << "SimpleSocketFuzzer v1.0\n"
			"by Shub-Nigurrath\n"
			"Syntax: {IP}:{port} {udp|tcp} {tests_script}"
			"\n"
			"  {udp|tcp} used to specify a protocol for fuzzing, but it can\n"
			"            be overridden by a script command (see instructions)\n"
			"Example:\n"
			"\t127.0.0.1:8080 udp script.txt -> uses udp protocol\n"
			"\t127.0.0.1:8080 script.txt -> uses tcp protocol\n";
}
 

int main(int argc, char* argv[]) {

	if( argc < 3 || argc > 4 ) {
		string message;
		message="Error: Invalid number of arguments\n" ;
		//LogCOMMUError( message.c_str() );
		cout << message << endl;
		print_Help();
		return 0;
	}

	std::vector<std::string> arguments;
	_tokenize(arguments, argv[1], ":");	

	string Address=arguments[0];
	int Port=0;
	istringstream ( arguments[1] ) >> Port;

	string script_filename;
	short socket_type=0; 
	_verbose_mode verbose_mode=Disabled;
	string str_socket_type;
	
	if(argc==3) {
		istringstream ( argv[2] ) >> script_filename;
		socket_type=SOCK_STREAM; //by default
		str_socket_type="TCP";
	}
	else {
		istringstream ( argv[3] ) >> script_filename;
		istringstream ( argv[2] ) >> str_socket_type;
		std::transform(str_socket_type.begin(), str_socket_type.end(), str_socket_type.begin(), ::toupper);
			
		if(str_socket_type=="UDP")
			socket_type=SOCK_DGRAM;
		else
			socket_type=SOCK_STREAM;
	}
	
	if(str_socket_type!="UDP" && str_socket_type!="TCP") {
		cout << "specified protocol is not supported, must be UDP|TCP" << endl;
		exit(1);
	}

	cout << "Fuzzing on " << str_socket_type <<"://" << Address << ":" << Port << ", Script_File: " << script_filename << endl;

	cFuzz c;

	Init();

	/////////////////////////////////////////////////////////////////////////
	//Read and execute a script
	//////////////////////////////////////////////////////////////////////////
	ifstream fs(script_filename);
		if(fs.bad()) {
			cout << "specified scriptfile is not existing" << endl;
			exit(EXIT_FAILURE);
		}

	fs >> noskipws; //just tell to not stop on white spaces with getline()
	int number_of_tests=0, current_test=0;
	size_t position=0;
	bool script_ready_to_go=false;
	bool pause_after_run=false; //command [P]
	long int delay_ms=0; //command [D]
	long current_script_line=0;
	string Delim; //the delimiter read from lines starting with [C] or [M]
	string Command; //the command issues in these lines after the delimiter
	int Repeats=0;

	while(!fs.fail()) {
		string line_token;
		getline(fs, line_token);
		current_script_line++;

		//skip comments after command, the comment start with a ; (also really trim spaces and tabs)
		size_t	comment=line_token.find_first_of(';');
		if(comment!=string::npos) 
			line_token.erase(comment);
		if(line_token.size()!=0)
			line_token=_trim_all(line_token); //trim anyway because spaces and tabs at the end of the command are a bad thing always
				
		position=line_token.find(ScriptLabels::Verbose); 
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Verbose.size());
			line_token=_trim(line_token, " "); //trim 
			std::transform(line_token.begin(), line_token.end(), line_token.begin(), ::toupper);
	
			if(line_token=="1H")
				verbose_mode=Hexadecimal; 
			else
				verbose_mode=Normal;
			cout << "Verbose mode: " << ((verbose_mode!=Disabled)?((verbose_mode==Hexadecimal)?"HEXADECIMAL":"ACTIVATED"):"DISABLED") << endl;
			continue;
		}

		position=line_token.find(ScriptLabels::Protocol); 
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Protocol.size());
			socket_type=atoi(line_token.c_str());
			
			switch(socket_type) {
				case SOCK_STREAM:
					str_socket_type="TCP";
				break;
				case SOCK_DGRAM:
					str_socket_type="UDP";
				break;
				default: {
					cout << "Error in the script, [PROTOCOL] not supported, line " << current_script_line << endl;
					exit(2);
				}
			}
			cout << "Socket opened as (commandline owerwritten) : " << str_socket_type << endl;
			continue;
		}

		position=line_token.find(ScriptLabels::Test); 
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Test.size());
			number_of_tests=atoi(line_token.c_str());
			cout << "Number of tests to perform: " << number_of_tests << endl;
			continue;
		}
		
		position=line_token.find(ScriptLabels::Constant);
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Constant.size());
			Command=_trim(line_token, " ");
			cout << "\tAdded Constant: " << Command << endl;
			c.AddConstant(Command);
			continue;
		}
		
		position=line_token.find(ScriptLabels::Mutable);
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Mutable.size());
			Delim=line_token[1];
			if(Delim=="]") { //no delimiter case.
				line_token.erase(0,2);
				//A special delimiter must be used with the Hex strings, because I use getline to separate tokens thus if the delim is part of the
				//hex string actually the real hex string is truncated before its real end.
				Delim=special_hex_delim; 
				line_token+=Delim;
			}
			else
				line_token.erase(0,3);
			Command=_trim(line_token, " ");
			if(Command.find(Delim)==string::npos) {
				cout << "Script error: must not use delimiter for single commands, line " << current_script_line << endl;
				exit(1);
			}

			//just strip the special_hex_delim from the output, just for clean out
			cout << "\tAdded Mutable: ";
			if(Delim==special_hex_delim) {
				string str=Command;
				str.erase(str.find(Delim));
				cout << str << endl;
			}
			else
				cout << Command << " (" << Delim <<")" << endl;

			c.AddMutable(Command,Delim);
			continue;
		}

		position=line_token.find(ScriptLabels::Repeats);
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Repeats.size());
			Repeats=atoi(line_token.c_str());
			c.Repeat( Repeats );
			cout << "\tAdded Repeats: " << Repeats << endl;
			script_ready_to_go=true;
			continue;
		}

		position=line_token.find(ScriptLabels::Pause);
		if(position!=string::npos) {
			pause_after_run=true;			
			continue;
		}

		position=line_token.find(ScriptLabels::Delay);
		if(position!=string::npos) {
			line_token.erase(position,ScriptLabels::Delay.size());
			delay_ms=atol(line_token.c_str());
			cout << "Delay Between retransmissions: " << delay_ms << " ms" << endl;
			continue;
		}
	
		if(script_ready_to_go) {
			cout << "\tExecuting attack #" << ++current_test << " of " << number_of_tests << endl;
			
			//Now executes this attack
			try {
				if( DoRound( c, Address, Port, socket_type, delay_ms, verbose_mode ) == TARGET_UP ) {
					printf("\tTarget still up!\n");
				} 
				else {
					printf("\tTarget down!\n");
				}
			}
			catch(char *ptr) {
				cout << "Fatal: " << ptr << endl;
			}
		
			c.Cleanup();
			script_ready_to_go=false;
			Delim=Command="";
			Repeats=0;
			position=0;
			cout << endl;			
		}

		if(pause_after_run) {
			cout << "\tPause After Run..\b" << endl;
			system("pause");
			pause_after_run=false;
		}
	}
		
	fs.close();

	Cleanup();

	cout << "All jobs (#" << number_of_tests << ") terminated" << endl;

#ifdef _DEBUG
	system("pause");
#endif // _DEBUG
	
	return 0;
}