#include "../bittool.h"
#include <stdlib.h>

static void invert(FILE * in, FILE * out, const char * opt){
	int space  = has_char(opt, 's');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE + (BIT_BUF_SIZE / 8)); // allow for spaces every 8..
	size_t ret = 0;

  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		size_t out_pos = 0;
		for(size_t idx = 0; idx < ret; ++idx){
			switch(in_data[idx]) {
			case '1': 
				out_data[out_pos++] = '0';
				bitcount++;
				break;
			case '0':
				out_data[out_pos++] = '1';
				bitcount++;
				break;
			default:
				out_data[out_pos++] = in_data[idx];
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
}

ENGINE_EXPORT(
	ENGINE_ENTRY("invert", 'e', invert, "invert binary"),
	ENGINE_ENTRY("invert", 'd', invert, "invert binary")
);
