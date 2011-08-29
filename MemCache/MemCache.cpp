/*
 * MemCache.cpp
 *
 *  Created on: 07.07.2011
 *      Author: ajazdzewski
 */

#include <vector>
#include <string>
#include <algorithm>

#include <string.h>

#include <libmemcached/memcached.hpp>

using namespace std;

class MemCache {
public:

	static const uint32_t num_of_clients = 10;

	static MemCache &singleton() {
		static MemCache instance;
		return instance;
	}

	void set(const string &key, const vector<char> &value) {
		time_t expiry = 0;
		uint32_t flags = 0;
		getCache()->set(key, value, expiry, flags);
	}

	vector<char> get(const string &key) {
		vector<char> ret_value;
		getCache()->get(key, ret_value);
		return ret_value;
	}

	void remove(const string &key) {
		getCache()->remove(key);
	}

	memcache::Memcache *getCache() {
		/*
		 * pick a random element from the vector of clients.
		 * Obviously, this is not very random but suffices as
		 * an example!
		 */
		uint32_t index = rand() % num_of_clients;
		return clients[index];
	}

private:

	/*
	 * A vector of clients.
	 */
	vector<memcache::Memcache *> clients;

	MemCache() :
			clients() {
		/* create clients and add them to the vector */
		for (uint32_t i = 0; i < num_of_clients; i++) {
			memcache::Memcache *client = new memcache::Memcache(
					"127.0.0.1:11211");
			clients.push_back(client);
		}
	}

	~MemCache() {
		//for_each(clients.begin(), clients.end(), DeletePtrs());
		clients.clear();
	}

	MemCache(const MemCache&);
};

