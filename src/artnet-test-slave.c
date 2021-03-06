/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "artnet-test-defaults.h"

#include <artnet/artnet.h>

int verbose = 0 ;
int nodes_found = 0;


int firmware_handler(artnet_node n, int ubea, uint16_t *data, int length, void *d) {

  	FILE *file=fopen("test", "wb");
	printf("in firmware hanlder got %d words\n", length) ;
	  if(file==NULL)
    	printf("could not open savefile\n");
	  else {
	      if(fwrite(data, sizeof(uint16_t), length, file) != length)
				printf("could not write complete savefile\n");
      	if(fclose(file) < 0)
			printf("could not close savefile");
	    }

	  return 0;
}

int main(int argc, char *argv[]) {
	artnet_node node ;
	char *ip_addr = NULL ;
	int i, optc, sd[2],  timeout = 2;
	time_t start ;
	
	// parse options 
	while ((optc = getopt (argc, argv, "va:t:")) != EOF) {
		switch (optc) {
 			case 'a':
				ip_addr = (char *) strdup(optarg) ;
		        break;
			case 't':
				timeout = atoi(optarg) ;
		        break;
			case 'v':
				verbose = 1 ;
				break; 
      		default:
				break;
    	}
	}

    node = artnet_new(ip_addr, verbose) ; ;

	// set default params
	artnet_set_short_name(node, SHORT_NAME) ;
	artnet_set_long_name(node, LONG_NAME) ;
	artnet_set_node_type(node, ARTNET_NODE) ;
	artnet_set_subnet_addr(node, DEFAULT_SUBNET ) ;
	
	for(i=0; i< ARTNET_MAX_PORTS; i++) 
		artnet_set_port_type(node, i,  ARTNET_ENABLE_OUTPUT & ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX  ) ;

	// set default port addresses
	artnet_set_port_addr(node, 0, ARTNET_INPUT_PORT, DEFAULT_PORT_IN_1 ) ;
	artnet_set_port_addr(node, 1, ARTNET_INPUT_PORT, DEFAULT_PORT_IN_2 ) ;
	artnet_set_port_addr(node, 2, ARTNET_INPUT_PORT, DEFAULT_PORT_IN_3 ) ;
	artnet_set_port_addr(node, 3, ARTNET_INPUT_PORT, DEFAULT_PORT_IN_4 ) ;
	
	artnet_set_port_addr(node, 0, ARTNET_OUTPUT_PORT, DEFAULT_PORT_OUT_1 ) ;
	artnet_set_port_addr(node, 1, ARTNET_OUTPUT_PORT, DEFAULT_PORT_OUT_2 ) ;
	artnet_set_port_addr(node, 2, ARTNET_OUTPUT_PORT, DEFAULT_PORT_OUT_3 ) ;
	artnet_set_port_addr(node, 3, ARTNET_OUTPUT_PORT, DEFAULT_PORT_OUT_4 ) ;


	
	// set poll reply handler
	artnet_set_firmware_handler(node, firmware_handler, NULL) ;

	artnet_start(node) ;

	sd[0] = artnet_get_sd(node,0) ;
	sd[1] = artnet_get_sd(node,1) ;

	start = time(NULL) ;

	// wait for timeout seconds before quitting
	// got an issue here if the upload starts towards the end of the time
	// we'll exit while the upload is in progress
	while(time(NULL) - start < timeout) {
		artnet_read(node,1) ;
	}

	return 0 ;	
}
