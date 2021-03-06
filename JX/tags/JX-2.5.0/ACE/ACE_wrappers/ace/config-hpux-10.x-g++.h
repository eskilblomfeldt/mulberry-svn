/* -*- C++ -*- */
// config-hpux-10.x-g++.h,v 4.26 2004/06/13 10:16:42 jwillemsen Exp

// The following configuration file is designed to work for HP
// platforms running HP/UX 10.x using G++.

#ifndef ACE_CONFIG_H
#define ACE_CONFIG_H
#include /**/ "ace/pre.h"

// config-g++-common.h undef's ACE_HAS_STRING_CLASS with -frepo, so
// this must appear before its #include.
#define ACE_HAS_STRING_CLASS

#include "ace/config-g++-common.h"

// These are apparantly some things which are special to g++ on HP?  They are
// compiler-related settings, but not in config-g++-common.h

#define ACE_HAS_BROKEN_CONVERSIONS
// Compiler supports the ssize_t typedef.
#define ACE_HAS_SSIZE_T
#define _CLOCKID_T

// We have to explicitly instantiate static template members
# define ACE_HAS_EXPLICIT_STATIC_TEMPLATE_MEMBER_INSTANTIATION

#include "ace/config-hpux-10.x.h"

#include /**/ "ace/post.h"
#endif /* ACE_CONFIG_H */
