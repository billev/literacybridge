#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char *categories[] = {"Other", "Agriculture", "Health", "Education", "Stories", "Business", "Governance", "Music", "Diary" };

int 
main(int argc, char*argv[])
{
	long wrk, numfields, fieldlen;
	int ret;
	short fid;
	unsigned char nfv;
	unsigned char buf[128];
	int i, j, k, n;

	if(argc == 1) {
		printf("usage: %s .a18 file names\n", argv[0]);
		printf("  for example: %s f1.a18 f2.a18 ...\n", argv[0]);
	}

	for(n = 1; n <= (argc - 1); n++) {

	int fd = open(argv[n], 0);
	if(fd <= 0) {
		fprintf(stderr, "can't open %s\n",argv[n]);
	}
	ret = read(fd, &wrk, 4);
	printf("%s has %ld audio bytes\n",argv[n], wrk);
	wrk = lseek(fd, wrk + 4, SEEK_SET);
	ret = read(fd, &wrk, 4);
	printf("meta data version=%ld\n", wrk);
	ret = read(fd, &numfields, 4);
	printf("num fields=%ld\n", numfields);

	for(i=0; i<(int)numfields; i++) {
		ret = read(fd, &fid, 2);
		printf("\n  field id=%d\n",fid);
		ret = read(fd, &fieldlen, 4);
		printf("    filed length=%d\n", fieldlen);
		ret = read(fd, &nfv, 1);
		printf("    num field values=%d\n", nfv);
		for(j=0; j<nfv; j++) {
			short fl;
			ret = read(fd, &fl, 2);
			printf("    field value length[%d]=%d\n",j,fl);
			ret = read(fd, buf, fl);
			buf[fl] = 0;
			printf("    field value[%d]=",j);
/*			for(k=0; k<fl; k++) {
				printf("0x%.2x ", buf[k]);
			}
			printf("\n");
*/
			printf("'%s'",buf);
			if(fid == 0) { // categories
				unsigned int m = buf[0] - '0';
				if((m >= 0 && m < 9)) {
					printf(" (%s) ", categories[m]);
				}
			}
			printf("\n");
		}
	}
	close(fd);
	printf("\n\n");
	}
}
