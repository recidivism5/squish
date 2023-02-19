#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef uint32_t u32;
typedef int8_t i8;
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
i8 *readFile(char *path, int *size){
	FILE *f = fopen(path,"rb");
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);
	i8 *b = malloc(*size);
	fread(b, *size, 1, f);
	fclose(f);
	return b;
}
void squish(char *inPath, char *outPath){
	int size;
	i8 *in = readFile(inPath, &size);
	FILE *fout = fopen(outPath,"wb");
    int last = 0;
	for (int i = 0; i < size;){
        i8 start = 0, count = 0;
        i8 *p = in+i;
        for (i8 *w = p-MIN(128,i); w < p; w++){
            int c = 0;
            while (c < 128 && (w[c % (p-w)]==p[c])) c++;
            if (c > count){
                start = w-p;
                count = c;
            }
        }
        if ((i == (size-1)) || (count >= 3) || ((i-last) == 128)){
            if (i-last){
                fputc(i-last-1, fout);
                fwrite(in+last, i-last, 1, fout);
            }
            fputc(start, fout);
			fputc(count, fout);
			i += count;
            last = i;
        } else i++;
	}
	free(in);
	fclose(fout);
}
i8 out[64*1024]; //for the final version we should do a linked list of chunks of some size, probably 1024 bytes
void unsquish(char *inPath, char *outPath){
	int size;
	i8 *in = readFile(inPath, &size), *outp = out;
	int i = 0;
	while (i < size){
		int v = in[i];
        if (v < 0){
            i8 *w = outp+v;
			for (int c = 0; c < in[i+1]; c++){
				*outp = w[c % (-v)];
				outp++;
			}
            i += 2;
        } else {
            i++;
            v++;
            memcpy(outp, in+i, v);
            outp += v;
            i += v;
        }
	}
	FILE *fout = fopen(outPath, "wb");
	fwrite(out, outp-out, 1, fout);
	fclose(fout);
	free(in);
}