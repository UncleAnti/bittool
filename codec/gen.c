#include "../bittool.h"
#include <stdlib.h>

void gen_read(FILE * /* in */, FILE * out, const char * opt){
	FILE* in = fopen("read.bin", "r");
	if (!in)
		return;

	int space = has_char(opt, 's');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE); // twice the size

	size_t out_pos = 0;
	size_t ret = 0;
  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		for(size_t idx = 0; idx < ret; ++idx){
			const uint8_t byte = in_data[idx];

			out_data[out_pos++] = byte;

			if(byte == '1' || byte == '0')
				bitcount++;

			if (space && bitcount == 8) {
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
	fclose(in);
}

void gen_write(FILE * in, FILE * /* out */, const char * opt){
	FILE* out = fopen("write.bin", "w");
	if(!out)
		return;

	int space = has_char(opt, 's');
	int bitcount = 0;

  char * in_data  = malloc(BIT_BUF_SIZE);
  char * out_data = malloc(BIT_BUF_SIZE); // twice the size

	size_t out_pos = 0;
	size_t ret = 0;
  while ((ret = fread(in_data, sizeof(char), BIT_BUF_SIZE, in)) > 0){
		for(size_t idx = 0; idx < ret; ++idx){
			const uint8_t byte = in_data[idx];

			out_data[out_pos++] = byte;

			if(byte == '1' || byte == '0')
				bitcount++;

			if (space && bitcount == 8) {
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
	fclose(out);
}

ENGINE_EXPORT(
	ENGINE_ENTRY("read", 'e', gen_read , "reads from read.bin"),
	ENGINE_ENTRY("write",'d', gen_write, "writes to write.bin"),
);
