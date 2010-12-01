// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "cbasetypes.h"
#include "showmsg.h"
#include "utils.h"
#include "lock.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

// �������݃t�@�C���̕ی쏈��
// �i�������݂��I���܂ŁA���t�@�C����ۊǂ��Ă����j

// �V�����t�@�C���̏������݊J�n
FILE* lock_fopen (const char* filename, int *info) {
	char newfile[512];
	int no = 0;

	// ���S�ȃt�@�C�����𓾂�i�蔲���j
	do {
		sprintf(newfile, "%s_%04d.tmp", filename, ++no);
	} while(exists(newfile) && no < 9999);
	*info = no;
	return fopen(newfile,"w");
}

// ���t�@�C�����폜���V�t�@�C�������l�[��
int lock_fclose (FILE *fp, const char* filename, int *info) {
	int ret = 1;
	char newfile[512];
	char oldfile[512];
	if (fp != NULL) {
		ret = fclose(fp);
		sprintf(newfile, "%s_%04d.tmp", filename, *info);
		sprintf(oldfile, "%s.bak", filename);	// old backup file

		if (exists(oldfile)) remove(oldfile);	// remove backup file if it already exists
		rename (filename, oldfile);				// backup our older data instead of deleting it

		// ���̃^�C�~���O�ŗ�����ƍň��B
		if ((ret = rename(newfile,filename)) != 0) {	// rename our temporary file to its correct name
#if defined(__NETBSD__) || defined(_WIN32) || defined(sun) || defined (_sun) || defined (__sun__)
			ShowError("%s - '"CL_WHITE"%s"CL_RESET"'\n", strerror(errno), newfile);
#else
			char ebuf[255];
			ShowError("%s - '"CL_WHITE"%s"CL_RESET"'\n", strerror_r(errno, ebuf, sizeof(ebuf)), newfile);
#endif
		}
	}
	
	return ret;
}
