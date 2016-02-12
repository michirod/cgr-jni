/*
 * chsim.h
 *
 *  Created on: 08 feb 2016
 *      Author: michele
 */

#ifndef ION_CGR_JNI_JNI_INTERFACE_INCLUDE_CHSIM_H_
#define ION_CGR_JNI_JNI_INTERFACE_INCLUDE_CHSIM_H_

void exchangeCurrentDiscoveredContacts(uvast node1, uvast node2);
void newContactDiscovered(uvast neighborNode, unsigned int xmitRate);
void contactLost(uvast neighborNode);
void exchangeContactHistory(uvast node1, uvast node2);
void predictContacts();

#endif /* ION_CGR_JNI_JNI_INTERFACE_INCLUDE_CHSIM_H_ */
