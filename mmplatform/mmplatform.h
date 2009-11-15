#ifndef MMPLATFORM_H
#define MMPLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
setup = 1 for full ipc setup(including procmgr_start for loading the base image)
setup = 2 for ipc setup without loading the base image
*/
void mmplatform_init(int setup);


void mmplatform_deinit();
#endif 
