#include "../bittool.h"
#include <stdlib.h>

static void doubler(FILE * in, FILE * out, const char * opt){
	int space  = has_char(opt, 's');
  int justone  = has_char(opt, '1');
  int justzero = has_char(opt, '0');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE + (BIT_BUF_SIZE / 8) * 2); // allow for spaces every 8..
	size_t ret = 0;

  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		size_t out_pos = 0;
		for(size_t idx = 0; idx < ret; ++idx){
			switch(in_data[idx]) {
			case '1':
				if(!justzero){
		  		out_data[out_pos++] = '1';
	  			out_data[out_pos++] = '1';
  				bitcount++;
  				bitcount++;
				} else {
	  			out_data[out_pos++] = '1';
  				bitcount++;
				}
				break;
			case '0':
			  if(!justone){
  				out_data[out_pos++] = '0';
	  			out_data[out_pos++] = '0';
		  		bitcount++;
		  		bitcount++;
		  	} else {
		  		out_data[out_pos++] = '0';
		  		bitcount++;
				}
				break;
			//default:
				//out_data[out_pos++] = in_data[idx];
			}

			if(space && bitcount == 8){
				bitcount = 0;
				out_data[out_pos++] = ' ';
			}
		}
		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
  }
	fflush(out);

  free(out_data);
  free(in_data);
}

static void halver(FILE * in, FILE * out, const char * opt){
	int space  = has_char(opt, 's');
  int justone  = has_char(opt, '1');
  int justzero = has_char(opt, '0');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE / 2 + ((BIT_BUF_SIZE / 2) / 8)); // allow for spaces every 8..
	size_t ret = 0;
	char last = 0;
  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		size_t out_pos = 0;
		for(size_t idx = 0; idx < ret; ++idx){
			switch(in_data[idx]) {
			case '1':
				if(!justzero){
					if(last == '1'){
					 	last = 0;
						out_data[out_pos++] = '1';
						bitcount++;
					} else
						last = '1';
				} else {
				 	last = '1';
					out_data[out_pos++] = '1';
					bitcount++;
				}
				break;
			case '0':
				if(!justone){
					if(last == '0'){
					 	last = 0;
						out_data[out_pos++] = '0';
						bitcount++;
					} else
						last = '0';
				} else {
				 	last = '0';
					out_data[out_pos++] = '0';
					bitcount++;
				}
				break;
			//default:
				//out_data[out_pos++] = in_data[idx];
			}

			if(space && bitcount == 8){
				bitcount = 0;
				out_data[out_pos++] = ' ';
			}
		}
		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
  }

	fflush(out);

  free(out_data);
  free(in_data);
}

ENGINE_EXPORT(
	ENGINE_ENTRY("double", 'e', doubler, "double each bit"),
	ENGINE_ENTRY("halve" , 'd', halver, "half each bit")
);
