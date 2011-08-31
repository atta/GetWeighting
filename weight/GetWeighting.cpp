/*
 * GetWeighting.cpp
 *
 *  Created on: 31.08.2011
 *      Author: a.jazdzewski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "/data/a.jazdzewski/git/GetWeighting/snmp++/snmp++.hpp"
#include <libmemcached/memcached.hpp>

using namespace std;

void printError() {
	cerr << "Options are:" << endl << "\t-client (realserver)" << endl
			<< "\t-oid (snmp oid ask on realserver)" << endl
			<< "\t[-cache] (memcached server name/ip default 127.0.0.1)" << endl
			<< "\t[-port] (memcached port default 11211)" << endl
			<< "\t[-ttl] (timetolive of the result default 10s)" << endl;
}

string convertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

int getWeight(string client, SNMPGet snmp, memcache::Memcache cache) {
	int i;
	vector<char> data;
	vector<string> snmpData;

	bool maintenance;

	snmpData = snmp.get();
	for (i = 0; i < (int) snmpData.size(); i++) {
		cout << snmpData[i] << endl;
	}

	cache.get("Maintenance_"+client, data);
	if (data.size() == 0) {
		cout << "no Value for: Maintenance_" << client << endl;
		maintenance = false;
	} else
	for (i = 0; i < (int) data.size(); i++) {
		cout << data[i] << endl;
		if (atoi(data[i]) > 0)
			maintenance = true;
	}

	cache.get("weight_"+client, data);
	if (data.size() == 0) {
		cout << "no Value for: weight_" << client << endl;
	} else
	for (i = 0; i < (int) data.size(); i++) {
		cout << data[i] << endl;
	}

	return 0;
}


int main(int argc, const char* argv[]) {
	string client;
	string oid;
	string cache = "127.0.0.1";
	int port = 11211;
	int ttl = 10;
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
	for (i = 0; i < (int) args.size(); i++) {
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

	/*
	 * get data
	 */


	SNMPGet snmp(oid, client, "public", "GetWeighting");
	memcache::Memcache connect(cache, port);

	getWeight(client, snmp, connect);

	//connect.set("key", some_vector_of_chars, time_to_live, flags);
	//connect.get("key", some_vector_to_fill_with_data);
	connect.remove("key");
	exit(EXIT_SUCCESS);
}
