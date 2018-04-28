/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Pushfyi Definitions

*********************************************************
*/

#ifndef __PUSHFYI_DEFS__
#define __PUSHFYI_DEFS__

#define RUN_MODE_STANDALONE 	"standalone"
#define RUN_MODE_CLUSTER 		"cluster"

/*
 * Keys string in pushfyi event
 */
#define PUSHFYI_LICENSE_KEY		"license"
#define PUSHFY_APP_SECRET		"appsecret"


/*!
 * Client Keys Status Codes
 */
#define PUB_SUB_KEYS_NOT_FOUND 				0x100000
#define PUB_SUB_KEYS_ERROR					0x010000
#define PUB_SUB_KEYS_VALID_LIMIT_EXCEEDED	0x110000
#define PUB_SUB_KEYS_VALID					0x000000
#define PUB_SUB_KEYS_MGMT					0x000001

/*
 * Client's Publish Queue Max Size
 */
#define MAX_CLIENT_QUEUE_SIZE 		256


/**
 *
 * Pushfyi Module Topics
 */

#define PUSHFYI_MANAGER_TOPIC			"pushfyi-manager"
#define PUSHFYI_TCP_SERVER_TOPIC		"pushfyirs-tcp-server"
#define PUSHFYI_TLS_SERVER_TOPIC		"pushfyirs-securetcp-server"
#define PUSHFYI_CLIENT_FACTORY_TOPIC 	"pushfyirs-client-factory"
#define PUSHFYI_TLS_READER_TOPIC		"pushfyi-securetls-reader"

#define TCP_READER_ONLINE			"tcp-reader-online"

/**
 * TCP Reader Commands
 *
 */
#define TCP_READER_REPORT_STATUS			"status"
#define TCP_READER_NEW_PUSHFYI_CLIENT		"new-pushfyi-client"
#define TCP_READER_CLEAN_UP					"cleanup"

/**
 *
 * Pushfyi Module Commands
 */
#define PUSHFYI_MODULE_HEARTBEAT	"heartbeat"
#define PUSHFYI_MODULE_COMMAND		"command"

#define START_NEW_READER			"start-new-reader"
#define REPORT_STATUS				"report-status"
#define RESET_DIAGNOSTICS			"reset-diagnostics"

#define CLIENT_HELLO				"hello"
#define PROVISION_NEW_CLIENT		"provision-new-client"
#define PROVISION_NEW_CLIENT_APP	"provision-new-client-app"


#endif //__PUSHFYI_DEFS__
