#include "../bittool.h"
#include <stdlib.h>

void man_decode(FILE *in, FILE *out, const char * opt){
	int space = has_char(opt, 's');
	int bitcount = 0;

	uint8_t init = 0;
	uint8_t last = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE >> 1); // 1/2 the size

	size_t out_pos = 0;
	size_t ret = 0;
	
	int bitpos = 0;

  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		for(size_t idx = 0; idx < ret; ++idx){
			const uint8_t byte = in_data[idx];
			if (byte == '0' || byte == '1'){
				uint8_t src = (byte == '1');
				if (!init) {
					init = 1;
					last = src;
					continue;
				}

				init = 0;
				if (src ^ last) {
					if (src)
						out_data[out_pos++] = '0';
					else
						out_data[out_pos++] = '1';
				}
			}
		}
		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
	}
	fflush(out);
	free(out_data);
	free(in_data);
}

void man_encode(FILE *in, FILE *out, const char * opt){
	int space = has_char(opt, 's');

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE << 1); // twice the size

	size_t out_pos = 0;
	size_t ret = 0;
  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		for(size_t idx = 0; idx < ret; ++idx){
			const uint8_t byte = in_data[idx];
			switch(byte){
			case '0':
				out_data[out_pos++] = '0';
				out_data[out_pos++] = '1';
				break;
			case '1':
				out_data[out_pos++] = '1';
				out_data[out_pos++] = '0';
				break;
			default:
				out_data[out_pos++] = byte;
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
	ENGINE_ENTRY("manchester", 'e', man_encode, "Convert to/from manchester."),
	ENGINE_ENTRY("manchester", 'd', man_decode, "Convert to/from manchester."),
);
