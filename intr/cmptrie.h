#include "rfd_utils.h"

#ifndef RFD_CORE_H
#define RFD_CORE_H


int init_refdata_info(char *rfd_file, refdata_info **rfd);

int free_refdata_info(refdata_info *rfd);

int is_word_present(refdata_info *rfd, char *word);

//int get_meaning(refdata_info *rfd, char *word, char *meaning);

#endif
