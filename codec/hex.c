#include "../bittool.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static const char hex[] = "0123456789ABCDEF";

void hex_decode(FILE * in, FILE * out, const char * opt){
	int space  = has_char(opt, 's');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc((BIT_BUF_SIZE * 4) + ( (BIT_BUF_SIZE / 8) * space) );
	size_t ret = 0;

  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		size_t out_pos = 0;
		for(size_t idx = 0; idx < ret; ++idx){
			const uint8_t val = in_data[idx];
			uint8_t nibble = 0;

			if (val >= '0' && val <= '9')
				nibble = (val - '0');
			else if (val >= 'a' && val <= 'f')
				nibble = (val - 'a') + 10;
			else if (val >= 'A' && val <= 'F')
				nibble = (val - 'A') + 10;
			else
				continue;

			out_data[out_pos + 0] = (nibble >> 3 & 1) ? '1' : '0';
			out_data[out_pos + 1] = (nibble >> 2 & 1) ? '1' : '0';
			out_data[out_pos + 2] = (nibble >> 1 & 1) ? '1' : '0';
			out_data[out_pos + 3] = (nibble      & 1) ? '1' : '0';
			out_pos += 4;
			bitcount += 4;

			if (space && bitcount == 8) {
				out_data[out_pos++] = ' ';
				bitcount = 0;
			}
		}

		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
  }

	fflush(out);
	free(out_data);
	free(in_data);
}

void hex_encode(FILE *in, FILE *out, const char * opt){
	int space  = has_char(opt, 's');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE >> 2); // 1/4 the size

	size_t ret = 0;

	char byte = 0;
	int bitpos = 0;

  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		size_t out_pos = 0;
		for(size_t idx = 0; idx < ret; ++idx){
			if(in_data[idx] == '1'){
				byte <<= 1;
				byte |= 1;
				++bitpos;
			}

			if(in_data[idx] == '0'){
				byte <<= 1;
				++bitpos;
			}

			if(bitpos == 4){
				out_data[out_pos++] = hex[byte & 0x0F];
				byte = 0;
				bitpos = 0;
				if (space && ++bitcount == 8){
					out_data[out_pos++] = ' ';
					bitcount = 0;
				}
			}
		}
		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
  }

	if(bitpos > 0){ // leftovers ??
		fputc(hex[byte & 0x0F], out);
	}

	fflush(out);
	free(out_data);
	free(in_data);
}

ENGINE_EXPORT(
	ENGINE_ENTRY("hex", 'e', hex_encode, "Convert to hex"),
	ENGINE_ENTRY("hex", 'd', hex_decode, "Convert from hex")
);
