#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

void printError() {
	cerr << "Options are:" << endl << "\t-client (realserver)" << endl
			<< "\t-oid (snmp oid ask on realserver)" << endl
			<< "\t[-cache] (memcached server name/ip default 127.0.0.1)" << endl
			<< "\t[-port] (memcached port default 11211)" << endl
			<< "\t[-ttl] (timetolive of the result default 10s)" << endl;
}

int main(int argc, const char* argv[]) {
	string client;
	string oid;
	string cache;
	int port;
	int ttl;
	int i;

	/*
	 * create an empty vector of strings
	 */
	vector<string> args;

	/*
	 * copy program arguments into vector
	 */
	for (i = 1; i < argc; i++)
		args.push_back(argv[i]);

	/**
	 * -client the client that is asked
	 * -oid the snmp oid that is called
	 * -cache memcache server (default 127.0.0.1)
	 * -port memcacheserver port (default 11211)
	 * -ttl cache time of the snmp result in the memcached (default 10s)
	 *
	 */
	if (args.size() % 2 != 0 || argc == 1) {
		printError();
		exit(EXIT_FAILURE);
	}

	/*
	 * get parameters and validate
	 */
	for (i = 0; i < args.size(); i++) {
		if (!(args[i]).compare("-client")) {
			client.assign(args[++i]);
			cout << "client: " << client << endl;
		} else if (!(args[i]).compare("-oid")) {
			oid.assign(args[++i]);
			cout << "oid: " << oid << endl;
		} else if (!(args[i]).compare("-cache")) {
			cache.assign(args[++i]);
			cout << "cache: " << cache << endl;
		} else if (!(args[i]).compare("-port")) {
			port = atoi(args[++i].c_str());
			cout << "port: " << port << endl;
		} else if (!(args[i]).compare("-ttl")) {
			ttl = atoi(args[++i].c_str());
			cout << "ttl: " << ttl << endl;
		} else {
			cerr << "option \"" << args[i] << "\" are unknown" << endl;
			printError();
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}
