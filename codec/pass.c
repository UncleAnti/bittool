#include "../bittool.h"
#include <stdlib.h>

void pass(FILE * in, FILE * out, const char * opt){
	int space = has_char(opt, 's');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE);

	size_t out_pos = 0;
	size_t ret = 0;
  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		for(size_t idx = 0; idx < ret; ++idx){
			const uint8_t byte = in_data[idx];

			if(byte == '1' || byte == '0'){
				out_data[out_pos++] = byte;
				bitcount++;
			} else
				continue;

			if (space && bitcount == 8) {
				bitcount = 0;
				out_data[out_pos++] = ' ';
			}
		}

		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
  }

	free(out_data);
	free(in_data);

	fflush(out);
}

ENGINE_EXPORT(
	ENGINE_ENTRY("pass",'e',pass,"pass through"),
	ENGINE_ENTRY("pass",'d',pass,"pass through")
);
