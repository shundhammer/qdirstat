/*
 *   File name: BrokenLibc.h
 *   Summary:	Substitutes for common system-level defines
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef BrokenLibc_h
#define BrokenLibc_h

// This contains common #defines that are present on modern systems, but
// sometimes not on systems that insist to exchange known working subsystems
// such as glibc with their own version, commonly because of the "not invented
// here" syndrome.


// Make sure the original defines are available regardless of include order

#include <sys/stat.h>   // ALLPERMS


#ifndef ALLPERMS
#  define ALLPERMS 07777

// Uncomment for debugging:
// #  warning "Using ALLPERMS replacement"

// Not available in musl-libc used on Gentoo:
//
//   https://github.com/shundhammer/qdirstat/issues/187
//
// Original from Linux / glibc /usr/include/x86_64-linux-gnu/sys/stat.h :
//
//   #define ALLPERMS (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)/* 07777 */
//
// But that might induce more complaints because any of S_IRWXU, S_IRWXG
// etc. may also not be defined on such a system. So let's keep it simple.
// If they also use a different bit pattern for those permissions, that's their
// problem.
#endif



#endif  // BrokenLibc_h
