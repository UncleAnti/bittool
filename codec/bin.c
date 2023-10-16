#include "../bittool.h"
#include <stdlib.h>

void bin_encode(FILE * in, FILE * out, const char * opt){
	int space  = has_char(opt, 's');

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc((BIT_BUF_SIZE * 8) + ( (BIT_BUF_SIZE / 8) * space) );
	size_t ret = 0;

  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		size_t out_pos = 0;
		for(size_t idx=0; idx < ret; ++idx){
			uint8_t byte = in_data[idx];

			for (size_t bit = 0; bit < 8; ++bit){
				uint8_t val = (byte & 0x80) != 0;
				out_data[out_pos++] = (val ? '1' : '0');
				byte <<= 1;
			}

			if (space)
				out_data[out_pos++] = ' ';
		}
		fwrite(out_data, sizeof(char), out_pos, out);
		out_pos = 0;
  }
	fputc('\n', out);

	fflush(out);

	free(out_data);
	free(in_data);
}

void bin_decode(FILE *in, FILE *out, const char * opt){
	UNUSED(opt);
	int bitcount = 0;
	uint8_t byte = 0;
	uint8_t bit;

	while (fread(&bit, sizeof(bit), 1, in) > 0){
		if (bit == '1'){
			byte <<= 1;
			byte |= 1;
			++bitcount;
		}

		if (bit == '0'){
			byte <<= 1;
			++bitcount;
		}

		if(bitcount == 8) {
			fwrite(&byte, sizeof(byte), 1, out);
			bitcount = 0;
		}
	}

	if(bitcount > 0) {
		fwrite(&byte, sizeof(byte), 1, out);
		bitcount = 0;
	}
}

ENGINE_EXPORT(
	ENGINE_ENTRY("bin", 'e', bin_encode, "Convert to/from Binary."),
	ENGINE_ENTRY("bin", 'd', bin_decode, "Convert to/from Binary.")
);