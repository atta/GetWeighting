/**
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

/*
 * snmp++.cpp
 *
 *  Created on: 07.07.2011
 *      Author: ajazdzewski
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
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string.h>

#include <string>
#include <vector>

#include "snmp++.hpp"

using namespace std;

//class SNMPGet {
//public:
SNMPGet::SNMPGet(string ask_oid, string server,string community, string client) {
	SNMPGet::ask_oid = ask_oid;
	SNMPGet::server = server;
	SNMPGet::community = community;
	SNMPGet::client = client;
}

vector<string> SNMPGet::get() {
	return SNMPGet::get(SNMPGet::ask_oid, SNMPGet::server,SNMPGet::community, SNMPGet::client);
}

vector<string> SNMPGet::get(string ask_oid, string server,string community, string client) {
	netsnmp_session session, *ss;
	netsnmp_pdu *pdu;
	netsnmp_pdu *response;

	vector<string> result;

	oid anOID[MAX_OID_LEN];
	size_t anOID_len;

	netsnmp_variable_list *vars;
	int status;
	int count = 1;

	/*
	 * Initialize the SNMP library
	 */
	init_snmp(client.c_str());

	/*
	 * Initialize a "session" that defines who we're going to talk to
	 */
	snmp_sess_init(&session); /* set up defaults */
	session.peername = strdup(server.c_str());

	/* set up the authentication parameters for talking to the server */

	/* set the SNMP version number */
	session.version = SNMP_VERSION_1;

	/* set the SNMPv1 community name used for authentication */
	session.community = (u_char*) community.c_str();
	session.community_len = community.length();

	/*
	 * Open the session
	 */
	SOCK_STARTUP;
	ss = snmp_open(&session); /* establish the session */

	if (!ss) {
		snmp_sess_perror("ack", &session);
		SOCK_CLEANUP;
		exit(1);
	}

	/*
	 * Create the PDU for the data for our request.
	 *   1) We're going to GET the system.sysDescr.0 node.
	 */
	pdu = snmp_pdu_create(SNMP_MSG_GET);
	anOID_len = MAX_OID_LEN;
	// old OID .1.3.6.1.2.1.1.1.0
	if (!snmp_parse_oid(ask_oid.c_str(), anOID, &anOID_len)) {
		snmp_perror(ask_oid.c_str());
		SOCK_CLEANUP;
		exit(EXIT_FAILURE);
	}

	snmp_add_null_var(pdu, anOID, anOID_len);

	/*
	 * Send the Request out.
	 */
	status = snmp_synch_response(ss, pdu, &response);

	/*
	 * Process the response.
	 */
	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		/*
		 * SUCCESS: Print the result variables
		 */

		string temp;
		/* manipuate the information ourselves */
		for (vars = response->variables; vars; vars = vars->next_variable) {
			if (vars->type == ASN_OCTET_STR) {
				//char *sp = (char *) malloc(1 + vars->val_len);
				//memcpy(sp, vars->val.string, vars->val_len);
				//sp[vars->val_len] = '\0';
				//printf("value #%d is a string: %s\n", count++, sp);
				result.push_back(temp.assign((char *) vars->val.string));
				//free(sp);
			} else
				fprintf(stderr, "value #%d is NOT a string! Ack!\n", count++);
		}
	} else {
		/*
		 * FAILURE: print what went wrong!
		 */

		if (status == STAT_SUCCESS
		)
			fprintf(stderr, "Error in packet\nReason: %s\n",
					snmp_errstring(response->errstat));
		else if (status == STAT_TIMEOUT
		)
			fprintf(stderr, "Timeout: No response from %s.\n",
					session.peername);
		else
			snmp_sess_perror(client.c_str(), ss);

	}

	/*
	 * Clean up:
	 *  1) free the response.
	 *  2) close the session.
	 */
	if (response)
		snmp_free_pdu(response);
	snmp_close(ss);

	SOCK_CLEANUP;

	return result;
}
//};
