/*
 * ONEtoION_interface.h
 *
 *  Created on: 25 gen 2016
 *      Author: michele
 */

#include <jni.h>

#include "bpP.h"

#ifndef JNI_JNI_INTERFACE_INCLUDE_ONETOION_INTERFACE_H_
#define JNI_JNI_INTERFACE_INCLUDE_ONETOION_INTERFACE_H_

void ion_bundle(Bundle * bundle, jobject message);
void ion_outduct(Outduct * duct, jobject jOutduct);
void init_ouduct_list();
int	getONEDirective(uvast nodeNbr, Object plans, Bundle *bundle,
			FwdDirective *directive);
int cgrForwardONE(jobject bundleONE, jlong terminusNodeNbr);
int bpEnqueONE(FwdDirective *directive, Bundle *bundle, Object bundleObj);
int bpCloneONE(Bundle *oldBundle, Bundle *newBundle, Object *newBundleObj);
int testMessage(jobject message);
int testOutduct(jobject jOuduct);

#endif /* JNI_JNI_INTERFACE_INCLUDE_ONETOION_INTERFACE_H_ */
